#include <main.h>

bool sendTelemetry(int totalSeen = -1, int totalReported = -1, int totalAdverts = -1)
{
    if (!online)
    {
        if (sendOnline() && sendDiscoveryConnectivity() && sendDiscoveryMaxDistance() && sendDiscoveryMotion())
        {
            online = true;
            reconnectTries = 0;
        }
        else
        {
            log_e("Error sending status=online");
        }
    }

    auto now = esp_timer_get_time();

    if (abs(now - lastTeleMicros) < 15000000)
        return false;

    lastTeleMicros = now;

    StaticJsonDocument<512> tele;
    tele["ip"] = localIp;
    tele["uptime"] = getUptimeSeconds();
    tele["firm"] = String(FIRMWARE);
    tele["rssi"] = WiFi.RSSI();

#ifdef VERSION
    tele["ver"] = String(VERSION);
#endif

    if (totalSeen > 0)
        tele["seen"] = totalSeen;
    if (totalReported > 0)
        tele["reported"] = totalReported;
    if (totalAdverts > 0)
        tele["adverts"] = totalAdverts;
    if (teleFails > 0)
        tele["teleFails"] = teleFails;
    if (reconnectTries > 0)
        tele["reconnectTries"] = reconnectTries;

    tele["freeHeap"] = ESP.getFreeHeap();
    tele["minFreeHeap"] = ESP.getMinFreeHeap();
    tele["maxAllocHeap"] = ESP.getMaxAllocHeap();
    tele["resetReason"] = resetReason(rtc_get_reset_reason(0));

    char teleMessageBuffer[512];
    serializeJson(tele, teleMessageBuffer);

    for (int i = 0; i < 10; i++)
    {
        if (!publishTele || mqttClient.publish(teleTopic.c_str(), 0, 0, teleMessageBuffer))
            return true;
        delay(50);
    }

    teleFails++;
    log_e("Error after 10 tries sending telemetry (%d times since boot)", teleFails);
    return false;
}

void connectToWifi()
{
    Serial.printf("Connecting to WiFi (%s)...\n", WiFi.macAddress().c_str());
    Display.update();

    WiFiSettings.onConnect = []()
    {
        Display.connected(false, false);
    };

    WiFiSettings.onFailure = []()
    {
        Display.status("WiFi Portal...");
    };
    WiFiSettings.onWaitLoop = []()
    {
        Display.connecting();
        return 150;
    };
    WiFiSettings.onPortalWaitLoop = []()
    {
        if (getUptimeSeconds() > 600)
            ESP.restart();
    };

    Display.connected(true, false);
    // Define custom settings saved by WifiSettings
    // These will return the default if nothing was set before
    mqttHost = WiFiSettings.string("mqtt_host", DEFAULT_MQTT_HOST);
    mqttPort = WiFiSettings.integer("mqtt_port", DEFAULT_MQTT_PORT);
    mqttUser = WiFiSettings.string("mqtt_user", DEFAULT_MQTT_USER);
    mqttPass = WiFiSettings.string("mqtt_pass", DEFAULT_MQTT_PASSWORD);
    room = WiFiSettings.string("room", ESPMAC);
    WiFiSettings.heading("Preferences");
    activeScan = WiFiSettings.checkbox("active_scan", true, "Active scanning (uses more battery but more results)");
    publishTele = WiFiSettings.checkbox("pub_tele", true, "Send to telemetry topic");
    publishRooms = WiFiSettings.checkbox("pub_rooms", true, "Send to rooms topic");
    publishDevices = WiFiSettings.checkbox("pub_devices", true, "Send to devices topic");
    discovery = WiFiSettings.checkbox("discovery", true, "Hass Discovery");
    pirPin = WiFiSettings.integer("pir_pin", 0, "PIR Motion Sensor pin (0 for disable)");
    radarPin = WiFiSettings.integer("radar_pin", 0, "Radar Motion pin (0 for disable)");
    maxDistance = WiFiSettings.integer("max_dist", DEFAULT_MAX_DISTANCE, "Maximum distance to report (in meters)");

    WiFiSettings.hostname = "espresense-" + room;

    if (slurp("/portal") == String("next"))
    {
        SPIFFS.remove("/portal");
        WiFiSettings.portal();
    }
    else if (!WiFiSettings.connect(true, 60))
        ESP.restart();

#ifdef VERSION
    Serial.println("Version:     " + String(VERSION));
#endif
    Serial.print("IP address:   ");
    Serial.println(WiFi.localIP());
    Serial.print("DNS address:  ");
    Serial.println(WiFi.dnsIP());
    Serial.print("Hostname:     ");
    Serial.println(WiFi.getHostname());
    Serial.print("Room:         ");
    Serial.println(room);
    Serial.print("Telemetry:    ");
    Serial.println(publishTele ? "enabled" : "disabled");
    Serial.print("Rooms:        ");
    Serial.println(publishRooms ? "enabled" : "disabled");
    Serial.print("Devices:      ");
    Serial.println(publishDevices ? "enabled" : "disabled");
    Serial.print("Discovery:    ");
    Serial.println(discovery ? "enabled" : "disabled");
    Serial.printf("Max Distance: %d\n", maxDistance);
    Serial.print("PIR Sensor:   ");
    Serial.println(pirPin ? "enabled" : "disabled");
    Serial.print("Radar Sensor: ");
    Serial.println(radarPin ? "enabled" : "disabled");

    localIp = WiFi.localIP().toString();
    roomsTopic = CHANNEL + "/rooms/" + room;
    statusTopic = roomsTopic + "/status";
    teleTopic = roomsTopic + "/telemetry";
    subTopic = roomsTopic + "/+/set";
}

