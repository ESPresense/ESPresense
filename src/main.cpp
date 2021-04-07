#include <main.h>

BleFingerprint *getFingerprintInternal(BLEAdvertisedDevice *advertisedDevice)
{
    auto mac = advertisedDevice->getAddress();

    auto it = std::find_if(fingerprints.begin(), fingerprints.end(), [mac](BleFingerprint *f) { return f->getAddress() == mac; });
    if (it != fingerprints.end())
    {
        return *it;
    }

    auto created = new BleFingerprint(advertisedDevice, ONE_EURO_FCMIN, ONE_EURO_BETA, ONE_EURO_DCUTOFF);
    auto it2 = std::find_if(fingerprints.begin(), fingerprints.end(), [created](BleFingerprint *f) { return f->getId() == created->getId(); });
    if (it2 != fingerprints.end())
    {
        auto found = *it2;
        created->setInitial(found->getRSSI(), found->getDistance());
    }

    fingerprints.push_front(created);
    return created;
}

BleFingerprint *getFingerprint(BLEAdvertisedDevice *advertisedDevice)
{
    if (xSemaphoreTake(fingerprintSemaphore, 1000) != pdTRUE)
        log_e("Couldn't take semaphore!");
    auto f = getFingerprintInternal(advertisedDevice);
    if (xSemaphoreGive(fingerprintSemaphore) != pdTRUE)
        log_e("Couldn't give semaphore!");
    return f;
}

bool sendTelemetry(int totalSeen = -1, int totalReported = -1, int totalAdverts = -1)
{
    StaticJsonDocument<512> tele;
    tele["ip"] = localIp;
    tele["hostname"] = WiFi.getHostname();
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
    if (sendFailures > 0)
        tele["sendFails"] = sendFailures;
    if (reconnectTries > 0)
        tele["reconnectTries"] = reconnectTries;

    tele["freeHeap"] = ESP.getFreeHeap();
    tele["minFreeHeap"] = ESP.getMinFreeHeap();
    tele["heapSize"] = ESP.getHeapSize();
    tele["maxAllocHeap"] = ESP.getMaxAllocHeap();
    tele["resetReason"] = resetReason(rtc_get_reset_reason(0));

    char teleMessageBuffer[512];
    serializeJson(tele, teleMessageBuffer);

    for (int i = 0; i < 10; i++)
    {
        if (mqttClient.publish((TELEMETRY_TOPIC).c_str(), 0, 0, teleMessageBuffer) == true)
            return true;
        delay(20);
    }

    sendFailures++;
    log_e("Error sending telemetry");
    return false;
}

void connectToWifi()
{
    Serial.printf("Connecting to WiFi (%s)...", WiFi.macAddress().c_str());

    WiFiSettings.onSuccess = []() {
        digitalWrite(LED_BUILTIN, LED_BUILTIN_ON);
    };
    WiFiSettings.onFailure = []() {
        digitalWrite(LED_BUILTIN, !LED_BUILTIN_ON);
    };
    WiFiSettings.onWaitLoop = []() {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        return 500;
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
    reconnectTries = 0;

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
    Serial.println("Reconnecting...");

    if (updateInProgress) return;
    if (WiFi.isConnected() && mqttClient.connected()) return;

    if (reconnectTries++ > 10)
    {
        log_e("Too many reconnect attempts; Restarting");
        ESP.restart();
    }

    if (!WiFi.isConnected())
        if (!WiFiSettings.connect(true, 60))
            ESP.restart();

    mqttClient.connect();
}

void connectToMqtt()
{
    reconnectTimer = xTimerCreate("reconnectionTimer", pdMS_TO_TICKS(3000), pdTRUE, (void *)0, reconnect);
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

private:
    int totalSeen = 0;

    void onResult(BLEAdvertisedDevice *advertisedDevice)
    {
        if (updateInProgress)
            return;
        totalSeen++;
        digitalWrite(LED_BUILTIN, LED_BUILTIN_ON);
        BleFingerprint *f = getFingerprint(advertisedDevice);
        f->seen(advertisedDevice);
        digitalWrite(LED_BUILTIN, !LED_BUILTIN_ON);
    }
};

bool reportDevice(BleFingerprint *f)
{
    StaticJsonDocument<512> doc;
    if (!f->report(&doc, MAX_DISTANCE))
        return false;

    char JSONmessageBuffer[512];
    serializeJson(doc, JSONmessageBuffer);

    String publishTopic = CHANNEL + "/" + room;
    String publishTopic2 = CHANNEL + "/" + room + "/" + f->getId();

    bool p1 = false, p2 = false;
    for (int i = 0; i < 10; i++)
    {
        if (!mqttClient.connected())
            return false;

        if (!p1 && mqttClient.publish((char *)publishTopic.c_str(), 0, 0, JSONmessageBuffer))
            p1 = true;

        if (!p2 && mqttClient.publish((char *)publishTopic2.c_str(), 0, 0, JSONmessageBuffer))
            p2 = true;

        if (p1 && p2)
            return true;
        delay(20);
    }
    sendFailures++;
    return false;
}

void scanForDevices(void *parameter)
{
    fingerprintSemaphore = xSemaphoreCreateBinary();
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

        if (xSemaphoreTake(fingerprintSemaphore, 1000) != pdTRUE)
            log_e("Couldn't take semaphore!");
        cleanupOldFingerprints();
        std::list<BleFingerprint *> seen(fingerprints);
        if (xSemaphoreGive(fingerprintSemaphore) != pdTRUE)
            log_e("Couldn't give semaphore!");

        for (auto it = seen.begin(); it != seen.end(); ++it)
        {
            totalSeen++;
            if (reportDevice(*it))
                totalReported++;
        }
        sendTelemetry(totalSeen, totalReported, results.getTotalAdverts());
    }
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
    ArduinoOTA.handle();
    firmwareUpdate();
}
