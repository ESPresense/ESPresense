#define VAR_DECLS
#include "main.h"

bool sendTelemetry(unsigned int totalSeen, unsigned int totalFpSeen, int unsigned totalFpQueried, int unsigned totalFpReported, unsigned int count) {
    if (!online) {
        if (
            pub(statusTopic.c_str(), 0, true, "online")
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
            && GUI::SendOnline()
        ) {
            online = true;
            reconnectTries = 0;
        } else {
            Serial.println("Error sending status=online");
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

            && sendDeleteDiscovery("switch", "Status LED")
            && sendDeleteDiscovery("switch", "Active Scan")

            && Updater::SendDiscovery()
            && GUI::SendDiscovery()
            && Motion::SendDiscovery()
            && Enrollment::SendDiscovery()
            && Battery::SendDiscovery()
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
            && HX711::SendDiscovery()
#endif
        ) {
            sentDiscovery = true;
        } else {
            Serial.println("Error sending discovery");
        }
    }

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
# else
    doc["ver"] = ESP.getSketchMD5();
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
    if (teleFails > 0)
        doc["teleFails"] = teleFails;
    if (reconnectTries > 0)
        doc["reconnectTries"] = reconnectTries;
    auto maxHeap = ESP.getMaxAllocHeap();
    auto freeHeap = ESP.getFreeHeap();
    doc["freeHeap"] = freeHeap;
    doc["maxAllocHeap"] = maxHeap;
    doc["memFrag"] = 100 - (maxHeap * 100.0 / freeHeap);
    doc["scanHighWater"] = uxTaskGetStackHighWaterMark(scanTaskHandle);
    doc["reportHighWater"] = uxTaskGetStackHighWaterMark(reportTaskHandle);

    serializeJson(doc, buffer);
    if (pub(teleTopic.c_str(), 0, false, buffer)) return true;

    teleFails++;
    log_e("Error after 10 tries sending telemetry (%d times since boot)", teleFails);
    return false;
}

void setupNetwork() {
    Serial.println("Setup network");
    WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);
    GUI::Connected(false, false);

#ifdef VERSION
    AsyncWiFiSettings.info("ESPresense Version: " + String(VERSION));
