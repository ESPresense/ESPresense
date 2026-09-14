#include "mqtt.h"

#include <memory>

#include "BleFingerprintCollection.h"
#include "GUI.h"
#include "Network.h"
#include "defaults.h"
#include "esp_chip_info.h"
#include "globals.h"
#include "mqtt_client.h"
#include "string_utils.h"

// Defined in main.cpp: full-message dispatch and the online flag.
void onMqttMessage(const char* topic, const char* payload);
extern bool online;
extern int reconnectTries;
extern std::string setTopic, configTopic;

namespace Mqtt {
namespace {
esp_mqtt_client_handle_t client = nullptr;
bool isConnected = false;
std::string topicBuffer, payloadBuffer;

void onEvent(void*, esp_event_base_t, int32_t id, void* data) {
    auto* ev = (esp_mqtt_event_handle_t)data;
    switch ((esp_mqtt_event_id_t)id) {
        case MQTT_EVENT_CONNECTED:
            isConnected = true;
            esp_mqtt_client_subscribe(client, "espresense/rooms/*/+/set", 1);
            esp_mqtt_client_subscribe(client, setTopic.c_str(), 1);
            esp_mqtt_client_subscribe(client, configTopic.c_str(), 1);
            GUI::Connected(true, true);
            break;
        case MQTT_EVENT_DISCONNECTED:
            isConnected = false;
            online = false;
            GUI::Connected(true, false);
            Log.println("Disconnected from MQTT");
            break;
        case MQTT_EVENT_DATA:
            // Large messages arrive in chunks; the topic only comes with the first one.
            if (ev->current_data_offset == 0) {
                topicBuffer.assign(ev->topic, ev->topic_len);
                payloadBuffer.clear();
                payloadBuffer.reserve(ev->total_data_len);
            }
            payloadBuffer.append(ev->data, ev->data_len);
            if (ev->current_data_offset + ev->data_len >= ev->total_data_len) {
                onMqttMessage(topicBuffer.c_str(), payloadBuffer.c_str());
                payloadBuffer.clear();
                payloadBuffer.shrink_to_fit();
            }
            break;
        default:
            break;
    }
}
}  // namespace

void Setup(const std::string& host, uint16_t port, const std::string& user, const std::string& pass,
           const std::string& clientId, const std::string& willTopic) {
    static std::string sHost = host, sUser = user, sPass = pass, sClient = clientId, sWill = willTopic;
    esp_mqtt_client_config_t cfg = {};
    cfg.broker.address.hostname = sHost.c_str();
    cfg.broker.address.port = port;
    cfg.broker.address.transport = MQTT_TRANSPORT_OVER_TCP;
    cfg.credentials.client_id = sClient.c_str();
    if (!sUser.empty()) cfg.credentials.username = sUser.c_str();
    if (!sPass.empty()) cfg.credentials.authentication.password = sPass.c_str();
    cfg.session.last_will.topic = sWill.c_str();
    cfg.session.last_will.msg = "offline";
    cfg.session.last_will.retain = true;
    cfg.session.keepalive = 60;
    cfg.network.reconnect_timeout_ms = 3000;
    cfg.network.timeout_ms = 10000;
    cfg.buffer.size = 1024;
    cfg.task.stack_size = 5120;
    client = esp_mqtt_client_init(&cfg);
    esp_mqtt_client_register_event(client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, onEvent, nullptr);
    esp_mqtt_client_start(client);
}

bool connected() { return isConnected; }

void reconnect() {
    if (client && !isConnected) esp_mqtt_client_reconnect(client);
}
}  // namespace Mqtt

bool pub(const char* topic, uint8_t qos, bool retain, const char* payload, size_t length) {
    if (!Mqtt::client) return false;
    if (freeHeap() < MQTT_MIN_FREE_MEMORY) return false;
    for (int i = 0; i < 10; i++) {
        if (!Mqtt::isConnected) return false;
        if (esp_mqtt_client_publish(Mqtt::client, topic, payload, length, qos, retain) >= 0) return true;
        delay(25);
    }
    return false;
}

