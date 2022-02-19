#include "main.h"

bool sendTelemetry(int totalSeen, int totalFpSeen, int totalFpQueried, int totalFpReported)
{
    if (!online)
    {
        if (sendOnline())
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
        if (sendDiscoveryConnectivity() && sendDiscoveryUptime() && sendDiscoveryFreeMem() && sendButtonDiscovery("Restart", "diagnostic") && sendSwitchDiscovery("Status LED", "config") && sendNumberDiscovery("Max Distance", "config") && sendSwitchDiscovery("Active Scan", "config") && sendDeleteDiscovery("switch", "Query") && sendDiscoveryMotion()
#ifdef SENSORS
            && sendDiscoveryHumidity() && sendDiscoveryTemperature() && sendDiscoveryLux() && sendDiscoveryBME280Temperature() && sendDiscoveryBME280Humidity() && sendDiscoveryBME280Pressure() && sendDiscoveryTSL2561Lux()
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
    doc["firm"] = String(FIRMWARE);
    doc["rssi"] = WiFi.RSSI();
#ifdef MACCHINA_A0
    doc["batt"] = a0_read_batt_mv() / 1000.0f;
#endif
#ifdef VERSION
    doc["ver"] = String(VERSION);
#endif
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
    doc["minFreeHeap"] = ESP.getMinFreeHeap();
    doc["maxAllocHeap"] = ESP.getMaxAllocHeap();
    doc["resetReason"] = resetReason(rtc_get_reset_reason(0));

    char teleMessageBuffer[512];
    serializeJson(doc, teleMessageBuffer);

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
    Display.blit();

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
#ifdef VERSION
    WiFiSettings.info("ESPResense Version: " + String(VERSION));
#endif
    room = WiFiSettings.string("room", ESPMAC, "Room");

    WiFiSettings.heading("MQTT Connection");
    mqttHost = WiFiSettings.string("mqtt_host", DEFAULT_MQTT_HOST, "Server");
    mqttPort = WiFiSettings.integer("mqtt_port", DEFAULT_MQTT_PORT, "Port");
    mqttUser = WiFiSettings.string("mqtt_user", DEFAULT_MQTT_USER, "Username");
    mqttPass = WiFiSettings.string("mqtt_pass", DEFAULT_MQTT_PASSWORD, "Password");

    WiFiSettings.heading("Preferences");
    statusLed = WiFiSettings.checkbox("status_led", true, "Status LED");
    Display.setStatusLed(statusLed);
    autoUpdate = WiFiSettings.checkbox("auto_update", DEFAULT_AUTO_UPDATE, "Automatically Update");
    otaUpdate = WiFiSettings.checkbox("ota_update", DEFAULT_OTA_UPDATE, "Arduino OTA Update");
    discovery = WiFiSettings.checkbox("discovery", true, "Home Assistant Discovery");
    activeScan = WiFiSettings.checkbox("active_scan", false, "Active scanning (uses more battery but more results)");
    publishTele = WiFiSettings.checkbox("pub_tele", true, "Send to telemetry topic");
    publishRooms = WiFiSettings.checkbox("pub_rooms", true, "Send to rooms topic");
    publishDevices = WiFiSettings.checkbox("pub_devices", true, "Send to devices topic");

    WiFiSettings.heading("Filtering");
    query = WiFiSettings.string("query", DEFAULT_QUERY, "Query device ids for characteristics (eg. apple:1005:9-26)");
    if (query == "1") query = "apple:10"; // This is to keep query=true doing the same thing as older firmwares
    include = WiFiSettings.string("include", DEFAULT_INCLUDE, "If set will only send matching to mqtt (eg. apple:iphone10-6 apple:iphone13-2)");
    exclude = WiFiSettings.string("exclude", DEFAULT_EXCLUDE, "Exclude sending these ids to mqtt (eg. exp:20 apple:iphone10-6)");

    WiFiSettings.heading("Calibration");
    maxDistance = WiFiSettings.floating("max_dist", 0, 100, DEFAULT_MAX_DISTANCE, "Maximum distance to report (in meters)");
    forgetMs = WiFiSettings.integer("forget_ms", 0, 3000000, DEFAULT_FORGET_MS, "Forget beacon if not seen for (in miliiseconds)");
    skipDistance = WiFiSettings.floating("skip_dist", 0, 10, DEFAULT_SKIP_DISTANCE, "Update mqtt if beacon has moved more than this distance since last report (in meters)");
    skipMs = WiFiSettings.integer("skip_ms", 0, 3000000, DEFAULT_SKIP_MS, "Update mqtt if this time has elapsed since last report (in ms)");
    refRssi = WiFiSettings.integer("ref_rssi", -100, 100, DEFAULT_REF_RSSI, "Rssi expected from a 0dBm transmitter at 1 meter");

    WiFiSettings.heading("Additional Sensors");
    pirPin = WiFiSettings.integer("pir_pin", 0, "PIR motion pin (0 for disable)");
    radarPin = WiFiSettings.integer("radar_pin", 0, "Radar motion pin (0 for disable)");
#ifdef SENSORS
    dht11Pin = WiFiSettings.integer("dht11_pin", 0, "DHT11 sensor pin (0 for disable)");
    dht22Pin = WiFiSettings.integer("dht22_pin", 0, "DHT22 sensor pin (0 for disable)");

    WiFiSettings.heading("I2C Settings");

    I2CDebug = WiFiSettings.checkbox("I2CDebug", false, "Debug I2C addreses. Look at the serial log to get the correct address");

    WiFiSettings.html("h4", "Bus 1:");
    I2C_Bus_1_SDA = WiFiSettings.integer("I2C_Bus_1_SDA", 0, 39, DEFAULT_I2C_BUS_1_SDA, "SDA pin (0 to disable)");
    I2C_Bus_1_SCL = WiFiSettings.integer("I2C_Bus_1_SCL", 0, 39, DEFAULT_I2C_BUS_1_SCL, "SCL pin (0 to disable)");

    WiFiSettings.html("h4", "Bus 2:");

    I2C_Bus_2_SDA = WiFiSettings.integer("I2C_Bus_2_SDA", 0, "SDA pin (0 to disable)");
    I2C_Bus_2_SCL = WiFiSettings.integer("I2C_Bus_2_SCL", 0, "SCL pin (0 to disable)");

    WiFiSettings.heading("I2C Sensors");

    WiFiSettings.html("h4", "BH1750 - Ambient Light Sensor:");
    BH1750_I2c_Bus = WiFiSettings.integer("BH1750_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
    BH1750_I2c = WiFiSettings.string("BH1750_I2c", "", "I2C address (0x23 or 0x5C)");

    WiFiSettings.html("h4", "BME280 - Weather Sensor:");
    BME280_I2c_Bus = WiFiSettings.integer("BME280_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
    BME280_I2c = WiFiSettings.string("BME280_I2c", "", "I2C address (0x76 or 0x77)");

    WiFiSettings.html("h4", "TSL2561 - Ambient Light Sensor:");
    TSL2561_I2c_Bus = WiFiSettings.integer("TSL2561_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
    TSL2561_I2c = WiFiSettings.string("TSL2561_I2c", "", "I2C address (0x39, 0x49 or 0x29)");
    TSL2561_I2c_Gain = WiFiSettings.string("TSL2561_I2c_Gain", DEFAULT_TSL2561_I2C_GAIN, "Gain (auto, 1x or 16x)");
#endif

    WiFiSettings.hostname = "espresense-" + kebabify(room);

    if (!WiFiSettings.connect(true, 60))
        ESP.restart();

#ifdef VERSION
    Serial.println("Version:    " + String(VERSION));
#endif
    Serial.print("IP address:   ");
    Serial.println(WiFi.localIP());
    Serial.print("DNS address:  ");
    Serial.println(WiFi.dnsIP());
    Serial.print("Hostname:     ");
    Serial.println(WiFi.getHostname());
    Serial.print("Room:         ");
    Serial.println(room);
    Serial.printf("MQTT server:  %s:%d\n", mqttHost.c_str(), mqttPort);
    Serial.printf("Max Distance: %.2f\n", maxDistance);
    Serial.print("Telemetry:    ");
    Serial.println(publishTele ? "enabled" : "disabled");
    Serial.print("Rooms:        ");
    Serial.println(publishRooms ? "enabled" : "disabled");
    Serial.print("Devices:      ");
    Serial.println(publishDevices ? "enabled" : "disabled");
    Serial.print("Discovery:    ");
    Serial.println(discovery ? "enabled" : "disabled");
    Serial.print("PIR Sensor:   ");
    Serial.println(pirPin ? "enabled" : "disabled");
    Serial.print("Radar Sensor: ");
    Serial.println(radarPin ? "enabled" : "disabled");
#ifdef SENSORS
    Serial.print("DHT11 Sensor: ");
    Serial.println(dht11Pin ? "enabled" : "disabled");
    Serial.print("DHT22 Sensor: ");
    Serial.println(dht22Pin ? "enabled" : "disabled");
    Serial.print("BH1750_I2c Sensor: ");
    Serial.println(BH1750_I2c + " on bus " + BH1750_I2c_Bus);
    Serial.print("BME280_I2c Sensor: ");
    Serial.println(BME280_I2c + " on bus " + BME280_I2c_Bus);
    Serial.print("TSL2561_I2c Sensor: ");
    Serial.println(TSL2561_I2c + " on bus " + TSL2561_I2c_Bus);
    Serial.println(BH1750_I2c);
#endif
    Serial.print("Query:        ");
    Serial.println(query);
    Serial.print("Include:      ");
    Serial.println(include);
    Serial.print("Exclude:      ");
    Serial.println(exclude);

    localIp = WiFi.localIP().toString();
    id = slugify(room);
    roomsTopic = CHANNEL + "/rooms/" + id;
    statusTopic = roomsTopic + "/status";
    teleTopic = roomsTopic + "/telemetry";
    setTopic = roomsTopic + "/+/set";
}

void onMqttConnect(bool sessionPresent)
{
    xTimerStop(reconnectTimer, 0);
    mqttClient.subscribe("espresense/rooms/*/+/set", 1);
    mqttClient.subscribe(setTopic.c_str(), 1);
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

    auto setPos = top.lastIndexOf("/set");
    if (setPos <= 1) return;
    auto commandPos = top.lastIndexOf("/", setPos - 1);
    if (commandPos < 0) return;
    auto command = top.substring(commandPos + 1, setPos);

    if (command == "max_distance")
    {
        maxDistance = pay.toFloat();
        spurt("/max_dist", pay);
        online = false;
    }
    else if (command == "active_scan")
    {
        activeScan = pay == "ON";
        spurt("/active_scan", String(activeScan));
        online = false;
    }
    else if (command == "query")
    {
        query = pay;
        spurt("/query", String(query));
        online = false;
    }
    else if (command == "include")
    {
        include = pay;
        spurt("/include", String(include));
        online = false;
    }
    else if (command == "exclude")
    {
        exclude = pay;
        spurt("/exclude", String(exclude));
        online = false;
    }
    else if (command == "status_led")
    {
        statusLed = pay == "ON";
        spurt("/status_led", String(statusLed));
        Display.setStatusLed(statusLed);
        online = false;
    }
    else if (command == "restart")
    {
        ESP.restart();
    }

    fingerprints.setParams(refRssi, forgetMs, skipDistance, skipMs, maxDistance, include, exclude, query);
}

void reconnect(TimerHandle_t xTimer)
{
    Serial.printf("%d Reconnect timer\n", xPortGetCoreID());
    if (updateInProgress) return;
    if (WiFi.isConnected() && mqttClient.connected()) return;

    if (reconnectTries++ > 50)
    {
        log_e("Too many reconnect attempts; Restarting");
        ESP.restart();
    }

    if (!WiFi.isConnected())
    {
        Serial.printf("%d Reconnecting to WiFi...\n", xPortGetCoreID());
        if (!WiFiSettings.connect(true, 60))
            ESP.restart();
    }

    Serial.printf("%d Reconnecting to MQTT...\n", xPortGetCoreID());
    mqttClient.connect();
}

void connectToMqtt()
{
    reconnectTimer = xTimerCreate("reconnectionTimer", pdMS_TO_TICKS(3000), pdTRUE, (void *)0, reconnect);
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.setServer(mqttHost.c_str(), mqttPort);
    mqttClient.setWill(statusTopic.c_str(), 0, 1, offline.c_str());
    mqttClient.setCredentials(mqttUser.c_str(), mqttPass.c_str());
    mqttClient.connect();
}

bool reportDevice(BleFingerprint *f)
{
    doc.clear();
    if (!f->report(&doc))
        return false;

    char JSONmessageBuffer[512];
    serializeJson(doc, JSONmessageBuffer);

    String devicesTopic = CHANNEL + "/devices/" + f->getId() + "/" + id;

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
    connectToMqtt();
    fingerprints.setParams(refRssi, forgetMs, skipDistance, skipMs, maxDistance, include, exclude, query);
    BLEDevice::init(Stdprintf("ESPresense-%06" PRIx64, ESP.getEfuseMac() >> 24));
    for (esp_ble_power_type_t i = ESP_BLE_PWR_TYPE_CONN_HDL0; i <= ESP_BLE_PWR_TYPE_CONN_HDL8; i = esp_ble_power_type_t((int)i + 1))
        NimBLEDevice::setPower(ESP_PWR_LVL_P9, i);
    NimBLEDevice::setSecurityAuth(false, false, false);

    auto pBLEScan = BLEDevice::getScan();
    pBLEScan->setInterval(BLE_SCAN_INTERVAL);
    pBLEScan->setWindow(BLE_SCAN_WINDOW);
    pBLEScan->setAdvertisedDeviceCallbacks(&fingerprints, true);
    if (activeScan) pBLEScan->setActiveScan(true);
    pBLEScan->setMaxResults(0);
    if (!pBLEScan->start(0, nullptr, false))
        log_e("Error starting continuous ble scan");

    int totalSeen = 0;
    int totalFpSeen = 0;
    int totalFpQueried = 0;
    int totalFpReported = 0;

    while (1)
    {
        while (updateInProgress || !mqttClient.connected())
            delay(1000);

        yield();
        auto seen = fingerprints.getCopy();

        sendTelemetry(totalSeen, totalFpSeen, totalFpQueried, totalFpReported);
        yield();

        if (millis() - lastQueryMillis > 3000)
        {
            auto started = millis();
            for (auto it = seen.begin(); it != seen.end(); ++it)
            {
                auto f = (*it);
                if (f->query())
                    totalFpQueried++;

                if (millis() - started > 3000) break;
            }

            if (!pBLEScan->isScanning())
            {
                if (!pBLEScan->start(0, nullptr, false))
                    log_e("Error re-starting continuous ble scan");

                lastQueryMillis = millis(); // If we stopped scanning, don't query for 3 seconds in order for us to catch any missed broadcasts
            }
        }

        auto reported = 0;
        for (auto it = seen.begin(); it != seen.end(); ++it)
        {
            auto f = (*it);
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
        }
    }
}

#ifdef SENSORS
/**
 * Task to reads temperature from DHT11 sensor
 * @param pvParameters
 *		pointer to task parameters
 */
void tempTask(void *pvParameters)
{
    Serial.println("tempTask loop started");
    while (1) // tempTask loop
    {
        if (dhtTasksEnabled && !gotNewTemperature)
        {
            // Read temperature only if old data was processed already
            // Reading temperature for humidity takes about 250 milliseconds!
            // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
            dhtSensorData = dhtSensor.getTempAndHumidity(); // Read values from sensor 1
            gotNewTemperature = true;
        }
        vTaskSuspend(NULL);
    }
}

/**
 * triggerGetTemp
 * Sets flag dhtUpdated to true for handling in loop()
 * called by Ticker tempTicker
 */
void triggerGetTemp()
{
    if (dhtTempTaskHandle != NULL)
    {
        xTaskResumeFromISR(dhtTempTaskHandle);
    }
}
#endif

void setup()
{
    Display.setup();

#ifdef FAST_MONITOR
    Serial.begin(1500000);
#else
    Serial.begin(115200);
#endif
    Serial.setDebugOutput(true);

#ifdef VERBOSE
    esp_log_level_set("*", ESP_LOG_DEBUG);
#endif

    spiffsInit();
    connectToWifi();
#if NTP
    setClock();
#endif
    if (pirPin) pinMode(pirPin, INPUT);
    if (radarPin) pinMode(radarPin, INPUT);

#ifdef SENSORS
    if (dht11Pin) dhtSensor.setup(dht11Pin, DHTesp::DHT11);
    if (dht22Pin) dhtSensor.setup(dht22Pin, DHTesp::DHT22); //(AM2302)

    if (dht11Pin || dht22Pin)
    {
        // Start task to get temperature
        xTaskCreatePinnedToCore(
            tempTask,           /* Function to implement the task */
            "tempTask ",        /* Name of the task */
            4000,               /* Stack size in words */
            NULL,               /* Task input parameter */
            5,                  /* Priority of the task */
            &dhtTempTaskHandle, /* Task handle. */
            1);                 /* Core where the task should run */

        if (dhtTempTaskHandle == NULL)
        {
            Serial.println("[ERROR] Failed to start task for temperature update");
        }
        else
        {
            // Start update of environment data every 10 seconds
            tempTicker.attach(dhtUpdateTime, triggerGetTemp);
        }

        // Signal end of setup() to tasks
        dhtTasksEnabled = true;
    }

    if (I2C_Bus_1_SDA != 0 && I2C_Bus_1_SDA != 0) {
        Wire.begin(I2C_Bus_1_SDA, I2C_Bus_1_SCL);
        I2C_Bus_1_Enabled = true;
        Serial.println("\nInitialized I2C Bus 1");
    }

    if (I2C_Bus_2_SDA != 0 && I2C_Bus_2_SDA != 0) {
        Wire1.begin(I2C_Bus_2_SDA, I2C_Bus_2_SCL);
        I2C_Bus_2_Enabled = true;
        Serial.println("\nInitialized I2C Bus 2");
    }

    if (I2CDebug)
    {
        Serial.println("\nI2C Scanner");
    }


    if (I2C_Bus_1_Enabled || I2C_Bus_2_Enabled) {
        // BH1750_I2c
        // BH1750_updateFr
        if (BH1750_I2c == "0x23" || BH1750_I2c == "0x5C")
        {
            // Init BH1750 (witch default l2c adres)
            int rc; // Returncode
            long m; // milli for calibration
            bool state = false;

            // if (! BH1750.begin(BH1750_TO_GROUND))
            if (BH1750_I2c == "0x23" && BH1750_I2c_Bus == 1)
            {
                state = BH1750.begin(BH1750_TO_GROUND, &Wire);
            }
            else if (BH1750_I2c == "0x5C" && BH1750_I2c_Bus == 1)
            {
                state = BH1750.begin(BH1750_TO_VCC, &Wire);
            }
            else if (BH1750_I2c == "0x23" && BH1750_I2c_Bus == 2)
            {
                state = BH1750.begin(BH1750_TO_GROUND, &Wire1);
            }
            else if (BH1750_I2c == "0x5C" && BH1750_I2c_Bus == 2)
            {
                state = BH1750.begin(BH1750_TO_VCC, &Wire1);
            }

            if (!state)
            {
                Serial.println("Error on initialisation BH1750 GY-302");
            }
            else
            {
                sendDiscoveryLux();
                Serial.println("initialisation BH1750 GY-302 success");
                m = millis();
                rc = BH1750.calibrateTiming();
                m = millis() - m;
                Serial.print("Lux Sensor BH1750 GY-302 calibrated (Time: ");
                Serial.print(m);
                Serial.print(" ms)");
                if (rc != 0)
                {
                    Serial.print(" - Lux Sensor Error code ");
                    Serial.print(rc);
                }
                Serial.println();

                // start first measure and timecount
                lux_BH1750 = -1; // nothing to compare
                BH1750.start(BH1750_QUALITY_HIGH, 1);
                ms_BH1750 = millis();
            }
        }
    }
#endif
    xTaskCreatePinnedToCore(scanForDevices, "scanForDevices", 4000, nullptr, 1, &scannerTask, 1);
    configureOTA();
}

void pirLoop()
{
    if (!pirPin) return;
    int pirValue = digitalRead(pirPin);

    if (pirValue != lastPirValue)
    {
        if (pirValue == HIGH)
        {
            mqttClient.publish((roomsTopic + "/motion").c_str(), 0, 1, "ON");
            Serial.println("PIR MOTION DETECTED!!!");
        }
        else
        {
            mqttClient.publish((roomsTopic + "/motion").c_str(), 0, 1, "OFF");
            Serial.println("NO PIR MOTION DETECTED!!!");
        }

        lastPirValue = pirValue;
    }
}

void radarLoop()
{
    if (!radarPin) return;
    int radarValue = digitalRead(radarPin);

    if (radarValue != lastRadarValue)
    {
        if (radarValue == HIGH)
        {
            mqttClient.publish((roomsTopic + "/motion").c_str(), 0, 1, "ON");
            Serial.println("Radar MOTION DETECTED!!!");
        }
        else
        {
            mqttClient.publish((roomsTopic + "/motion").c_str(), 0, 1, "OFF");
            Serial.println("NO Radar MOTION DETECTED!!!");
        }

        lastRadarValue = radarValue;
    }
}

#ifdef SENSORS
void dhtLoop()
{
    if (!dht11Pin && !dht22Pin) return;

    if (gotNewTemperature)
    {
        float humidity = dhtSensorData.humidity;
        float temperature = dhtSensorData.temperature;
        Serial.println("Temp: " + String(temperature, 2) + "'C Humidity: " + String(humidity, 1) + "%");

        mqttClient.publish((roomsTopic + "/humidity").c_str(), 0, 1, String(humidity).c_str());
        mqttClient.publish((roomsTopic + "/temperature").c_str(), 0, 1, String(temperature).c_str());

        gotNewTemperature = false;
    }
}

//non blocking ambient sensor
void luxLoop()
{
    if (I2C_Bus_1_Enabled || I2C_Bus_2_Enabled) {

        if (BH1750_I2c == "0x23" || BH1750_I2c == "0x5C")
        {

            float lux;
            int lux_mqtt;

            if (BH1750.hasValue())
            {
                ms_BH1750 = millis() - ms_BH1750;
                if (!BH1750.saturated())
                {
                    lux = BH1750.getLux();
                    lux_mqtt = int(lux);

                    if (lux != lux_BH1750)
                    {
                        lux_BH1750 = lux;
                        // Serial.print("BH1750 (");
                        // Serial.print(ms_BH1750);
                        // Serial.print(" ms): ");
                        // Serial.print(lux);
                        // Serial.println(" lx");
                    }

                    //convert lx to integer to reduce mqtt traffic, send only if lx changed
                    if (lux_mqtt != lux_BH1750_MQTT)
                    {
                        lux_BH1750_MQTT = lux_mqtt;
                        Serial.print("BH1750 (");
                        Serial.print(ms_BH1750);
                        Serial.print(" ms): ");
                        Serial.print(lux_mqtt);
                        Serial.println(" lx");
                        mqttClient.publish((roomsTopic + "/lux").c_str(), 0, 1, String(lux_mqtt).c_str());
                    }
                }

                BH1750.adjustSettings(90);
                BH1750.start();
                ms_BH1750 = millis();
            }
        }
    }
}

void bme280Loop() {

    if (I2C_Bus_1_Enabled || I2C_Bus_2_Enabled) {

        if (BME280_I2c == "0x76" && BME280_I2c_Bus == 1) {
            BME280_status = BME280.begin(0x76, &Wire);
        } else if (BME280_I2c == "0x77" && BME280_I2c_Bus == 1) {
            BME280_status = BME280.begin(0x77, &Wire);
        } else if (BME280_I2c == "0x76" && BME280_I2c_Bus == 2) {
            BME280_status = BME280.begin(0x76, &Wire1);
        } else if (BME280_I2c == "0x77" && BME280_I2c_Bus == 2) {
            BME280_status = BME280.begin(0x77, &Wire1);
        } else {
            return;
        }

        if (!BME280_status) {
            Serial.println("[BME280] Couldn't find a sensor, check your wiring and I2C address!");
        }


        BME280.setSampling(Adafruit_BME280::MODE_NORMAL,
                        Adafruit_BME280::SAMPLING_X16,  // Temperature
                        Adafruit_BME280::SAMPLING_X16,  // Pressure
                        Adafruit_BME280::SAMPLING_X16,  // Humidity
                        Adafruit_BME280::FILTER_X16,
                        //Adafruit_BME280::FILTER_OFF,
                        Adafruit_BME280::STANDBY_MS_1000
        );


        float temperature = BME280.readTemperature();
        float humidity = BME280.readHumidity();
        float pressure = BME280.readPressure() / 100.0F;

        if (millis() - bme280PreviousMillis >= sensorInterval) {

            mqttClient.publish((roomsTopic + "/bme280_temperature").c_str(), 0, 1, String(temperature).c_str());
            mqttClient.publish((roomsTopic + "/bme280_humidity").c_str(), 0, 1, String(humidity).c_str());
            mqttClient.publish((roomsTopic + "/bme280_pressure").c_str(), 0, 1, String(pressure).c_str());

            bme280PreviousMillis = millis();
        }
    }
}

void tsl2561Loop() {

    if (I2C_Bus_1_Enabled || I2C_Bus_2_Enabled) {

        int tsl2561_address;

        if (TSL2561_I2c == "0x39") {
            tsl2561_address = 0x39;
        } else if (TSL2561_I2c == "0x29") {
            tsl2561_address = 0x29;
        } else if (TSL2561_I2c == "0x49") {
            tsl2561_address = 0x49;
        } else {
            return;
        }

        Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(tsl2561_address);

        if (TSL2561_I2c_Bus == 1) {
            tsl.begin(&Wire);
        } else if (TSL2561_I2c_Bus == 2) {
            tsl.begin(&Wire1);
        }

        if (TSL2561_I2c_Gain == "auto") {
            tsl.enableAutoRange(true);
        } else if (TSL2561_I2c_Gain == "1x") {
            tsl.setGain(TSL2561_GAIN_1X);
        } else if (TSL2561_I2c_Gain == "16x") {
            tsl.setGain(TSL2561_GAIN_16X);
        } else {
            Serial.println("[TSL2561] Invalid gain");
            return;
        }

        tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);

        sensors_event_t event;
        tsl.getEvent(&event);

        if (event.light) {
            if (millis() - tsl2561PreviousMillis >= sensorInterval) {
                mqttClient.publish((roomsTopic + "/tsl2561_lux").c_str(), 0, 1, String(event.light).c_str());

                tsl2561PreviousMillis = millis();
            }
        } else {
            Serial.println("[TSL2561] Sensor overloaded");
        }
    }
}

void l2cScanner()
{
    if (I2C_Bus_1_Enabled || I2C_Bus_2_Enabled) {

        if (!I2CDebug) return;

        byte error, address;
        int nDevices;
        Serial.println("Scanning I2C device...");
        nDevices = 0;

        for (address = 1; address < 127; address++)
        {
            Wire.beginTransmission(address);
            error = Wire.endTransmission();
            if (error == 0)
            {
                Serial.print("I2C device found on bus 1 at address 0x");

                if (address < 16)
                {
                    Serial.print("0");
                }

                Serial.println(address, HEX);
                nDevices++;
            }
            else if (error == 4)
            {
                Serial.print("Unknow error on bus 1 at address 0x");
                if (address < 16)
                {
                    Serial.print("0");
                }
                Serial.println(address, HEX);
            }
        }

        for (address = 1; address < 127; address++)
        {
            Wire1.beginTransmission(address);
            error = Wire1.endTransmission();
            if (error == 0)
            {
                Serial.print("I2C device found on bus 2 at address 0x");

                if (address < 16)
                {
                    Serial.print("0");
                }

                Serial.println(address, HEX);
                nDevices++;
            }
            else if (error == 4)
            {
                Serial.print("Unknow error on bus 2 at address 0x");
                if (address < 16)
                {
                    Serial.print("0");
                }
                Serial.println(address, HEX);
            }
        }

        if (nDevices == 0)
        {
            Serial.println("No I2C devices found\n");
        }
        else
        {
            Serial.println("done\n");
            I2CDebug = false;
        }
        delay(5000);
    }
}
#endif

void loop()
{
    if (otaUpdate)
        ArduinoOTA.handle();
    firmwareUpdate();
    Display.blit();
    pirLoop();
    radarLoop();
#ifdef SENSORS
    dhtLoop();
    luxLoop();
    bme280Loop();
    tsl2561Loop();
    l2cScanner();
#endif
    WiFiSettings.httpLoop();
}