#endif
    room = AsyncWiFiSettings.string("room", ESPMAC, "Room");
    auto wifiTimeout = AsyncWiFiSettings.integer("wifi_timeout", DEFAULT_WIFI_TIMEOUT, "Seconds to wait for WiFi before captive portal (-1 = forever)");
    auto portalTimeout = 1000UL * AsyncWiFiSettings.integer("portal_timeout", DEFAULT_PORTAL_TIMEOUT, "Seconds to wait in captive portal before rebooting");
    std::vector<String> ethernetTypes = {"None", "WT32-ETH01", "ESP32-POE", "WESP32", "QuinLED-ESP32", "TwilightLord-ESP32", "ESP32Deux", "KIT-VE", "LilyGO-T-ETH-POE", "GL-inet GL-S10 v2.1 Ethernet"};
    ethernetType = AsyncWiFiSettings.dropdown("eth", ethernetTypes, 0, "Ethernet Type");

    AsyncWiFiSettings.heading("MQTT <a href='https://espresense.com/configuration/settings#mqtt' target='_blank'>ℹ️</a>", false);
    mqttHost = AsyncWiFiSettings.string("mqtt_host", DEFAULT_MQTT_HOST, "Server");
    mqttPort = AsyncWiFiSettings.integer("mqtt_port", DEFAULT_MQTT_PORT, "Port");
    mqttUser = AsyncWiFiSettings.pstring("mqtt_user", DEFAULT_MQTT_USER, "Username");
    mqttPass = AsyncWiFiSettings.pstring("mqtt_pass", DEFAULT_MQTT_PASSWORD, "Password");
    discovery = AsyncWiFiSettings.checkbox("discovery", true, "Send to discovery topic");
    homeAssistantDiscoveryPrefix = AsyncWiFiSettings.string("discovery_prefix", DEFAULT_HA_DISCOVERY_PREFIX, "Home Assistant discovery topic prefix");
    publishTele = AsyncWiFiSettings.checkbox("pub_tele", true, "Send to telemetry topic");
    publishRooms = AsyncWiFiSettings.checkbox("pub_rooms", true, "Send to rooms topic");
    publishDevices = AsyncWiFiSettings.checkbox("pub_devices", true, "Send to devices topic");

    AsyncWiFiSettings.heading("Updating <a href='https://espresense.com/configuration/settings#updating' target='_blank'>ℹ️</a>", false);
    Updater::ConnectToWifi();

    AsyncWiFiSettings.heading("Scanning <a href='https://espresense.com/configuration/settings#scanning' target='_blank'>ℹ️</a>", false);
    BleFingerprintCollection::knownMacs = AsyncWiFiSettings.string("known_macs", "", "Known BLE mac addresses (no colons, space seperated)");
    BleFingerprintCollection::knownIrks = AsyncWiFiSettings.string("known_irks", "", "Known BLE identity resolving keys, should be 32 hex chars space seperated");
    BleFingerprintCollection::query = AsyncWiFiSettings.string("query", DEFAULT_QUERY, "Query device ids for characteristics (eg. apple:1005:9-26)");

    AsyncWiFiSettings.heading("Counting <a href='https://espresense.com/configuration/settings#counting' target='_blank'>ℹ️</a>", false);
    BleFingerprintCollection::countIds = AsyncWiFiSettings.string("count_ids", "", "Include id prefixes (space seperated)");
    BleFingerprintCollection::countEnter = AsyncWiFiSettings.floating("count_enter", 0, 100, 2, "Start counting devices less than distance (in meters)");
    BleFingerprintCollection::countExit = AsyncWiFiSettings.floating("count_exit", 0, 100, 4, "Stop counting devices greater than distance (in meters)");
    BleFingerprintCollection::countMs = AsyncWiFiSettings.integer("count_ms", 0, 3000000, 30000, "Include devices with age less than (in ms)");

    AsyncWiFiSettings.heading("Filtering <a href='https://espresense.com/configuration/settings#filtering' target='_blank'>ℹ️</a>", false);
    BleFingerprintCollection::include = AsyncWiFiSettings.string("include", DEFAULT_INCLUDE, "Include only sending these ids to mqtt (eg. apple:iphone10-6 apple:iphone13-2)");
    BleFingerprintCollection::exclude = AsyncWiFiSettings.string("exclude", DEFAULT_EXCLUDE, "Exclude sending these ids to mqtt (eg. exp:20 apple:iphone10-6)");
    BleFingerprintCollection::maxDistance = AsyncWiFiSettings.floating("max_dist", 0, 100, DEFAULT_MAX_DISTANCE, "Maximum distance to report (in meters)");
    BleFingerprintCollection::skipDistance = AsyncWiFiSettings.floating("skip_dist", 0, 10, DEFAULT_SKIP_DISTANCE, "Report early if beacon has moved more than this distance (in meters)");
    BleFingerprintCollection::skipMs = AsyncWiFiSettings.integer("skip_ms", 0, 3000000, DEFAULT_SKIP_MS, "Skip reporting if message age is less that this (in milliseconds)");

    AsyncWiFiSettings.heading("Calibration <a href='https://espresense.com/configuration/settings#calibration' target='_blank'>ℹ️</a>", false);
    BleFingerprintCollection::rxRefRssi = AsyncWiFiSettings.integer("ref_rssi", -100, 100, DEFAULT_RX_REF_RSSI, "Rssi expected from a 0dBm transmitter at 1 meter (NOT used for iBeacons or Eddystone)");
    BleFingerprintCollection::rxAdjRssi = AsyncWiFiSettings.integer("rx_adj_rssi", -100, 100, 0, "Rssi adjustment for receiver (use only if you know this device has a weak antenna)");
    BleFingerprintCollection::absorption = AsyncWiFiSettings.floating("absorption", -100, 100, DEFAULT_ABSORPTION, "Factor used to account for absorption, reflection, or diffraction");
    BleFingerprintCollection::forgetMs = AsyncWiFiSettings.integer("forget_ms", 0, 3000000, DEFAULT_FORGET_MS, "Forget beacon if not seen for (in milliseconds)");
    BleFingerprintCollection::txRefRssi = AsyncWiFiSettings.integer("tx_ref_rssi", -100, 100, DEFAULT_TX_REF_RSSI, "Rssi expected from this tx power at 1m (used for node iBeacon)");

    GUI::ConnectToWifi();

    AsyncWiFiSettings.heading("GPIO Sensors <a href='https://espresense.com/configuration/settings#gpio-sensors' target='_blank'>ℹ️</a>", false);

    BleFingerprintCollection::ConnectToWifi();
    Motion::ConnectToWifi();

#ifdef SENSORS
    DHT::ConnectToWifi();
    I2C::ConnectToWifi();

    AsyncWiFiSettings.heading("I2C Sensors <a href='https://espresense.com/configuration/settings#i2c-sensors' target='_blank'>ℹ️</a>", false);

    AHTX0::ConnectToWifi();
    BH1750::ConnectToWifi();
    BME280::ConnectToWifi();
    BMP180::ConnectToWifi();
    BMP280::ConnectToWifi();
    SHT::ConnectToWifi();
    TSL2561::ConnectToWifi();
    SensirionSGP30::ConnectToWifi();
    HX711::ConnectToWifi();