bool pub(const char* topic, uint8_t qos, bool retain, JsonVariantConst jsonDoc) {
    // Heap, not a VLA on the caller's task stack; refuse on a size mismatch rather than
    // publish a truncated document.
    size_t const jsonSize = measureJson(jsonDoc);
    std::unique_ptr<char[]> buffer(new (std::nothrow) char[jsonSize + 1]);
    if (!buffer) return false;
    size_t const buffSize = serializeJson(jsonDoc, buffer.get(), jsonSize + 1);
    if (buffSize == 0 || buffSize != jsonSize) return false;
    return pub(topic, qos, retain, buffer.get(), buffSize);
}

static const char* chipModel() {
    esp_chip_info_t info;
    esp_chip_info(&info);
    switch (info.model) {
        case CHIP_ESP32: return "ESP32";
        case CHIP_ESP32S2: return "ESP32-S2";
        case CHIP_ESP32S3: return "ESP32-S3";
        case CHIP_ESP32C3: return "ESP32-C3";
        case CHIP_ESP32C6: return "ESP32-C6";
        case CHIP_ESP32H2: return "ESP32-H2";
        default: return "ESP32";
    }
}

void commonDiscovery() {
    doc.clear();
    auto identifiers = doc["dev"].createNestedArray("ids");
    identifiers.add(Sprintf("espresense_%06x", (unsigned)CHIPID));
    auto connections = doc["dev"].createNestedArray("cns");
    auto mac = connections.createNestedArray();
    mac.add("mac");
    mac.add(Network::macAddress());
    doc["dev"]["name"] = "ESPresense " + room;
    doc["dev"]["sa"] = room;
#ifdef VERSION
    doc["dev"]["sw"] = VERSION;
#endif
#ifdef FIRMWARE
    doc["dev"]["mf"] = "ESPresense (" FIRMWARE ")";
#endif
    doc["dev"]["cu"] = "http://" + localIp;
    doc["dev"]["mdl"] = chipModel();
}