void onMqttConnect(bool sessionPresent)
{
    xTimerStop(reconnectTimer, 0);
    mqttClient.subscribe(subTopic.c_str(), 2);
    Display.connected(true, true);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    Display.connected(true, false);
    log_e("Disconnected from MQTT; reason %d\n", reason);
    xTimerStart(reconnectTimer, 0);
    online = false;
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
    char new_payload[len + 1];
    new_payload[len] = '\0';
    strncpy(new_payload, payload, len);
    Serial.printf("%s: %s\n", topic, new_payload);

    String top = String(topic);
    String pay = String(new_payload);
    if (top == roomsTopic + "/max_distance/set")
    {
        maxDistance = pay.toInt();
        spurt("/max_dist", pay);
    }

    if (top == roomsTopic + "/portal/set")
    {
        spurt("/portal", pay);
        ESP.restart();
    }
}

void reconnect(TimerHandle_t xTimer)
{
    if (updateInProgress) return;
    if (WiFi.isConnected() && mqttClient.connected()) return;

    if (reconnectTries++ > 10)
    {
        log_e("Too many reconnect attempts; Restarting");
        ESP.restart();
    }

    if (!WiFi.isConnected())
    {
        Serial.println("Reconnecting to WiFi...");
        if (!WiFiSettings.connect(true, 60))
            ESP.restart();
    }

    Serial.println("Reconnecting to MQTT...");
    mqttClient.connect();
}

void connectToMqtt()
{
    reconnectTimer = xTimerCreate("reconnectionTimer", pdMS_TO_TICKS(3000), pdTRUE, (void *)0, reconnect);
    Serial.printf("Connecting to MQTT %s %d\n", mqttHost.c_str(), mqttPort);
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.setServer(mqttHost.c_str(), mqttPort);
    mqttClient.setWill(statusTopic.c_str(), 0, 1, "offline");
    mqttClient.setCredentials(mqttUser.c_str(), mqttPass.c_str());
    mqttClient.connect();
}