#endif

    unsigned int connectProgress = 0;
    AsyncWiFiSettings.onWaitLoop = [&connectProgress]() {
        GUI::Wifi(connectProgress++);
        SerialImprov::Loop(true);
        return 50;
    };
    unsigned int portalProgress = 0;
    AsyncWiFiSettings.onPortalWaitLoop = [&portalProgress, portalTimeout]() {
        GUI::Portal(portalProgress++);
        SerialImprov::Loop(false);

        if (millis() > portalTimeout)
            ESP.restart();

        return 50;
    };
    AsyncWiFiSettings.onHttpSetup = HttpWebServer::Init;
    AsyncWiFiSettings.hostname = "espresense-" + kebabify(room);

    bool success = false;
    if (ethernetType > 0) success = Network.connect(ethernetType, 20, AsyncWiFiSettings.hostname.c_str());
    if (!success && !AsyncWiFiSettings.connect(true, wifiTimeout))
        ESP.restart();

    GUI::Connected(true, false);

#ifdef FIRMWARE
    Serial.println("Firmware:     " + String(FIRMWARE));
#endif
#ifdef VERSION
    Serial.println("Version:      " + String(VERSION));
#endif
    Serial.printf("WiFi BSSID:   %s (channel=%d rssi=%d)\n", WiFi.BSSIDstr().c_str(), WiFi.channel(), WiFi.RSSI());
    Serial.print("IP address:   ");
    Serial.println(Network.localIP());
    Serial.print("DNS address:  ");
    Serial.println(Network.dnsIP());
    Serial.print("Hostname:     ");
    Serial.println(Network.getHostname());
    Serial.print("Room:         ");
    Serial.println(room);
    Serial.printf("Mqtt server:  %s:%d\n", mqttHost.c_str(), mqttPort);
    Serial.printf("Max Distance: %.2f\n", BleFingerprintCollection::maxDistance);
    Serial.printf("Init Free Mem:%d\n", ESP.getFreeHeap());
    GUI::SerialReport();
    Motion::SerialReport();
    I2C::SerialReport();
#ifdef SENSORS
    DHT::SerialReport();
    AHTX0::SerialReport();
    BH1750::SerialReport();
    BME280::SerialReport();
    BMP180::SerialReport();
    BMP280::SerialReport();
    SHT::SerialReport();
    TSL2561::SerialReport();
    SensirionSGP30::SerialReport();
    HX711::SerialReport();

#endif
    Serial.print("Query:        ");
    Serial.println(BleFingerprintCollection::query);
    Serial.print("Include:      ");
    Serial.println(BleFingerprintCollection::include);
    Serial.print("Exclude:      ");
    Serial.println(BleFingerprintCollection::exclude);
    Serial.print("Known Macs:   ");
    Serial.println(BleFingerprintCollection::knownMacs);
    Serial.print("Count Ids:    ");
    Serial.println(BleFingerprintCollection::countIds);
    Serial.println();

    localIp = Network.localIP().toString();
    id = slugify(room);
    roomsTopic = CHANNEL + String("/rooms/") + id;
    statusTopic = roomsTopic + "/status";
    teleTopic = roomsTopic + "/telemetry";
    setTopic = roomsTopic + "/+/set";
    configTopic = CHANNEL + String("/settings/+/config");
    AsyncWiFiSettings.httpSetup();
}

void onMqttConnect(bool sessionPresent) {
    xTimerStop(reconnectTimer, 0);
    mqttClient.subscribe("espresense/rooms/*/+/set", 1);
    mqttClient.subscribe(setTopic.c_str(), 1);
    mqttClient.subscribe(configTopic.c_str(), 1);
    GUI::Connected(true, true);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    GUI::Connected(true, false);
    Serial.printf("Disconnected from MQTT; reason %d\n", reason);
    xTimerStart(reconnectTimer, 0);
    online = false;
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
    char new_payload[len + 1];
    new_payload[len] = '\0';
    strncpy(new_payload, payload, len);

    String top = String(topic);
    String pay = String(new_payload);

    auto setPos = top.lastIndexOf("/set");
    auto configPos = top.lastIndexOf("/config");
    if (configPos > 1) {
        auto idPos = top.lastIndexOf("/", configPos - 1);
        if (idPos < 0) goto skip;
        auto id = top.substring(idPos + 1, configPos);
        Serial.printf("%d MQTT  | Config %s: %s\n", xPortGetCoreID(), id.c_str(), pay.c_str());
        BleFingerprintCollection::Config(id, pay);
    } else if (setPos > 1) {
        auto commandPos = top.lastIndexOf("/", setPos - 1);
        if (commandPos < 0) goto skip;
        auto command = top.substring(commandPos + 1, setPos);
        Serial.printf("%d MQTT  | Set %s: %s\n", xPortGetCoreID(), command.c_str(), pay.c_str());

        bool changed = false;
        if (command == "restart")
            ESP.restart();
        else if (command == "wifi-ssid" || command == "wifi-password")
            spurt("/" + command, pay);
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
        if (changed) online = false;
    } else {
    skip:
        Serial.printf("%d MQTT  | Unknown: %s: %s\n", xPortGetCoreID(), topic, new_payload);
    }
}

