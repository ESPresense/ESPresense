#define VAR_DECLS
#include "main.h"

bool sendTelemetry(int totalSeen, int totalFpSeen, int totalFpQueried, int totalFpReported, int count)
{
    if (!online)
    {
        if (pub(statusTopic.c_str(), 0, true, "online")
        && pub((roomsTopic + "/max_distance").c_str(), 0, true, String(BleFingerprintCollection::maxDistance).c_str())
        && pub((roomsTopic + "/absorption").c_str(), 0, true, String(BleFingerprintCollection::absorption).c_str())
        && pub((roomsTopic + "/query").c_str(), 0, true, BleFingerprintCollection::query.c_str())
        && pub((roomsTopic + "/include").c_str(), 0, true, BleFingerprintCollection::include.c_str())
        && pub((roomsTopic + "/exclude").c_str(), 0, true, BleFingerprintCollection::exclude.c_str())
        && pub((roomsTopic + "/known_macs").c_str(), 0, true, BleFingerprintCollection::knownMacs.c_str())
        && pub((roomsTopic + "/known_irks").c_str(), 0, true, BleFingerprintCollection::knownIrks.c_str())
        && pub((roomsTopic + "/count_ids").c_str(), 0, true, BleFingerprintCollection::countIds.c_str())
        && pub((roomsTopic + "/status_led").c_str(), 0, true, String(GUI::statusLed ? "ON" : "OFF").c_str())
        && pub((roomsTopic + "/arduino_ota").c_str(), 0, true, String(arduinoOta ? "ON" : "OFF").c_str())
        && pub((roomsTopic + "/auto_update").c_str(), 0, true, String(autoUpdate ? "ON" : "OFF").c_str())
        && pub((roomsTopic + "/prerelease").c_str(), 0, true, String(prerelease ? "ON" : "OFF").c_str())
        && pub((roomsTopic + "/active_scan").c_str(), 0, true, String(activeScan ? "ON" : "OFF").c_str())
        && Motion::SendOnline())
        {
            online = true;
            reconnectTries = 0;
        }
        else
        {
            log_e("Error sending status=online");
        }
    }

    if (discovery && !sentDiscovery)
    {
        if (sendConnectivityDiscovery()
            && sendTeleSensorDiscovery("Uptime", EC_DIAGNOSTIC, "{{ value_json.uptime }}", DEVICE_CLASS_NONE, "s")
            && sendTeleSensorDiscovery("Free Mem", EC_DIAGNOSTIC, "{{ value_json.freeHeap }}", DEVICE_CLASS_NONE, "bytes")
            && (BleFingerprintCollection::countIds.isEmpty() ? sendDeleteDiscovery("sensor", "Count") : sendTeleSensorDiscovery("Count", EC_NONE, "{{ value_json.count }}"))
            && sendButtonDiscovery("Restart", EC_DIAGNOSTIC)
            && sendSwitchDiscovery("Status LED", EC_CONFIG)
            && sendNumberDiscovery("Max Distance", EC_CONFIG)
            && sendNumberDiscovery("Absorption", EC_CONFIG)
            && sendSwitchDiscovery("Active Scan", EC_CONFIG)
            && sendSwitchDiscovery("Auto Update", EC_CONFIG)
            && sendSwitchDiscovery("Arduino OTA", EC_CONFIG)
            && sendSwitchDiscovery("Prerelease", EC_CONFIG)
            && Motion::SendDiscovery()
            && Enrollment::SendDiscovery()
#ifdef MACCHINA_A0
            && sendTeleSensorDiscovery("Battery", EC_NONE, "{{ value_json.batt }}", "battery", "%")
            && sendTeleBinarySensorDiscovery("Charging", EC_NONE, "{{ value_json.charging }}", "battery_charging")
#endif
#ifdef SENSORS
            && DHT::SendDiscovery()
            && BH1750::SendDiscovery()
            && BME280::SendDiscovery()
            && TSL2561::SendDiscovery()
            && HX711::SendDiscovery()
#endif
        )
        {
            sentDiscovery = true;
        }
        else
        {
            log_e("Error sending discovery");
        }
    }

    auto now = millis();

    if (now - lastTeleMillis < 15000)
        return false;

    lastTeleMillis = now;

    doc.clear();
    doc["ip"] = localIp;
    doc["uptime"] = getUptimeSeconds();
#ifdef FIRMWARE
    doc["firm"] = String(FIRMWARE);
#endif
    doc["rssi"] = WiFi.RSSI();
#ifdef MACCHINA_A0
    auto mv = a0_read_batt_mv();
    doc["mV"] = mv;
    bool charging = (mv > 13200);
    unsigned int soc = round(-13275.04 + 2.049731 * mv - (0.00007847975 * mv) * mv);
    doc["batt"] = charging ? (unsigned int)100 : max((unsigned int)0, min((unsigned int)100, soc));
    doc["charging"] = charging ? "ON" : "OFF";
#endif
#ifdef VERSION
    doc["ver"] = String(VERSION);
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

    doc["freeHeap"] = ESP.getFreeHeap();
    doc["maxAllocHeap"] = ESP.getMaxAllocHeap();
    doc["memFrag"] = 100 - (ESP.getMaxAllocHeap() * 100.0 / ESP.getFreeHeap());
    doc["resetReason"] = resetReason(rtc_get_reset_reason(0));
    doc["scanHighWater"] = uxTaskGetStackHighWaterMark(scanTaskHandle);
    doc["reportHighWater"] = uxTaskGetStackHighWaterMark(reportTaskHandle);

    char teleMessageBuffer[512];
    serializeJson(doc, teleMessageBuffer);

    for (int i = 0; i < 10; i++)
    {
        if (!publishTele || mqttClient.publish(teleTopic.c_str(), 0, false, teleMessageBuffer))
            return true;
        delay(50);
    }

    teleFails++;
    log_e("Error after 10 tries sending telemetry (%d times since boot)", teleFails);
    return false;
}

