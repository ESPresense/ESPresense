#include "globals.h"
#include "defaults.h"
#include "string_utils.h"
#include <WiFi.h>

bool pub(const char *topic, uint8_t qos, bool retain, const char *payload, size_t length = 0, bool dup = false, uint16_t message_id = 0)
{
    for (int i = 0; i < 10; i++)
    {
        if (mqttClient.publish(topic, qos, retain, payload, length, dup, message_id))
            return true;
        delay(25);
    }
    return false;
}

bool pub(const char *topic, uint8_t qos, bool retain, JsonVariantConst jsonDoc, bool dup = false, uint16_t message_id = 0)
{
    size_t const jsonSize = measureJson(jsonDoc);
    char buffer[jsonSize + 1]; // +1 for null terminator
    size_t const buffSize = serializeJson(jsonDoc, buffer, sizeof(buffer));
    if (buffSize == 0) return false;
    return pub(topic, qos, retain, buffer, buffSize, dup, message_id);
}

void commonDiscovery()
{
    doc.clear();
    auto identifiers = doc["dev"].createNestedArray("ids");
    identifiers.add(Sprintf("espresense_%06x", CHIPID));
    auto connections = doc["dev"].createNestedArray("cns");
    auto mac = connections.createNestedArray();
    mac.add("mac");
    mac.add(WiFi.macAddress());
    doc["dev"]["name"] = "ESPresense " + room;
    doc["dev"]["sa"] = room;
#ifdef VERSION
    doc["dev"]["sw"] = VERSION;
#endif
#ifdef FIRMWARE
    doc["dev"]["mf"] = "ESPresense (" FIRMWARE ")";
#endif
    doc["dev"]["cu"] = "http://" + localIp;
    doc["dev"]["mdl"] = String(ESP.getChipModel());
}

