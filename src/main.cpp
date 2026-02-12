#define VAR_DECLS
#include "main.h"

#include "esp_heap_caps.h"

void heapCapsAllocFailedHook(size_t requestedSize, uint32_t caps, const char *functionName)
{
    printf("%s was called but failed to allocate %d bytes with 0x%X capabilities. \n",functionName, requestedSize, caps);
}

/**
 * @brief Publish device telemetry and perform online/discovery announcements.
 *
 * Attempts to send status and discovery payloads when the device is not marked online
 * or discovery has not yet been announced, then builds and publishes a telemetry
 * JSON document containing device state, diagnostics, and aggregated counters.
 * Successful status/discovery sends update internal flags such as `online` and
 * `sentDiscovery`; failed telemetry publishes increment internal failure counters.
 *
 * @param totalSeen Total advertisement packets observed since the last report.
 * @param totalFpSeen Total distinct fingerprints seen since the last report.
 * @param totalFpQueried Total fingerprints queried (e.g., looked up) since the last report.
 * @param totalFpReported Total fingerprint reports published since the last report.
 * @param count Current count value (included only when a count identifier is configured).
 * @return `true` if the telemetry document was published successfully, `false` otherwise. 
 * `false` is also returned when telemetry publishing is disabled or the function is rate-limited.
 */