void setupNetwork()
{
    Serial.println("Setup network");
    WiFi.persistent(false);
    GUI::connected(false, false);

    AsyncWiFiSettings.onFailure = []()
    {
        GUI::status("WiFi Failed");
    };
    AsyncWiFiSettings.onWaitLoop = []()
    {
        GUI::connecting();
        return 150;
    };
    static bool inPortal = false;
    AsyncWiFiSettings.onPortalWaitLoop = []()
    {
        if (!inPortal)
        {
            inPortal = true;
            GUI::status("WiFi Portal...");
        }

        if (getUptimeSeconds() > CAPTIVE_PORTAL_TIMEOUT)
            ESP.restart();
    };
    AsyncWiFiSettings.onHttpSetup = HttpServer::Init;

#ifdef VERSION
    AsyncWiFiSettings.info("ESPresense Version: " + String(VERSION));
#endif
    room = AsyncWiFiSettings.string("room", ESPMAC, "Room");
    std::vector<String> ethernetTypes = {"None", "WT32-ETH01", "ESP32-POE", "WESP32", "QuinLED-ESP32", "TwilightLord-ESP32", "ESP32Deux", "KIT-VE", "LilyGO-T-ETH-POE"};
    auto allChannelScan = AsyncWiFiSettings.checkbox("all_channel_scan", false, "WiFi: Scan for all APs on all channels");
    if (allChannelScan) WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);
    else WiFi.setScanMethod(WIFI_FAST_SCAN);
    ethernetType = AsyncWiFiSettings.dropdown("eth", ethernetTypes, 0, "Ethernet Type");

    AsyncWiFiSettings.heading("MQTT <a href='https://espresense.com/configuration/settings#mqtt' target='_blank'>ℹ️</a>", false);
    mqttHost = AsyncWiFiSettings.string("mqtt_host", DEFAULT_MQTT_HOST, "Server");
    mqttPort = AsyncWiFiSettings.integer("mqtt_port", DEFAULT_MQTT_PORT, "Port");
    mqttUser = AsyncWiFiSettings.pstring("mqtt_user", DEFAULT_MQTT_USER, "Username");
    mqttPass = AsyncWiFiSettings.pstring("mqtt_pass", DEFAULT_MQTT_PASSWORD, "Password");
    discovery = AsyncWiFiSettings.checkbox("discovery", true, "Send to discovery topic");
    publishTele = AsyncWiFiSettings.checkbox("pub_tele", true, "Send to telemetry topic");
    publishRooms = AsyncWiFiSettings.checkbox("pub_rooms", true, "Send to rooms topic");
    publishDevices = AsyncWiFiSettings.checkbox("pub_devices", true, "Send to devices topic");

    AsyncWiFiSettings.heading("Updating <a href='https://espresense.com/configuration/settings#updating' target='_blank'>ℹ️</a>", false);
    autoUpdate = AsyncWiFiSettings.checkbox("auto_update", DEFAULT_AUTO_UPDATE, "Automatically update");
    prerelease = AsyncWiFiSettings.checkbox("prerelease", false, "Include pre-released versions in auto-update");
    arduinoOta = AsyncWiFiSettings.checkbox("arduino_ota", DEFAULT_ARDUINO_OTA, "Arduino OTA Update");

    AsyncWiFiSettings.heading("Scanning <a href='https://espresense.com/configuration/settings#scanning' target='_blank'>ℹ️</a>", false);
    activeScan = AsyncWiFiSettings.checkbox("active_scan", false, "Request scan results (usually not needed)");
    BleFingerprintCollection::knownMacs = AsyncWiFiSettings.string("known_macs", "", "Known BLE mac addresses (no colons, space seperated)");
    BleFingerprintCollection::knownIrks = AsyncWiFiSettings.string("known_irks", "", "Known BLE identity resolving keys, should be 32 hex chars space seperated");
    BleFingerprintCollection::query = AsyncWiFiSettings.string("query", DEFAULT_QUERY, "Query device ids for characteristics (eg. apple:1005:9-26)");

    AsyncWiFiSettings.heading("Counting <a href='https://espresense.com/configuration/settings#counting' target='_blank'>ℹ️</a>", false);
    BleFingerprintCollection::countIds = AsyncWiFiSettings.string("count_ids", "", "Include device ids (space seperated ids)");
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
    BleFingerprintCollection::refRssi = AsyncWiFiSettings.integer("ref_rssi", -100, 100, DEFAULT_REF_RSSI, "Rssi expected from a 0dBm transmitter at 1 meter (NOT used for iBecons or Eddystone)");
    BleFingerprintCollection::absorption = AsyncWiFiSettings.floating("absorption", -100, 100, DEFAULT_ABSORPTION, "Factor used to account for absorption, reflection, or diffraction");
    BleFingerprintCollection::forgetMs = AsyncWiFiSettings.integer("forget_ms", 0, 3000000, DEFAULT_FORGET_MS, "Forget beacon if not seen for (in milliseconds)");

    AsyncWiFiSettings.heading("Misc <a href='https://espresense.com/configuration/settings#misc' target='_blank'>ℹ️</a>", false);
    GUI::statusLed = AsyncWiFiSettings.checkbox("status_led", true, "Status LED");

    fingerprints.connectToWifi();
    Motion::ConnectToWifi();