bool sendConnectivityDiscovery() {
    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = "Connectivity";
    doc["uniq_id"] = Sprintf("espresense_%06x_connectivity", (unsigned)CHIPID);
    doc["json_attr_t"] = "~/telemetry";
    doc["stat_t"] = "~/status";
    doc["dev_cla"] = "connectivity";
    doc["pl_on"] = "online";
    doc["pl_off"] = "offline";
    const std::string discoveryTopic = Sprintf("%s/binary_sensor/espresense_%06x/connectivity/config", homeAssistantDiscoveryPrefix.c_str(), (unsigned)CHIPID);
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendTeleBinarySensorDiscovery(const std::string& name, const std::string& entityCategory, const std::string& temp, const std::string& devClass) {
    auto slug = slugify(name);
    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = name;
    doc["uniq_id"] = Sprintf("espresense_%06x_%s", (unsigned)CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/telemetry";
    doc["value_template"] = temp;
    if (!entityCategory.empty()) doc["entity_category"] = entityCategory;
    if (!devClass.empty()) doc["dev_cla"] = devClass;
    const std::string discoveryTopic = Sprintf("%s/binary_sensor/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(), (unsigned)CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendTeleSensorDiscovery(const std::string& name, const std::string& entityCategory, const std::string& temp, const std::string& devClass, const std::string& units) {
    auto slug = slugify(name);
    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = name;
    doc["uniq_id"] = Sprintf("espresense_%06x_%s", (unsigned)CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/telemetry";
    doc["value_template"] = temp;
    if (!entityCategory.empty()) doc["entity_category"] = entityCategory;
    if (!units.empty()) doc["unit_of_meas"] = units;
    if (!devClass.empty()) doc["dev_cla"] = devClass;
    const std::string discoveryTopic = Sprintf("%s/sensor/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(), (unsigned)CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendSensorDiscovery(const std::string& name, const std::string& entityCategory, const std::string& devClass, const std::string& units, bool frcUpdate) {
    auto slug = slugify(name);
    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = name;
    doc["uniq_id"] = Sprintf("espresense_%06x_%s", (unsigned)CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/" + slug;
    if (!entityCategory.empty()) doc["entity_category"] = entityCategory;
    if (!units.empty()) doc["unit_of_meas"] = units;
    if (!devClass.empty()) doc["dev_cla"] = devClass;
    doc["frc_upd"] = frcUpdate;
    const std::string discoveryTopic = Sprintf("%s/sensor/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(), (unsigned)CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendBinarySensorDiscovery(const std::string& name, const std::string& entityCategory, const std::string& devClass) {
    auto slug = slugify(name);
    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = name;
    doc["uniq_id"] = Sprintf("espresense_%06x_%s", (unsigned)CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/" + slug;
    if (!entityCategory.empty()) doc["entity_category"] = entityCategory;
    if (!devClass.empty()) doc["dev_cla"] = devClass;
    const std::string discoveryTopic = Sprintf("%s/binary_sensor/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(), (unsigned)CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendButtonDiscovery(const std::string& name, const std::string& entityCategory) {
    auto slug = slugify(name);
    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = name;
    doc["uniq_id"] = Sprintf("espresense_%06x_%s", (unsigned)CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/" + slug;
    doc["cmd_t"] = "~/" + slug + "/set";
    if (!entityCategory.empty()) doc["entity_category"] = entityCategory;
    const std::string discoveryTopic = Sprintf("%s/button/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(), (unsigned)CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendSwitchDiscovery(const std::string& name, const std::string& entityCategory) {
    auto slug = slugify(name);
    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = name;
    doc["uniq_id"] = Sprintf("espresense_%06x_%s", (unsigned)CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/" + slug;
    doc["cmd_t"] = "~/" + slug + "/set";
    doc["entity_category"] = entityCategory;
    const std::string discoveryTopic = Sprintf("%s/switch/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(), (unsigned)CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendNumberDiscovery(const std::string& name, const std::string& entityCategory) {
    auto slug = slugify(name);
    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = name;
    doc["uniq_id"] = Sprintf("espresense_%06x_%s", (unsigned)CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/" + slug;
    doc["cmd_t"] = "~/" + slug + "/set";
    doc["step"] = "0.1";
    if (!entityCategory.empty()) doc["entity_category"] = entityCategory;
    const std::string discoveryTopic = Sprintf("%s/number/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(), (unsigned)CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendLightDiscovery(const std::string& name, const std::string& entityCategory, bool rgb, bool rgbw) {
    auto slug = slugify(name);
    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = name;
    doc["uniq_id"] = Sprintf("espresense_%06x_%s", (unsigned)CHIPID, slug.c_str());
    doc["schema"] = "json";
    doc["stat_t"] = "~/" + slug;
    doc["cmd_t"] = "~/" + slug + "/set";
    doc["brightness"] = true;
    doc["supported_color_modes"][0] = rgbw ? "rgbw" : rgb ? "rgb" : "brightness";
    if (!entityCategory.empty()) doc["entity_category"] = entityCategory;
    const std::string discoveryTopic = Sprintf("%s/light/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(), (unsigned)CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendDeleteDiscovery(const std::string& domain, const std::string& name) {
    auto slug = slugify(name);
    const std::string discoveryTopic = Sprintf("%s/%s/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(), domain.c_str(), (unsigned)CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, false, "");
}

bool sendConfig(const std::string& id, const std::string& alias, const std::string& name, int calRssi) {
    DeviceConfig existing;
    if (BleFingerprintCollection::FindDeviceConfigByAlias(alias, existing) && existing.id != id)
        deleteConfig(existing.id);
    Log.printf("%u Alias  | %s to %s\r\n", (unsigned)xPortGetCoreID(), id.c_str(), alias.c_str());
    DynamicJsonDocument json(256);
    json["id"] = alias;
    json["name"] = name;
    if (calRssi > NO_RSSI) json["rssi@1m"] = calRssi;
    const std::string settingsTopic = CHANNEL + std::string("/settings/") + id + "/config";
    return pub(settingsTopic.c_str(), 0, true, json);
}

bool deleteConfig(const std::string& id) {
    Log.printf("%u Delete | %s\r\n", (unsigned)xPortGetCoreID(), id.c_str());
    const std::string settingsTopic = CHANNEL + std::string("/settings/") + id + "/config";
    return pub(settingsTopic.c_str(), 0, true, "");
}
