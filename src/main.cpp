#include <main.h>

BleFingerprint *getFingerprint(BLEAdvertisedDevice *advertisedDevice)
{
    auto mac = advertisedDevice->getAddress();

    auto it = std::find_if(fingerprints.begin(), fingerprints.end(), [mac](BleFingerprint *f) { return f->getAddress() == mac; });
    if (it != fingerprints.end())
    {
        return *it;
    }

    if (fingerprints.size() >= MAX_MAC_ADDRESSES)
    {
        delete fingerprints.back();
        fingerprints.pop_back();
    }

    auto created = new BleFingerprint(advertisedDevice);
    auto it2 = std::find_if(fingerprints.begin(), fingerprints.end(), [created](BleFingerprint *f) { return f->getId() == created->getId(); });
    if (it2 != fingerprints.end())
    {
        auto found = *it2;
        created->setDistance(found->getDistance());
    }

    fingerprints.push_front(created);
    return created;
}

bool sendTelemetry(int totalSeen = -1, int totalReported = -1, int totalAdverts = -1)
{
    StaticJsonDocument<512> tele;
    tele["ip"] = localIp;
    tele["hostname"] = WiFi.getHostname();
    tele["uptime"] = getUptimeSeconds();
    tele["firm"] = String(FIRMWARE);

#ifdef VERSION
    tele["ver"] = String(VERSION);
#endif

    if (totalSeen > 0)
        tele["seen"] = totalSeen;
    if (totalReported > 0)
        tele["reported"] = totalReported;
    if (totalAdverts > 0)
        tele["adverts"] = totalAdverts;
    if (sendFailures > 0)
        tele["sendFails"] = sendFailures;

    tele["resetCore0"] = resetReason(rtc_get_reset_reason(0));
    tele["resetCore1"] = resetReason(rtc_get_reset_reason(1));

    tele["freeHeap"] = ESP.getFreeHeap();
    tele["minFreeHeap"] = ESP.getMinFreeHeap();
    tele["heapSize"] = ESP.getHeapSize();
    tele["maxAllocHeap"] = ESP.getMaxAllocHeap();

    char teleMessageBuffer[512];
    serializeJson(tele, teleMessageBuffer);

    if (mqttClient.publish((TELEMETRY_TOPIC).c_str(), 0, 0, teleMessageBuffer) == true)
    {
        return true;
    }
    else
    {
        sendFailures++;
        log_e("Error sending telemetry");
        return false;
    }
}

void connectToWifi()
{
    Serial.printf("Connecting to WiFi (%s)...", WiFi.macAddress().c_str());

    // Set custom callback functions
    WiFiSettings.onSuccess = []() {
        digitalWrite(LED_BUILTIN, LED_BUILTIN_ON); // Turn LED on
    };
    WiFiSettings.onFailure = []() {
        digitalWrite(LED_BUILTIN, !LED_BUILTIN_ON); // Turn LED off
    };
    WiFiSettings.onWaitLoop = []() {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Toggle LED
        return 500;                                           // Delay next function call by 500ms
    };
    WiFiSettings.onPortalWaitLoop = []() {
        if (getUptimeSeconds() > 600)
            ESP.restart();
    };

    // Define custom settings saved by WifiSettings
    // These will return the default if nothing was set before
    mqttHost = WiFiSettings.string("mqtt_host", DEFAULT_MQTT_HOST);
    mqttPort = WiFiSettings.integer("mqtt_port", DEFAULT_MQTT_PORT);
    mqttUser = WiFiSettings.string("mqtt_user", DEFAULT_MQTT_USER);
    mqttPass = WiFiSettings.string("mqtt_pass", DEFAULT_MQTT_PASSWORD);
    room = WiFiSettings.string("room", ESPMAC);
    availabilityTopic = AVAILABILITY_TOPIC;

    WiFiSettings.hostname = "mqtt-room-" + room;

    if (!WiFiSettings.connect(true, 60))
        ESP.restart();

    Serial.print("IP address:  ");
    Serial.println(WiFi.localIP());
    Serial.print("DNS address: ");
    Serial.println(WiFi.dnsIP());
    Serial.print("Hostname:    ");
    Serial.println(WiFi.getHostname());
    Serial.print("Room:        ");
    Serial.println(room);

    localIp = WiFi.localIP().toString();
}

void onMqttConnect(bool sessionPresent)
{
    Serial.println("Connected to MQTT");
    retryAttempts = 0;

    if (mqttClient.publish(availabilityTopic.c_str(), 0, 1, "CONNECTED") == true)
    {
        Serial.printf("Success sending presence message to: %s\n", availabilityTopic.c_str());
        if (sendTelemetry())
            xTimerStop(reconnectTimer, 0);
    }
    else
    {
        Serial.println(F("Error sending presence message"));
    }
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    Serial.printf("Disconnected from MQTT; reason %d\n", (int)reason);
    xTimerStart(reconnectTimer, 0);
}

void reconnect(TimerHandle_t xTimer)
{
    if (updateInProgress)
        return;

    if (WiFi.isConnected() && mqttClient.connected())
        return;

    if (retryAttempts > 10)
    {
        log_e("Too many retries. Restarting");
        ESP.restart();
    }
    else
    {
        retryAttempts++;
    }
    if (!WiFi.isConnected())
        if (!WiFiSettings.connect(true, 60))
            ESP.restart();

    mqttClient.connect();
}