#ifdef SENSORS
    DHT::ConnectToWifi();
    I2C::ConnectToWifi();

    AsyncWiFiSettings.heading("I2C Sensors <a href='https://espresense.com/configuration/settings#i2c-sensors' target='_blank'>ℹ️</a>", false);

    BH1750::ConnectToWifi();
    BME280::ConnectToWifi();
    TSL2561::ConnectToWifi();
    HX711::ConnectToWifi();
#endif

    AsyncWiFiSettings.hostname = "espresense-" + kebabify(room);

    bool success = false;
    if (ethernetType > 0) success = Network.connect(ethernetType, 20, AsyncWiFiSettings.hostname.c_str());
    if (!success && !AsyncWiFiSettings.connect(true, 40))
        ESP.restart();

    GUI::connected(true, false);

#ifdef FIRMWARE
    Serial.println("Firmware:     " + String(FIRMWARE));
#endif
#ifdef VERSION
    Serial.println("Version:      " + String(VERSION));
#endif
    Serial.printf("WiFi Scan:    %s (picked channel=%d bss=%s rssi=%d)\n", (allChannelScan ? "All" : "Fast"), WiFi.channel(), WiFi.BSSIDstr().c_str(), WiFi.RSSI());
    Serial.print("IP address:   ");
    Serial.println(Network.localIP());
    Serial.print("DNS address:  ");
    Serial.println(Network.dnsIP());
    Serial.print("Hostname:     ");
    Serial.println(Network.getHostname());
    Serial.print("Room:         ");
    Serial.println(room);
    Serial.printf("MQTT server:  %s:%d\n", mqttHost.c_str(), mqttPort);
    Serial.printf("Max Distance: %.2f\n", BleFingerprintCollection::maxDistance);
    Motion::SerialReport();
    I2C::SerialReport();