bool sendTelemetry(unsigned int totalSeen, unsigned int totalFpSeen, unsigned int totalFpQueried, unsigned int totalFpReported, unsigned int count) {
    if (!online) {
        if (
            pub(statusTopic.c_str(), 0, true, "online")
            && pub((roomsTopic + "/name").c_str(), 0, true, room.c_str())
            && pub((roomsTopic + "/max_distance").c_str(), 0, true, String(BleFingerprintCollection::maxDistance).c_str())
            && pub((roomsTopic + "/absorption").c_str(), 0, true, String(BleFingerprintCollection::absorption).c_str())
            && pub((roomsTopic + "/tx_ref_rssi").c_str(), 0, true, String(BleFingerprintCollection::txRefRssi).c_str())
            && pub((roomsTopic + "/rx_adj_rssi").c_str(), 0, true, String(BleFingerprintCollection::rxAdjRssi).c_str())
            && pub((roomsTopic + "/query").c_str(), 0, true, BleFingerprintCollection::query.c_str())
            && pub((roomsTopic + "/include").c_str(), 0, true, BleFingerprintCollection::include.c_str())
            && pub((roomsTopic + "/exclude").c_str(), 0, true, BleFingerprintCollection::exclude.c_str())
            && pub((roomsTopic + "/known_macs").c_str(), 0, true, BleFingerprintCollection::knownMacs.c_str())
            && pub((roomsTopic + "/known_irks").c_str(), 0, true, BleFingerprintCollection::knownIrks.c_str())
            && pub((roomsTopic + "/count_ids").c_str(), 0, true, BleFingerprintCollection::countIds.c_str())
            && Updater::SendOnline()
            && Motion::SendOnline()
            && Switch::SendOnline()
            && Button::SendOnline()
            && GUI::SendOnline()
        ) {
            online = true;
            reconnectTries = 0;
        } else {
            Log.println("Error sending status=online");
        }
    }

    if (discovery && !sentDiscovery) {
        if (sendConnectivityDiscovery()
            && sendTeleSensorDiscovery("Uptime", EC_DIAGNOSTIC, "{{ value_json.uptime }}", DEVICE_CLASS_NONE, "s")
            && sendTeleSensorDiscovery("Free Mem", EC_DIAGNOSTIC, "{{ value_json.freeHeap }}", DEVICE_CLASS_NONE, "bytes")
            && (BleFingerprintCollection::countIds.isEmpty() ? sendDeleteDiscovery("sensor", "Count") : sendTeleSensorDiscovery("Count", EC_NONE, "{{ value_json.count }}"))
            && sendButtonDiscovery("Restart", EC_DIAGNOSTIC)
            && sendNumberDiscovery("Max Distance", EC_CONFIG)
            && sendNumberDiscovery("Absorption", EC_CONFIG)

            && Updater::SendDiscovery()
            && GUI::SendDiscovery()
            && Motion::SendDiscovery()
            && Switch::SendDiscovery()
            && Button::SendDiscovery()
            && Enrollment::SendDiscovery()
            && Battery::SendDiscovery()
            && CAN::SendDiscovery()
#ifdef SENSORS
            && DHT::SendDiscovery()
            && AHTX0::SendDiscovery()
            && BH1750::SendDiscovery()
            && BME280::SendDiscovery()
            && BMP180::SendDiscovery()
            && BMP280::SendDiscovery()
            && SHT::SendDiscovery()
            && TSL2561::SendDiscovery()
            && SensirionSGP30::SendDiscovery()
            && SensirionSCD4x::SendDiscovery()
            && HX711::SendDiscovery()
            && DS18B20::SendDiscovery()
#endif
        ) {
            sentDiscovery = true;
        } else {
            Log.println("Error sending discovery");
        }
    }

    if (!publishTele)
        return false;
    auto now = millis();

    if (now - lastTeleMillis < 15000)
        return false;

    lastTeleMillis = now;

    doc.clear();
    doc["ip"] = localIp;
    doc["uptime"] = esp_timer_get_time() / 1000000;
#ifdef FIRMWARE
    doc["firm"] = String(FIRMWARE);
#endif
    doc["rssi"] = WiFi.RSSI();
    Battery::SendTelemetry();

#ifdef VERSION
    doc["ver"] = String(VERSION);
#else
    doc["ver"] = ESP.getSketchMD5() + "-" + getBuildTimestamp();
#endif

    if (!BleFingerprintCollection::countIds.isEmpty())
        doc["count"] = count;
    if (totalSeen > 0)
        doc["adverts"] = totalSeen;
    if (totalFpSeen > 0)
        doc["seen"] = totalFpSeen;
    if (totalFpQueried > 0)
        doc["queried"] = totalFpQueried;
    if (totalFpReported > 0)
        doc["reported"] = totalFpReported;
    if (reportFailed > 0)
        doc["failed"] = reportFailed;
    if (teleFails > 0)
        doc["teleFails"] = teleFails;
    if (reconnectTries > 0)
        doc["reconnectTries"] = reconnectTries;
    auto maxHeap = ESP.getMaxAllocHeap();
    auto freeHeap = ESP.getFreeHeap();
    doc["freeHeap"] = freeHeap;
    doc["maxHeap"] = maxHeap;
    doc["scanStack"] = uxTaskGetStackHighWaterMark(scanTaskHandle);
    doc["loopStack"] = uxTaskGetStackHighWaterMark(nullptr);
    doc["bleStack"] = bleStack;

    if (pub(teleTopic.c_str(), 0, false, doc)) return true;

    teleFails++;
    log_e("Error after 10 tries sending telemetry (%d times since boot)", teleFails);
    return false;
}

/**
 * @brief Configure network settings, initialize network-connected subsystems, and establish connectivity.
 *
 * Reads runtime settings (Wi‑Fi, Ethernet type, MQTT, discovery/publication flags, timeouts, hostname, room),
 * initializes and connects subsystem components that require network access, registers wait-loop callbacks for
 * captive portal and connection progress, and attempts to bring up Ethernet or Wi‑Fi. Logs network and device
 * information and prepares MQTT/topic strings for later use.
 *
 * On unrecoverable connection failure this function will reboot the device (calls ESP.restart()). The captive
 * portal wait callback can also trigger a restart if the portal timeout elapses.
 */