bool reportDevice(BleFingerprint *f)
{
    StaticJsonDocument<512> doc;
    if (!f->report(&doc, maxDistance))
        return false;

    char JSONmessageBuffer[512];
    serializeJson(doc, JSONmessageBuffer);

    String devicesTopic = CHANNEL + "/devices/" + f->getId() + "/" + room;

    bool p1 = false, p2 = false;
    for (int i = 0; i < 10; i++)
    {
        if (!mqttClient.connected())
            return false;

        if (!p1 && (!publishRooms || mqttClient.publish((char *)roomsTopic.c_str(), 0, 0, JSONmessageBuffer)))
            p1 = true;

        if (!p2 && (!publishDevices || mqttClient.publish((char *)devicesTopic.c_str(), 0, 0, JSONmessageBuffer)))
            p2 = true;

        if (p1 && p2)
            return true;
        delay(20);
    }
    teleFails++;
    return false;
}

void scanForDevices(void *parameter)
{
    BLEDevice::init("");
    auto pBLEScan = BLEDevice::getScan();
    pBLEScan->setInterval(BLE_SCAN_INTERVAL);
    pBLEScan->setWindow(BLE_SCAN_WINDOW);
    pBLEScan->setAdvertisedDeviceCallbacks(&fingerprints, true);
    if (activeScan) pBLEScan->setActiveScan(BLE_ACTIVE_SCAN);
    pBLEScan->setMaxResults(0);
    if (!pBLEScan->start(0, nullptr, false))
        log_e("Error starting continuous ble scan");

    while (1)
    {
        delay(1000);

        if (updateInProgress || !mqttClient.connected())
            continue;

        int totalSeen = 0;
        int totalReported = 0;

        auto seen = fingerprints.getSeen();

        for (auto it = seen.begin(); it != seen.end(); ++it)
        {
            totalSeen++;
            if (reportDevice(*it))
                totalReported++;
        }
        sendTelemetry(totalSeen, totalReported, fingerprints.getTotalAdverts());
    }
}

void setup()
{
    if (pirPin) pinMode(pirPin, INPUT);
    if (radarPin) pinMode(radarPin, INPUT);
#ifdef LED_BUILTIN
    pinMode(LED_BUILTIN, OUTPUT);
#endif

    Serial.begin(115200);
    Serial.setDebugOutput(true);
#ifdef VERBOSE
    esp_log_level_set("*", ESP_LOG_DEBUG);
#endif
    spiffsInit();
    connectToWifi();
#if NTP
    setClock();
#endif
    connectToMqtt();
    xTaskCreatePinnedToCore(scanForDevices, "BLE Scan", 5120, nullptr, 1, &scannerTask, 1);
    configureOTA();
}

void pirloop()
{
    if (!pirPin) return;
    int pirValue = digitalRead(pirPin);

    if (pirValue != lastPirValue)
    {
        if (pirValue == HIGH)
        {
            mqttClient.publish((roomsTopic + "/motion").c_str(), 0, 1, "on");
            Serial.println("PIR MOTION DETECTED!!!");
        }
        else
        {
            mqttClient.publish((roomsTopic + "/motion").c_str(), 0, 1, "off");
            Serial.println("NO PIR MOTION DETECTED!!!");
        }

        lastPirValue = pirValue;
    }
}

void radarloop()
{
    if (!radarPin) return;
    int radarValue = digitalRead(radarPin);

    if (radarValue != lastRadarValue)
    {
        if (radarValue == HIGH)
        {
            mqttClient.publish((roomsTopic + "/motion").c_str(), 0, 1, "on");
            Serial.println("Radar MOTION DETECTED!!!");
        }
        else
        {
            mqttClient.publish((roomsTopic + "/motion").c_str(), 0, 1, "off");
            Serial.println("NO Radar MOTION DETECTED!!!");
        }

        lastRadarValue = radarValue;
    }
}

void loop()
{
    ArduinoOTA.handle();
    firmwareUpdate();
    Display.update();
    pirloop();
    radarloop();
}