#ifdef SENSORS
    DHT::SerialReport();
    BH1750::SerialReport();
    BME280::SerialReport();
    TSL2561::SerialReport();
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
    roomsTopic = CHANNEL + "/rooms/" + id;
    statusTopic = roomsTopic + "/status";
    teleTopic = roomsTopic + "/telemetry";
    setTopic = roomsTopic + "/+/set";
    configTopic = CHANNEL + "/settings/+/config";
    AsyncWiFiSettings.httpSetup();
}

void onMqttConnect(bool sessionPresent)
{
    xTimerStop(reconnectTimer, 0);
    mqttClient.subscribe("espresense/rooms/*/+/set", 1);
    mqttClient.subscribe(setTopic.c_str(), 1);
    mqttClient.subscribe(configTopic.c_str(), 1);
    GUI::connected(true, true);
    Serial.println("Connected to MQTT");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    GUI::connected(true, false);
    Serial.printf("Disconnected from MQTT; reason %d\n", reason);
    xTimerStart(reconnectTimer, 0);
    online = false;
}

bool Command(String& command, String& pay) {

    if (command == "active_scan")
    {
        activeScan = pay == "ON";
        spurt("/active_scan", String(activeScan));
    }
    else if (command == "status_led")
    {
        GUI::statusLed = pay == "ON";
        spurt("/status_led", String(GUI::statusLed));
    }
    else if (command == "arduino_ota")
    {
        arduinoOta = pay == "ON";
        spurt("/arduino_ota", String(arduinoOta));
    }
    else if (command == "auto_update")
    {
        autoUpdate = pay == "ON";
        spurt("/auto_update", String(autoUpdate));
    }
    else if (command == "prerelease")
    {
        prerelease = pay == "ON";
        spurt("/prerelease", String(prerelease));
    }
    else if (command == "restart")
    {
        ESP.restart();
    }
    else
        return false;
    return true;
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
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
        fingerprints.config(id, pay);
    } else if (setPos > 1) {

        auto commandPos = top.lastIndexOf("/", setPos - 1);
        if (commandPos < 0) goto skip;
        auto command = top.substring(commandPos + 1, setPos);
        Serial.printf("%d MQTT  | Set %s: %s\n", xPortGetCoreID(), command.c_str(), pay.c_str());

        bool changed = false;
        if (Command(command, pay))
            changed = true;
        else if (fingerprints.command(command, pay))
            changed = true;
        else if (Enrollment::Command(command, pay))
            changed = true;
        else if (Motion::Command(command, pay))
            changed = true;
        if (changed) online = false;
    } else {
skip:
        Serial.printf("%d MQTT  | Unknown: %s: %s\n", xPortGetCoreID(), topic, new_payload);
    }
}

void reconnect(TimerHandle_t xTimer)
{
    Serial.printf("%u Reconnect timer\n", xPortGetCoreID());
    if (updateInProgress()) return;
    if (Network.isConnected() && mqttClient.connected()) return;

    if (reconnectTries++ > 50)
    {
        Serial.println("Too many reconnect attempts; Restarting");
        ESP.restart();
    }

    if (!Network.isConnected())
    {
        Serial.printf("%u Reconnecting to Network...\n", xPortGetCoreID());

        bool success = false;
        if (ethernetType > 0) success = Network.connect(ethernetType, 2, AsyncWiFiSettings.hostname.c_str());
        if (!success && !AsyncWiFiSettings.connect(true, 40))
            ESP.restart();
    }

    Serial.printf("%u Reconnecting to MQTT...\n", xPortGetCoreID());
    mqttClient.connect();
}

void connectToMqtt()
{
    reconnectTimer = xTimerCreate("reconnectionTimer", pdMS_TO_TICKS(3000), pdTRUE, (void *)nullptr, reconnect);
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.setClientId(AsyncWiFiSettings.hostname.c_str());
    mqttClient.setServer(mqttHost.c_str(), mqttPort);
    mqttClient.setWill(statusTopic.c_str(), 0, true, offline.c_str());
    mqttClient.setCredentials(mqttUser.c_str(), mqttPass.c_str());
    mqttClient.connect();
}