void setupNetwork() {
    Log.println("Setup network");
    WiFi.persistent(false);
    WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);
    GUI::Connected(false, false);

    room = HeadlessWiFiSettings.string("room", ESPMAC, "Room");
    HeadlessWiFiSettings.string("wifi-ssid", "", "WiFi SSID");
    HeadlessWiFiSettings.pstring("wifi-password", "", "WiFi Password");
    auto wifiTimeout = HeadlessWiFiSettings.integer("wifi_timeout", DEFAULT_WIFI_TIMEOUT, "Seconds to wait for WiFi before captive portal (-1 = forever)");
    auto portalTimeout = 1000UL * HeadlessWiFiSettings.integer("portal_timeout", DEFAULT_PORTAL_TIMEOUT, "Seconds to wait in captive portal before rebooting");
    std::vector<String> ethernetTypes = {"None", "WT32-ETH01", "ESP32-POE", "WESP32", "QuinLED-ESP32", "TwilightLord-ESP32", "ESP32Deux", "KIT-VE", "LilyGO-T-ETH-POE", "GL-inet GL-S10 v2.1 Ethernet", "EST-PoE-32", "LilyGO-T-ETH-Lite (RTL8201)", "ESP32-POE_A1"};
    ethernetType = HeadlessWiFiSettings.dropdown("eth", ethernetTypes, 0, "Ethernet Type");

    mqttHost = HeadlessWiFiSettings.string("mqtt_host", DEFAULT_MQTT_HOST, "Server");
    mqttPort = HeadlessWiFiSettings.integer("mqtt_port", DEFAULT_MQTT_PORT, "Port");
    mqttUser = HeadlessWiFiSettings.pstring("mqtt_user", DEFAULT_MQTT_USER, "Username");
    mqttPass = HeadlessWiFiSettings.pstring("mqtt_pass", DEFAULT_MQTT_PASSWORD, "Password");
    discovery = HeadlessWiFiSettings.checkbox("discovery", true, "Send to discovery topic");
    homeAssistantDiscoveryPrefix = HeadlessWiFiSettings.string("discovery_prefix", DEFAULT_HA_DISCOVERY_PREFIX, "Home Assistant discovery topic prefix");
    publishTele = HeadlessWiFiSettings.checkbox("pub_tele", true, "Send to telemetry topic");
    publishDevices = HeadlessWiFiSettings.checkbox("pub_devices", true, "Send to devices topic");

    Updater::ConnectToWifi();

    // Mark endpoint boundary: settings registered after this belong to /wifi/extras endpoint
    HeadlessWiFiSettings.markExtra();

    // Register BLE scanning and fingerprinting settings (part of extras endpoint)
    BleFingerprintCollection::ConnectToWifi();

    // Mark endpoint boundary: settings registered after this belong to /wifi/hardware endpoint
    // IMPORTANT: This order matters! BleFingerprintCollection must be registered before
    // this line to ensure BLE settings appear in /wifi/extras, not /wifi/hardware
    HeadlessWiFiSettings.markEndpoint("hardware");

    // Register hardware settings (LEDs, PIR, I2C, etc.) - part of hardware endpoint
    GUI::ConnectToWifi();

    Motion::ConnectToWifi();
    Switch::ConnectToWifi();
    Button::ConnectToWifi();

#ifdef SENSORS
    DHT::ConnectToWifi();
    I2C::ConnectToWifi();

    AHTX0::ConnectToWifi();
    BH1750::ConnectToWifi();
    BME280::ConnectToWifi();
    BMP180::ConnectToWifi();
    BMP280::ConnectToWifi();
    SHT::ConnectToWifi();
    TSL2561::ConnectToWifi();
    SensirionSGP30::ConnectToWifi();
    SensirionSCD4x::ConnectToWifi();
    HX711::ConnectToWifi();
    DS18B20::ConnectToWifi();
#endif

    unsigned int connectProgress = 0;
    HeadlessWiFiSettings.onWaitLoop = [&connectProgress]() {
        GUI::Wifi(connectProgress++);
        SerialImprov::Loop(true);
        return 50;
    };
    unsigned int portalProgress = 0;
    HeadlessWiFiSettings.onPortalWaitLoop = [&portalProgress, portalTimeout]() {
        GUI::Portal(portalProgress++);
        SerialImprov::Loop(false);

        if (millis() > portalTimeout)
            ESP.restart();

        return 50;
    };
    HeadlessWiFiSettings.onHttpSetup = HttpWebServer::Init;
    HeadlessWiFiSettings.hostname = "espresense-" + kebabify(room);

    bool success = false;
    if (ethernetType > 0) success = Network.connect(ethernetType, 20, HeadlessWiFiSettings.hostname.c_str());
    if (!success && !HeadlessWiFiSettings.connect(true, wifiTimeout))
        ESP.restart();

    GUI::Connected(true, false);