void connectToMqtt()
{
    reconnectTimer = xTimerCreate("reconnectionTimer", pdMS_TO_TICKS(15000), pdTRUE, (void *)0, reconnect);
    Serial.printf("Connecting to MQTT %s %d\n", mqttHost.c_str(), mqttPort);
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.setServer(mqttHost.c_str(), mqttPort);
    mqttClient.setWill(availabilityTopic.c_str(), 0, 1, "DISCONNECTED");
    mqttClient.setKeepAlive(60);
    mqttClient.setCredentials(mqttUser.c_str(), mqttPass.c_str());
    mqttClient.connect();
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
public:
    int getTotalAdverts() { return totalSeen; }
    std::set<BleFingerprint *> getSeen() { return seen; }

private:
    int totalSeen = 0;
    std::set<BleFingerprint *> seen;

    void onResult(BLEAdvertisedDevice *advertisedDevice)
    {
        totalSeen++;
        digitalWrite(LED_BUILTIN, LED_BUILTIN_ON);
        BleFingerprint *f = getFingerprint(advertisedDevice);
        f->seen(advertisedDevice);
        seen.insert(f);
        digitalWrite(LED_BUILTIN, !LED_BUILTIN_ON);
    }
};

bool reportDevice(BleFingerprint *f)
{
    StaticJsonDocument<512> doc;

    if (!f->report(&doc))
        return false;

    char JSONmessageBuffer[512];
    serializeJson(doc, JSONmessageBuffer);
    String id = doc["id"];
    String publishTopic = CHANNEL + "/" + room;
    String publishTopic2 = CHANNEL + "/" + room + "/" + id;

    if (mqttClient.connected())
    {
        if (MAX_DISTANCE == 0 || doc["distance"] < MAX_DISTANCE)
        {
            if (mqttClient.publish((char *)publishTopic.c_str(), 0, 0, JSONmessageBuffer) && mqttClient.publish((char *)publishTopic2.c_str(), 0, 0, JSONmessageBuffer))
            {
#if VERBOSE
//                Serial.println(JSONmessageBuffer);
#endif
                return true;
            }
            else
            {
                sendFailures++;
                Serial.print("Error sending message: ");
                Serial.println(publishTopic);
                Serial.print("Message: ");
                Serial.println(JSONmessageBuffer);
                return false;
            }
        }
        else
        {
            //Serial.printf("%s exceeded distance threshold %.2f\n\r", mac_address.c_str(), distance);
            return false;
        }
    }
    return false;
}

void scanForDevices(void *parameter)
{
    auto scan = MyAdvertisedDeviceCallbacks();
    BLEDevice::init("");
    auto pBLEScan = BLEDevice::getScan();
    pBLEScan->setInterval(BLE_SCAN_INTERVAL);
    pBLEScan->setWindow(BLE_SCAN_WINDOW);
    pBLEScan->setAdvertisedDeviceCallbacks(&scan, true);
    pBLEScan->setActiveScan(BLE_ACTIVE_SCAN);
    pBLEScan->setMaxResults(0);
    if (!pBLEScan->start(0, nullptr, false))
        log_e("Error starting continuous ble scan");

    while (1)
    {
        delay(1000);

        if (updateInProgress || !mqttClient.connected())
            continue;

        auto results = scan;
        pBLEScan->setAdvertisedDeviceCallbacks(&(scan = MyAdvertisedDeviceCallbacks()), true);

        int totalSeen = 0;
        int totalReported = 0;
        auto seen = results.getSeen();
        for (auto it = seen.begin(); it != seen.end(); ++it)
        {
            totalSeen++;
            if (reportDevice(*it))
                totalReported++;
        }
        sendTelemetry(totalSeen, totalReported, results.getTotalAdverts());
    }
}

void spiffsInit()
{
    int ledState = HIGH;
    digitalWrite(LED_BUILTIN, ledState);

#ifdef BUTTON

    pinMode(BUTTON, INPUT);
    int flashes = 0;
    unsigned long debounceDelay = 250;

    long lastDebounceTime = millis();
    while (digitalRead(BUTTON) == BUTTON_PRESSED)
    {
        if ((millis() - lastDebounceTime) > debounceDelay)
        {
            ledState = !ledState;
            digitalWrite(LED_BUILTIN, ledState);
            lastDebounceTime = millis();
            flashes++;

            if (flashes > 10)
            {
                Serial.println(F("Resetting back to defaults..."));
                digitalWrite(LED_BUILTIN, 1);
                SPIFFS.format();
                SPIFFS.begin(true);
                digitalWrite(LED_BUILTIN, 0);

                return;
            }
        }
    }

#endif

    SPIFFS.begin(true);
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    Serial.setDebugOutput(true);
#if VERBOSE
    esp_log_level_set("*", ESP_LOG_DEBUG);
#endif
    spiffsInit();
    connectToWifi();
    setClock();
    connectToMqtt();
    xTaskCreatePinnedToCore(scanForDevices, "BLE Scan", 4096, nullptr, 1, &scannerTask, 1);

#ifdef M5STICK
    M5.begin();
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
#endif

    configureOTA();
}

void loop()
{
    TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed = 1;
    TIMERG0.wdt_wprotect = 0;

    ArduinoOTA.handle();
    firmwareUpdate();
}