bool sendConnectivityDiscovery()
{
    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = "Connectivity";
    doc["uniq_id"] = Sprintf("espresense_%06x_connectivity", CHIPID);
    doc["json_attr_t"] = "~/telemetry";
    doc["stat_t"] = "~/status";
    doc["dev_cla"] = "connectivity";
    doc["pl_on"] = "online";
    doc["pl_off"] = "offline";

    const String discoveryTopic = Sprintf("%s/binary_sensor/espresense_%06x/connectivity/config", homeAssistantDiscoveryPrefix.c_str(), CHIPID);
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendTeleBinarySensorDiscovery(const String &name, const String &entityCategory, const String &temp, const String &devClass = "")
{
    auto slug = slugify(name);

    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = name;
    doc["uniq_id"] = Sprintf("espresense_%06x_%s", CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/telemetry";
    doc["value_template"] = temp;
    if (!entityCategory.isEmpty()) doc["entity_category"] = entityCategory;
    if (!devClass.isEmpty()) doc["dev_cla"] = devClass;

    const String discoveryTopic = Sprintf("%s/binary_sensor/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(), CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendTeleSensorDiscovery(const String &name, const String &entityCategory, const String &temp, const String &devClass = "", const String &units = "")
{
    auto slug = slugify(name);

    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = name;
    doc["uniq_id"] = Sprintf("espresense_%06x_%s", CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/telemetry";
    doc["value_template"] = temp;
    if (!entityCategory.isEmpty()) doc["entity_category"] = entityCategory;
    if (!units.isEmpty()) doc["unit_of_meas"] = units;
    if (!devClass.isEmpty()) doc["dev_cla"] = devClass;

    const String discoveryTopic = Sprintf("%s/sensor/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(),CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendSensorDiscovery(const String &name, const String &entityCategory, const String &devClass = "", const String &units = "", bool frcUpdate = true)
{
    auto slug = slugify(name);

    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = name;
    doc["uniq_id"] = Sprintf("espresense_%06x_%s", CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/" + slug;
    if (!entityCategory.isEmpty()) doc["entity_category"] = entityCategory;
    if (!units.isEmpty()) doc["unit_of_meas"] = units;
    if (!devClass.isEmpty()) doc["dev_cla"] = devClass;
    doc["frc_upd"] = frcUpdate;

    const String discoveryTopic = Sprintf("%s/sensor/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(), CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendBinarySensorDiscovery(const String &name, const String &entityCategory, const String &devClass = "")
{
    auto slug = slugify(name);

    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = name;
    doc["uniq_id"] = Sprintf("espresense_%06x_%s", CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/" + slug;
    if (!entityCategory.isEmpty()) doc["entity_category"] = entityCategory;
    if (!devClass.isEmpty()) doc["dev_cla"] = devClass;

    const String discoveryTopic = Sprintf("%s/binary_sensor/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(), CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendButtonDiscovery(const String &name, const String &entityCategory)
{
    auto slug = slugify(name);

    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = name;
    doc["uniq_id"] = Sprintf("espresense_%06x_%s", CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/" + slug;
    doc["cmd_t"] = "~/" + slug + "/set";
    if (!entityCategory.isEmpty()) doc["entity_category"] = entityCategory;

    const String discoveryTopic = Sprintf("%s/button/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(), CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendSwitchDiscovery(const String &name, const String &entityCategory)
{
    auto slug = slugify(name);

    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = name;
    doc["uniq_id"] = Sprintf("espresense_%06x_%s", CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/" + slug;
    doc["cmd_t"] = "~/" + slug + "/set";
    doc["entity_category"] = entityCategory;

    String const discoveryTopic = Sprintf("%s/switch/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(), CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendNumberDiscovery(const String &name, const String &entityCategory)
{
    auto slug = slugify(name);

    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = name;
    doc["uniq_id"] = Sprintf("espresense_%06x_%s", CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/" + slug;
    doc["cmd_t"] = "~/" + slug + "/set";
    doc["step"] = "0.1";
    if (!entityCategory.isEmpty()) doc["entity_category"] = entityCategory;

    const String discoveryTopic = Sprintf("%s/number/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(), CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendLightDiscovery(const String &name, const String &entityCategory, bool rgb, bool rgbw)
{
    auto slug = slugify(name);

    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = name;
    doc["uniq_id"] = Sprintf("espresense_%06x_%s", CHIPID, slug.c_str());
    doc["schema"] = "json";
    doc["stat_t"] = "~/" + slug;
    doc["cmd_t"] = "~/" + slug + "/set";
    doc["brightness"] = true;

    if (rgbw) {
        doc["supported_color_modes"][0] = "rgbw";
    } else if (rgb) {
        doc["supported_color_modes"][0] = "rgb";
    } else {
        doc["supported_color_modes"][0] = "brightness";
    }

    if (!entityCategory.isEmpty()) doc["entity_category"] = entityCategory;

    const String discoveryTopic = Sprintf("%s/light/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(), CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, doc);
}

bool sendDeleteDiscovery(const String &domain, const String &name)
{
    auto slug = slugify(name);
    const String discoveryTopic = Sprintf("%s/%s/espresense_%06x/%s/config", homeAssistantDiscoveryPrefix.c_str(), domain.c_str(), CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, false, "");
}

bool sendConfig(const String &id, const String &alias, const String &name = "", int calRssi = -128)
{
    Serial.printf("%u Alias  | %s to %s\r\n", xPortGetCoreID(), id.c_str(), alias.c_str());
    doc.clear();
    doc["id"] = alias;
    doc["name"] = name;
    if (calRssi > NO_RSSI) doc["rssi@1m"] = calRssi;

    const String settingsTopic = CHANNEL + String("/settings/") + id + "/config";
    return pub(settingsTopic.c_str(), 0, true, doc);
}

bool deleteConfig(const String &id)
{
    Serial.printf("%u Delete | %s\r\n", xPortGetCoreID(), id.c_str());
    const String settingsTopic = CHANNEL + String("/settings/") + id + "/config";
    return pub(settingsTopic.c_str(), 0, true, "");
}