#ifdef FIRMWARE
    Log.println("Firmware:     " + String(FIRMWARE));
#endif
#ifdef VERSION
    Log.println("Version:      " + String(VERSION));
#endif
    Log.printf("WiFi BSSID:   %s (channel=%d rssi=%d)\r\n", WiFi.BSSIDstr().c_str(), WiFi.channel(), WiFi.RSSI());
    Log.print("IP address:   ");
    Log.println(Network.localIP());
    Log.print("DNS address:  ");
    Log.println(Network.dnsIP());
    Log.print("Hostname:     ");
    Log.println(Network.getHostname());
    Log.print("Room:         ");
    Log.println(room);
    Log.printf("Mqtt server:  %s:%d\r\n", mqttHost.c_str(), mqttPort);
    Log.printf("Max Distance: %.2f\r\n", BleFingerprintCollection::maxDistance);
    GUI::SerialReport();
    Motion::SerialReport();
    Switch::SerialReport();
    Button::SerialReport();
#ifdef SENSORS
    I2C::SerialReport();
    DHT::SerialReport();
    AHTX0::SerialReport();
    BH1750::SerialReport();
    BME280::SerialReport();
    BMP180::SerialReport();
    BMP280::SerialReport();
    SHT::SerialReport();
    TSL2561::SerialReport();
    SensirionSGP30::SerialReport();
    SensirionSCD4x::SerialReport();
    HX711::SerialReport();
    DS18B20::SerialReport();

#endif
    Log.print("Query:        ");
    Log.println(BleFingerprintCollection::query);
    Log.print("Include:      ");
    Log.println(BleFingerprintCollection::include);
    Log.print("Exclude:      ");
    Log.println(BleFingerprintCollection::exclude);
    Log.print("Known Macs:   ");
    Log.println(BleFingerprintCollection::knownMacs);
    Log.print("Count Ids:    ");
    Log.println(BleFingerprintCollection::countIds);

    localIp = Network.localIP().toString();
    id = slugify(room);
    roomsTopic = CHANNEL + String("/rooms/") + id;
    statusTopic = roomsTopic + "/status";
    teleTopic = roomsTopic + "/telemetry";
    setTopic = roomsTopic + "/+/set";
    configTopic = CHANNEL + String("/settings/+/config");
    HeadlessWiFiSettings.httpSetup();
    Updater::MarkOtaSuccess();
}

void onMqttConnect(bool sessionPresent) {
    xTimerStop(reconnectTimer, 0);
    mqttClient.subscribe("espresense/rooms/*/+/set", 1);
    mqttClient.subscribe(setTopic.c_str(), 1);
    mqttClient.subscribe(configTopic.c_str(), 1);
    GUI::Connected(true, true);
}

/**
 * @brief Handle MQTT disconnection events and initiate reconnection.
 *
 * Updates the GUI to show disconnected status, logs the disconnect reason, starts the reconnect timer, and marks the device as offline.
 *
 * @param reason The MQTT client's disconnection reason code.
 */
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    GUI::Connected(true, false);
    Log.printf("Disconnected from MQTT; reason %d\r\n", (int)reason);
    xTimerStart(reconnectTimer, 0);
    online = false;
}

/**
 * @brief Handle an incoming MQTT message by routing configuration or command topics.
 *
 * Parses the topic to detect trailing "/config" or "/set" paths. For "/config" topics,
 * extracts the device id segment and applies the configuration payload. For "/set"
 * topics, extracts the command segment and executes the corresponding action:
 * - "restart" triggers a system restart.
 * - "wifi-ssid" and "wifi-password" store the provided credential.
 * - "name" updates the room/name value (uses device MAC if payload is empty).
 * - Other commands are dispatched to registered subsystems; if a dispatched command
 *   reports a configuration change, the node is marked offline to force re-sync.
 *
 * If the topic does not match the expected patterns, the message is logged as unknown.
 *
 * @param topic MQTT topic string (null-terminated) expected to end with "/config" or "/set".
 * @param payload MQTT message payload string (null-terminated).
 */