bool reportDevice(BleFingerprint *f)
{
    doc.clear();
    JsonObject obj = doc.to<JsonObject>();
    if (!f->report(&obj))
        return false;

    serializeJson(doc, buffer);
    String devicesTopic = CHANNEL + "/devices/" + f->getId() + "/" + id;

    bool p1 = false, p2 = false;
    for (int i = 0; i < 10; i++)
    {
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

int totalFpReported = 0;
int totalSeen = 0;
int totalFpSeen = 0;
int totalFpQueried = 0;

void reportTask(void *parameter)
{
    connectToMqtt();

    while (true)
    {
        while (updateInProgress() || !mqttClient.connected())
            delay(1000);

        yield();
        auto copy = fingerprints.getCopy();

        int count = 0;
        for (auto i: copy)
            if (i->shouldCount())
                count++;

        yield();
        sendTelemetry(totalSeen, totalFpSeen, totalFpQueried, totalFpReported, count);
        yield();

        auto reported = 0;
        for (auto f : copy)
        {
            auto seen = f->getSeenCount();
            if (seen)
            {
                totalSeen += seen;
                totalFpSeen++;
            }
            if (reportDevice(f))
            {
                totalFpReported++;
                reported++;
            }
            yield();
        }
    }
}

void scanTask(void *parameter)
{
    NimBLEDevice::init("ESPresense");
    for (esp_ble_power_type_t i = ESP_BLE_PWR_TYPE_CONN_HDL0; i <= ESP_BLE_PWR_TYPE_CONN_HDL8; i = esp_ble_power_type_t((int)i + 1))
        NimBLEDevice::setPower(ESP_PWR_LVL_P9, i);
    Enrollment::Setup();
    NimBLEDevice::setMTU(23);

    auto pBLEScan = NimBLEDevice::getScan();
    pBLEScan->setInterval(BLE_SCAN_INTERVAL);
    pBLEScan->setWindow(BLE_SCAN_WINDOW);
    pBLEScan->setAdvertisedDeviceCallbacks(&fingerprints, true);
    pBLEScan->setActiveScan(activeScan);
    pBLEScan->setDuplicateFilter(false);
    pBLEScan->setMaxResults(0);
    if (!pBLEScan->start(0, nullptr, false))
        log_e("Error starting continuous ble scan");

    while (true)
    {
        for (auto f : *fingerprints.getNative())
            if (f->query())
                totalFpQueried++;

        if (updateInProgress())
        {
            fingerprints.setDisable(true);
            while (updateInProgress())
                delay(1000);
            fingerprints.setDisable(false);
        }
        else
        {
            Enrollment::Loop();
        }

        if (!pBLEScan->isScanning())
        {
            if (!pBLEScan->start(0, nullptr, true))
                log_e("Error re-starting continuous ble scan");
            delay(3000); // If we stopped scanning, don't query for 3 seconds in order for us to catch any missed broadcasts
        }
        else
        {
            delay(100);
        }
    }
}

#ifdef SENSORS

#endif

void setup()
{
    GUI::setup();

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

    spiffsInit();
    setupNetwork();
#if NTP
    setClock();
#endif
    Motion::Setup();
#if MACCHINA_A0
    pinMode(GPIO_NUM_35, INPUT);
#endif
#ifdef SENSORS
    DHT::Setup();
    I2C::Setup();
    BH1750::Setup();
    BME280::Setup();
    TSL2561::Setup();
    HX711::Setup();
#endif
    xTaskCreatePinnedToCore(scanTask, "scanTask", 7168, nullptr, 1, &scanTaskHandle, CONFIG_BT_NIMBLE_PINNED_TO_CORE);
    xTaskCreatePinnedToCore(reportTask, "reportTask", 7168, nullptr, 1, &reportTaskHandle, REPORT_PINNED_TO_CORE);
    configureOTA();
}

void loop()
{
    uint32_t freeHeap = ESP.getFreeHeap();
    if (arduinoOta && freeHeap > 4096)
        ArduinoOTA.handle();
    if (freeHeap < 10000) Serial.printf("Low memory: %u bytes free", freeHeap);
    firmwareUpdate();
    Motion::Loop();
    HttpServer::Loop();
#ifdef SENSORS
    DHT::Loop();
    BH1750::Loop();
    BME280::Loop();
    TSL2561::Loop();
    HX711::Loop();
    I2C::Loop();
#endif
}