void reconnect(TimerHandle_t xTimer) {
    Serial.printf("%u Reconnect timer\n", xPortGetCoreID());
    if (Network.isConnected() && mqttClient.connected()) return;

    if (reconnectTries++ > 50) {
        Serial.println("Too many reconnect attempts; Restarting");
        ESP.restart();
    }

    if (!Network.isConnected()) {
        Serial.printf("%u Reconnecting to Network...\n", xPortGetCoreID());

        bool success = false;
        if (ethernetType > 0) success = Network.connect(ethernetType, 2, AsyncWiFiSettings.hostname.c_str());
        if (!success && !AsyncWiFiSettings.connect(true, 40))
            ESP.restart();
    }

    Serial.printf("%u Reconnecting to MQTT...\n", xPortGetCoreID());
    mqttClient.connect();
}

void connectToMqtt() {
    reconnectTimer = xTimerCreate("reconnectionTimer", pdMS_TO_TICKS(3000), pdTRUE, (void *)nullptr, reconnect);
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.setClientId(AsyncWiFiSettings.hostname.c_str());
    mqttClient.setServer(mqttHost.c_str(), mqttPort);
    mqttClient.setWill(statusTopic.c_str(), 0, true, "offline");
    mqttClient.setCredentials(mqttUser.c_str(), mqttPass.c_str());
    mqttClient.connect();
}

bool reportDevice(BleFingerprint *f) {
    doc.clear();
    JsonObject obj = doc.to<JsonObject>();
    if (!f->report(&obj))
        return false;

    serializeJson(doc, buffer);
    String devicesTopic = Sprintf(CHANNEL "/devices/%s/%s", f->getId().c_str(), id.c_str());

    bool p1 = false, p2 = false;
    for (int i = 0; i < 10; i++) {
        if (!mqttClient.connected())
            return false;

        if (!p1 && (!publishRooms || mqttClient.publish(roomsTopic.c_str(), 0, false, buffer)))
            p1 = true;

        if (!p2 && (!publishDevices || mqttClient.publish(devicesTopic.c_str(), 0, false, buffer)))
            p2 = true;

        if (p1 && p2)
            return true;
        delay(20);
    }
    teleFails++;
    return false;
}

unsigned int totalSeen = 0;
unsigned int totalFpSeen = 0;
unsigned int totalFpQueried = 0;
unsigned int totalFpReported = 0;

void reportTask(void *parameter) {
    connectToMqtt();

    while (true) {
        while (!mqttClient.connected())
            delay(1000);

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
            if (reportDevice(f)) {
                totalFpReported++;
                reported++;
            }
            yield();
        }
    }
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice *advertisedDevice) {
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

void setup() {
#ifdef FAST_MONITOR
    Serial.begin(1500000);
#else
    Serial.begin(115200);
#endif
    Serial.setDebugOutput(true);

#if M5STICK
    AXP192::Setup();
#endif

    GUI::Setup(true);
    BleFingerprintCollection::Setup();

#ifdef VERBOSE
    esp_log_level_set("*", ESP_LOG_DEBUG);
#else
    esp_log_level_set("*", ESP_LOG_ERROR);
#endif
    SPIFFS.begin(true);
    setupNetwork();
    Updater::Setup();
#if NTP
    setClock();
#endif
    GUI::Setup(false);
    Motion::Setup();
    Battery::Setup();
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
    HX711::Setup();
#endif
    xTaskCreatePinnedToCore(scanTask, "scanTask", SCAN_TASK_STACK_SIZE, nullptr, 1, &scanTaskHandle, CONFIG_BT_NIMBLE_PINNED_TO_CORE);
    xTaskCreatePinnedToCore(reportTask, "reportTask", REPORT_TASK_STACK_SIZE, nullptr, 1, &reportTaskHandle, REPORT_PINNED_TO_CORE);
}

void loop() {
    static unsigned long lastSlowLoop = 0;
    if (millis() - lastSlowLoop > 5000) {
        lastSlowLoop = millis();
        auto freeHeap = ESP.getFreeHeap();
        if (freeHeap < 20000) Serial.printf("Low memory: %u bytes free\n", freeHeap);
        if (freeHeap > 70000) Updater::Loop();
    }
    GUI::Loop();
    Motion::Loop();
    HttpWebServer::Loop();
    SerialImprov::Loop(false);
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
    HX711::Loop();
#endif
}