void onMqttMessage(const char *topic, const char *payload) {
    String const top = String(topic);
    String pay = String(payload);

    auto setPos = top.lastIndexOf("/set");
    auto configPos = top.lastIndexOf("/config");
    if (configPos > 1) {
        auto idPos = top.lastIndexOf("/", configPos - 1);
        if (idPos < 0) goto skip;
        auto id = top.substring(idPos + 1, configPos);
        Log.printf("%d Config | %s to %s\r\n", xPortGetCoreID(), id.c_str(), pay.c_str());
        BleFingerprintCollection::Config(id, pay);
    } else if (setPos > 1) {
        auto commandPos = top.lastIndexOf("/", setPos - 1);
        if (commandPos < 0) goto skip;
        auto command = top.substring(commandPos + 1, setPos);
        Log.printf("%d Set    | %s to %s\r\n", xPortGetCoreID(), command.c_str(), pay.c_str());

        bool changed = false;
        if (command == "restart")
            ESP.restart();
        else if (command == "wifi-ssid" || command == "wifi-password")
            spurt("/" + command, pay);
        else if (command == "name")
            spurt("/room", pay.isEmpty() ? ESPMAC : pay);
        else if (GUI::Command(command, pay))
            ;
        else if (Motion::Command(command, pay))
            ;
        else if (BleFingerprintCollection::Command(command, pay))
            changed = true;
        else if (Enrollment::Command(command, pay))
            changed = true;
        else if (Updater::Command(command, pay))
            changed = true;
        else if (Motion::Command(command, pay))
            changed = true;
        else if (Switch::Command(command, pay))
            changed = true;
        else if (Button::Command(command, pay))
            changed = true;
        if (changed) online = false;
    } else {
    skip:
        Log.printf("%d Unknown| %s to %s\r\n", xPortGetCoreID(), topic, payload);
    }
}

std::string payload_buffer_;
/**
 * @brief Reassembles chunked MQTT message payloads and dispatches the complete message.
 *
 * Buffers incoming payload fragments until the full MQTT message has been received; when the final fragment arrives, calls onMqttMessage with the original topic and the reconstructed payload, then clears the buffer.
 *
 * @param topic Topic string associated with the incoming MQTT message.
 * @param payload Pointer to the current fragment's data.
 * @param properties MQTT message properties for the current fragment.
 * @param len Length in bytes of the current fragment.
 * @param index Byte offset of the current fragment within the full message.
 * @param total Total size in bytes of the full MQTT message.
 */
void onMqttMessageRaw(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
    if (index == 0)
        payload_buffer_.reserve(total);

    // append new payload, may contain incomplete MQTT message
    payload_buffer_.append(payload, len);

    // MQTT fully received
    if (len + index == total) {
        onMqttMessage(topic, payload_buffer_.data());
        payload_buffer_.clear();
    }
}

/**
 * @brief Attempts to re-establish network and MQTT connectivity, restarting the device on repeated failure.
 *
 * If both network and MQTT are already connected the function returns immediately.
 * The function increments an internal reconnect-attempt counter and restarts the device after more than 50 attempts.
 * When the network is disconnected it tries to restore connectivity using configured Ethernet or Wi‑Fi settings; failing to restore the network triggers a device restart.
 * After ensuring network connectivity it initiates an MQTT connection attempt.
 *
 * @param xTimer FreeRTOS timer handle associated with the reconnect timer (unused).
 */
void reconnect(TimerHandle_t xTimer) {
    Log.printf("%u Reconnect timer\r\n", xPortGetCoreID());
    if (Network.isConnected() && mqttClient.connected()) return;

    if (reconnectTries++ > 50) {
        Log.println("Too many reconnect attempts; Restarting");
        ESP.restart();
    }

    if (!Network.isConnected()) {
        Log.printf("%u Reconnecting to Network...\r\n", xPortGetCoreID());

        bool success = false;
        if (ethernetType > 0) success = Network.connect(ethernetType, 2, HeadlessWiFiSettings.hostname.c_str());
        if (!success && !HeadlessWiFiSettings.connect(true, 40))
            ESP.restart();
    }

    Log.printf("%u Reconnecting to MQTT...\r\n", xPortGetCoreID());
    mqttClient.connect();
}

void connectToMqtt() {
    reconnectTimer = xTimerCreate("reconnectionTimer", pdMS_TO_TICKS(3000), pdTRUE, (void *)nullptr, reconnect);
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onMessage(onMqttMessageRaw);
    mqttClient.setClientId(HeadlessWiFiSettings.hostname.c_str());
    mqttClient.setServer(mqttHost.c_str(), mqttPort);
    mqttClient.setWill(statusTopic.c_str(), 0, true, "offline");
    mqttClient.setCredentials(mqttUser.c_str(), mqttPass.c_str());
    mqttClient.connect();
}

bool reportBuffer(BleFingerprint *f) {
    if (!mqttClient.connected()) return false;
    auto report = f->getReport();
    String const topic = Sprintf(CHANNEL "/devices/%s/%s/%s", f->getId().c_str(), id.c_str(), report.getId().c_str());
    return mqttClient.publish(topic.c_str(), 0, false, report.getPayload().c_str());
}

bool reportDevice(BleFingerprint *f) {
    doc.clear();
    JsonObject obj = doc.to<JsonObject>();
    if (!f->report(&obj))
        return false;

    String const devicesTopic = Sprintf(CHANNEL "/devices/%s/%s", f->getId().c_str(), id.c_str());
    if (pub(devicesTopic.c_str(), 0, false, doc))
        return true;

    reportFailed++;
    return false;
}

unsigned int totalSeen = 0;
unsigned int totalFpSeen = 0;
unsigned int totalFpQueried = 0;
unsigned int totalFpReported = 0;

void reportSetup() {
    connectToMqtt();
}

void reportLoop() {
    if (!mqttClient.connected()) {
        return;
    }

    yield();
    auto copy = BleFingerprintCollection::GetCopy();

    unsigned int count = 0;
    for (auto &i : copy)
        if (i->shouldCount())
            count++;

    GUI::Count(count);

    yield();
    sendTelemetry(totalSeen, totalFpSeen, totalFpQueried, totalFpReported, count);
    yield();

    auto reported = 0;
    for (auto &f : copy) {
        auto seen = f->getSeenCount();
        if (seen) {
            totalSeen += seen;
            totalFpSeen++;
        }

        if (f->hasReport()) {
            if (reportBuffer(f))
                f->clearReport();
        }
        if (reportDevice(f)) {
            totalFpReported++;
            reported++;
        }
        yield();
    }
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice *advertisedDevice) {
        bleStack = uxTaskGetStackHighWaterMark(nullptr);
        BleFingerprintCollection::Seen(advertisedDevice);
    }
};

void scanTask(void *parameter) {
    NimBLEDevice::init("ESPresense");
    Enrollment::Setup();
    NimBLEDevice::setMTU(23);

    auto pBLEScan = NimBLEDevice::getScan();
    pBLEScan->setInterval(BLE_SCAN_INTERVAL);
    pBLEScan->setWindow(BLE_SCAN_WINDOW);
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), true);
    pBLEScan->setActiveScan(false);
    pBLEScan->setDuplicateFilter(false);
    pBLEScan->setMaxResults(0);
    if (!pBLEScan->start(0, nullptr, false))
        log_e("Error starting continuous ble scan");

    while (true) {
        for (auto &f : BleFingerprintCollection::fingerprints)
            if (f->query())
                totalFpQueried++;

        Enrollment::Loop();

        if (!pBLEScan->isScanning()) {
            if (!pBLEScan->start(0, nullptr, true))
                log_e("Error re-starting continuous ble scan");
            delay(3000);  // If we stopped scanning, don't query for 3 seconds in order for us to catch any missed broadcasts
        } else {
            delay(100);
        }
    }
}

/**
 * @brief Initialize hardware, peripherals, services, and background tasks required by the program.
 *
 * Configures serial logging and ESP log levels, registers a heap allocation-failure callback, initializes
 * platform-specific power/hardware (when enabled), filesystem, network, OTA/updater, GUI and application
 * subsystems (motion, switches, buttons, battery, CAN, NTP, optional sensors), enables TCP remote logging,
 * starts the BLE scan task, and performs MQTT/reporting setup.
 */
void setup() {
#ifdef FAST_MONITOR
    Serial.begin(1500000);
#else
    Serial.begin(115200);
#endif
    Serial.setDebugOutput(true);
#ifdef VERBOSE
    esp_log_level_set("*", ESP_LOG_DEBUG);
#else
    esp_log_level_set("*", ESP_LOG_ERROR);
#endif
    Log.printf("Pre-Setup Free Mem: %d\r\n", ESP.getFreeHeap());
    heap_caps_register_failed_alloc_callback(heapCapsAllocFailedHook);

#if M5STICK
    AXP192::Setup();
#endif

    GUI::Setup(true);
    BleFingerprintCollection::Setup();
    SPIFFS.begin(true);
    setupNetwork();
    Log.enableTcp(6053);
    Updater::Setup();
    GUI::Setup(false);
    Motion::Setup();
    Switch::Setup();
    Button::Setup();
    Battery::Setup();
    CAN::Setup();
    NTP::Setup();
#ifdef SENSORS
    DHT::Setup();
    I2C::Setup();
    AHTX0::Setup();
    BH1750::Setup();
    BME280::Setup();
    BMP180::Setup();
    BMP280::Setup();
    SHT::Setup();
    TSL2561::Setup();
    SensirionSGP30::Setup();
    SensirionSCD4x::Setup();
    HX711::Setup();
    DS18B20::Setup();
#endif
    xTaskCreatePinnedToCore(scanTask, "scanTask", SCAN_TASK_STACK_SIZE, nullptr, 1, &scanTaskHandle, CONFIG_BT_NIMBLE_PINNED_TO_CORE);
    reportSetup();
    Log.printf("Post-Setup Free Mem: %d\r\n", ESP.getFreeHeap());
    Log.println();
}

/**
 * @brief Executes the main periodic processing and dispatches per-subsystem loop handlers.
 *
 * Runs the primary report processing and repeatedly invokes each subsystem's Loop method.
 * Every 5 seconds it performs a slow maintenance check that reads available heap memory,
 * logs a low-memory warning when free memory is less than 20,000 bytes, and runs the updater
 * loop when free memory exceeds 70,000 bytes.
 *
 * Subsystems invoked each iteration include GUI, Motion, Switch, Button, HTTP server,
 * SerialImprov, NTP, and (conditionally) AXP192 and various sensor modules.
 */
void loop() {
    reportLoop();
    static unsigned long lastSlowLoop = 0;
    if (millis() - lastSlowLoop > 5000) {
        lastSlowLoop = millis();
        auto freeHeap = ESP.getFreeHeap();
        if (freeHeap < 20000) Log.printf("Low memory: %u bytes free\r\n", freeHeap);
        if (freeHeap > 70000) Updater::Loop();
    }
    GUI::Loop();
    Motion::Loop();
    Switch::Loop();
    Button::Loop();
    HttpWebServer::Loop();
    SerialImprov::Loop(false);
    NTP::Loop();
#if M5STICK
    AXP192::Loop();
#endif
#ifdef SENSORS
    DHT::Loop();
    AHTX0::Loop();
    BH1750::Loop();
    BME280::Loop();
    BMP180::Loop();
    BMP280::Loop();
    SHT::Loop();
    TSL2561::Loop();
    SensirionSGP30::Loop();
    SensirionSCD4x::Loop();
    HX711::Loop();
    DS18B20::Loop();
#endif

}
