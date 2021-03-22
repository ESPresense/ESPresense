/*
     ESP32 Bluetooth Low Energy presence detection, for use with MQTT.

     Project and documentation are available on GitHub at https://jptrsn.github.io/ESP32-mqtt-room/

     Some giants upon whose shoulders the project stands -- major thanks to:

     pcbreflux for the original version of this code, as well as the eddystone handlers https://github.com/pcbreflux

     Andreis Speiss for his work on YouTube and his invaluable github at https://github.com/sensorsiot.

     Sidddy for the implementation of Mi Flora plant sensor support. https://github.com/sidddy/flora

     Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
     Ported to Arduino ESP32 by Evandro Copercini
*/
#include <WiFi.h>
extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

#include <AsyncTCP.h>

#include <AsyncMqttClient.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <HTTPUpdate.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include <WebServer.h>
#include <WiFiSettings.h>
#include <SPIFFS.h>

#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include "NimBLEEddystoneURL.h"
#include "NimBLEEddystoneTLM.h"
#include "NimBLEBeacon.h"

#include "Settings.h"

#include "BleFingerprint.h"

#ifdef M5STICK
#ifdef PLUS
#include <M5StickCPlus.h>
#else
#include <M5StickC.h>
#endif
#endif

#ifdef htuSensorTopic
#define tempTopic htuSensorTopic "/temperature"
#define humidityTopic htuSensorTopic "/humidity"
#include "sensors/sensor_htu21d.h"
#endif

#define MAX_MAC_ADDRESSES 50
#define CHECK_FOR_UPDATES_MILI 100000

WiFiClient espClient;
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;
bool updateInProgress = false;
String localIp;
byte retryAttempts = 0;
unsigned long last = 0;
BLEScan *pBLEScan;
TaskHandle_t thBLEScan;

String mqttHost;
int mqttPort;
String mqttUser;
String mqttPass;

String room;
String hostname;

static BleFingerprint *fingerprints[MAX_MAC_ADDRESSES];

int mac_pos(const uint8_t mac[6])
{
#ifdef ALLOWED_LIST_CHECK
    if (allowedListCheck)
    {
        bool allowedListFound = false;
        for (uint32_t x = 0; x < allowedListNumberOfItems; x++)
        {
            if (mac_address == allowedList[x])
            {
                allowedListFound = true;
            }
        }

        if (allowedListFound == false)
        {
            return false;
        }
    }
#endif

    static uint8_t mac_lib[MAX_MAC_ADDRESSES][6];
    static unsigned int mac_count = 0;

    for (int i = 0; i < mac_count && i < MAX_MAC_ADDRESSES; i++)
    {
        bool flag = true;
        for (uint8_t j = 0; j < 6; j++)
        {
            if (mac_lib[i][j] != mac[j])
            {
                flag = false;
                break;
            }
        }
        if (flag == true)
            return i;
    }
    uint8_t dest = mac_count % MAX_MAC_ADDRESSES;
    for (uint8_t j = 0; j < 6; j++)
        mac_lib[dest][j] = mac[j];
    mac_count++;
    return dest;
}

BleFingerprint *getFingerprint(BLEAdvertisedDevice *advertisedDevice)
{
    int pos = mac_pos(advertisedDevice->getAddress().getNative());
    //Serial.printf("%d\n", pos);

    BleFingerprint *f = fingerprints[pos];
    if (f)
        return f;

    return fingerprints[pos] = new BleFingerprint(advertisedDevice, MAX_DISTANCE);
}

/**
 * CalculateUptimeSeconds()
 *
 * Handle millis() rollover and calculate the total uptime in seconds.
 * This function must be called at least once for every 50 days to be
 * able to see the rollover.
 */
unsigned long CalculateUptimeSeconds(void)
{
    static unsigned int _rolloverCount = 0; // Number of 0xFFFFFFFF rollover we had in millis()
    static unsigned long _lastMillis = 0;   // Value of the last millis()

    // Get the current milliseconds uptime from the system.
    // Note: This only works as long as no one else did hook up with timer0
    //       because the arduino system uses timer0 to manage delay() and millis().
    unsigned long currentMilliSeconds = millis();

    // If we had a rollover we count that.
    if (currentMilliSeconds < _lastMillis)
    {
        _rolloverCount++;
    }

    // Now store the current number of milliseconds for the next round.
    _lastMillis = currentMilliSeconds;

    // Based on the current milliseconds and the number of rollovers
    // we had in total we calculate here the uptime in seconds since
    // poweron or reset.
    // Caution: Because we shorten millis to seconds we may miss one
    // second for every rollover (1 second every 50 days).
    return (0xFFFFFFFF / 1000) * _rolloverCount + (_lastMillis / 1000);
}

#ifdef htuSensorTopic

void reportSensorValues()
{
    if (htuSensorIsConnected())
    {
        char temp[8];
        char humidity[8];

        dtostrf(getTemp(), 0, 1, temp);         // convert float to string with one decimal place precision
        dtostrf(getHumidity(), 0, 1, humidity); // convert float to string with one decimal place precision

        if (mqttClient.publish(tempTopic, 0, 0, temp) == true)
        {
            Serial.printf("Temperature %s sent\t", temp);
        }

        if (mqttClient.publish(humidityTopic, 0, 0, humidity) == true)
        {
            Serial.printf("Humidity %s sent\n\r", humidity);
        }
    }
}

#endif

bool sendTelemetry(int deviceCount = -1, int reportCount = -1)
{
    StaticJsonDocument<256> tele;
    tele["room"] = room;
    tele["ip"] = localIp;
    tele["hostname"] = WiFi.getHostname();
    tele["scan_dur"] = BLE_SCAN_DURATION;
    tele["max_dist"] = MAX_DISTANCE;
    tele["uptime"] = CalculateUptimeSeconds();
    tele["firm"] = String(FIRMWARE);
#ifdef VERSION
    tele["ver"] = String(VERSION);
#endif

    if (deviceCount > -1)
    {
        Serial.printf("devices_discovered: %d\n\r", deviceCount);
        tele["disc_ct"] = deviceCount;
    }

    if (reportCount > -1)
    {
        Serial.printf("devices_reported: %d\n\r", reportCount);
        tele["rept_ct"] = reportCount;
    }

    char teleMessageBuffer[258];
    serializeJson(tele, teleMessageBuffer);

#ifdef htuSensorTopic
    reportSensorValues();
#endif

    if (mqttClient.publish((TELEMETRY_TOPIC).c_str(), 0, 0, teleMessageBuffer) == true)
    {
        Serial.println("Telemetry sent");
        return true;
    }
    else
    {
        Serial.println("Error sending telemetry");
        return false;
    }
}

void connectToWifi()
{
    Serial.println("Connecting to WiFi...");
    pinMode(LED_BUILTIN, OUTPUT);

    // Set custom callback functions
    WiFiSettings.onSuccess = []() {
        digitalWrite(LED_BUILTIN, LED_ON); // Turn LED on
    };
    WiFiSettings.onFailure = []() {
        digitalWrite(LED_BUILTIN, !LED_ON); // Turn LED off
    };
    WiFiSettings.onWaitLoop = []() {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Toggle LED
        return 500;                                           // Delay next function call by 500ms
    };
    WiFiSettings.onPortalWaitLoop = []() {
        if (CalculateUptimeSeconds() > 600)
            ESP.restart();
    };

    // Define custom settings saved by WifiSettings
    // These will return the default if nothing was set before
    mqttHost = WiFiSettings.string("mqtt_host", DEFAULT_MQTT_HOST);
    mqttPort = WiFiSettings.integer("mqtt_port", DEFAULT_MQTT_PORT);
    mqttUser = WiFiSettings.string("mqtt_user", DEFAULT_MQTT_USER);
    mqttPass = WiFiSettings.string("mqtt_pass", DEFAULT_MQTT_PASSWORD);
    room = WiFiSettings.string("room", DEFAULT_ROOM);
    hostname = room;

    if (!WiFiSettings.connect(true, 60))
        ESP.restart();

    Serial.print("IP address: \t");
    Serial.println(WiFi.localIP());
    Serial.print("Hostname: \t");
    Serial.println(WiFi.getHostname());

    localIp = WiFi.localIP().toString();
}

void connectToMqtt()
{
    if (WiFi.isConnected() && !updateInProgress)
    {
        Serial.println("Connecting to MQTT");
        mqttClient.setCredentials(mqttUser.c_str(), mqttPass.c_str());
        mqttClient.setClientId(hostname.c_str());
        mqttClient.connect();
    }
}

void handleMqttDisconnect()
{
    if (updateInProgress)
        return;

    if (retryAttempts > 10)
    {
        Serial.println("Too many mqtt retries. Restarting");
        WiFiSettings.portal();
    }
    else
    {
        retryAttempts++;
    }

    if (WiFi.isConnected() && !updateInProgress)
    {
        Serial.println("Starting MQTT reconnect timer");
        xTimerReset(mqttReconnectTimer, 0);
    }
}

void handleWifiDisconnect()
{
    if (WiFi.isConnected())
    {
        Serial.println("WiFi appears to be connected. Not retrying.");
        return;
    }
    if (retryAttempts > 10)
    {
        Serial.println("Too many retries. Restarting");
        ESP.restart();
    }
    else
    {
        retryAttempts++;
    }

    if (xTimerIsTimerActive(mqttReconnectTimer) != pdFALSE)
    {
        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    }

    xTimerReset(wifiReconnectTimer, 0);
}

void onMqttConnect(bool sessionPresent)
{
    Serial.println("Connected to MQTT");
    retryAttempts = 0;

    if (mqttClient.publish((AVAILABILITY_TOPIC).c_str(), 0, 1, "CONNECTED") == true)
    {
        Serial.print("Success sending message to topic:\t");
        Serial.println(AVAILABILITY_TOPIC);
    }
    else
    {
        Serial.println("Error sending message");
    }

    sendTelemetry();
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    Serial.println("Disconnected from MQTT.");
    handleMqttDisconnect();
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice *advertisedDevice)
    {
        digitalWrite(LED_BUILTIN, LED_ON);
        //Serial.printf("Advertised Device: %s \n", advertisedDevice->toString().c_str());
        BleFingerprint *f = getFingerprint(advertisedDevice);
        f->seen(advertisedDevice);
        vTaskDelay(advertisedDevice->getRSSI() > -60 ? 2 : 1);
        digitalWrite(LED_BUILTIN, !LED_ON);
    }
};

bool reportDevice(BLEAdvertisedDevice advertisedDevice)
{
    BleFingerprint *f = getFingerprint(&advertisedDevice);

    f->report(&advertisedDevice);
    StaticJsonDocument<500> doc = f->getJson();
    char JSONmessageBuffer[512];
    serializeJson(doc, JSONmessageBuffer);
    String id = doc["id"];
    String publishTopic = CHANNEL + "/" + room;
    String publishTopic2 = CHANNEL + "/" + room + "/" + id;

    if (mqttClient.connected())
    {
        if (MAX_DISTANCE == 0 || doc["distance"] < MAX_DISTANCE)
        {
            if (mqttClient.publish((char *)publishTopic.c_str(), 0, 0, JSONmessageBuffer) == true)
            {
                return (mqttClient.publish((char *)publishTopic2.c_str(), 0, 0, JSONmessageBuffer) == true);
            }
            else
            {
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
    else
    {

        Serial.println("MQTT disconnected.");
        if (xTimerIsTimerActive(mqttReconnectTimer) != pdFALSE)
        {
            TickType_t xRemainingTime = xTimerGetExpiryTime(mqttReconnectTimer) - xTaskGetTickCount();
            Serial.print("Time remaining: ");
            Serial.println(xRemainingTime);
        }
        else
        {
            handleMqttDisconnect();
        }
    }
    return false;
}

void scanForDevices(void *parameter)
{
    int i = 0;
    while (1)
    {
        i++;
        if (!updateInProgress)
        {
            pBLEScan->setActiveScan(i % 4 == 0 ? BLE_ACTIVE_SCAN : false);
            if (i % 4 == 0)
                Serial.print("Scanning (ACTIVE)...\t");
            else
                Serial.print("Scanning...\t");
            BLEScanResults foundDevices = pBLEScan->start(BLE_SCAN_DURATION);
            int devicesCount = foundDevices.getCount();
            Serial.printf("Scan done! Devices found: %d\n\r", devicesCount);

#ifdef M5STICK
            M5.Lcd.setCursor(0, 0);
            M5.Lcd.fillScreen(TFT_BLACK);

#endif

            int devicesReported = 0;
            if (mqttClient.connected())
            {
                for (uint32_t i = 0; i < devicesCount; i++)
                {
                    bool included = reportDevice(foundDevices.getDevice(i));
                    if (included)
                    {
                        devicesReported++;
                    }
                }
                sendTelemetry(devicesCount, devicesReported);
                pBLEScan->clearResults();
            }
            else
            {
                Serial.println("Cannot report; mqtt disconnected");
                if (xTimerIsTimerActive(mqttReconnectTimer) != pdFALSE)
                {
                    TickType_t xRemainingTime = xTimerGetExpiryTime(mqttReconnectTimer) - xTaskGetTickCount();
                    Serial.print("Time remaining: ");
                    Serial.println(xRemainingTime);
                }
                else
                {
                    handleMqttDisconnect();
                }
            }
            last = millis();
        }
    }
}

void configureOTA()
{
    ArduinoOTA
        .onStart([]() {
            Serial.println("OTA Start");
            pBLEScan->stop();
            updateInProgress = true;
            mqttClient.disconnect(true);
            xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
        })
        .onEnd([]() {
            updateInProgress = false;
            digitalWrite(LED_BUILTIN, !LED_ON);
            Serial.println("\n\rEnd");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            byte percent = (progress / (total / 100));
            Serial.printf("Progress: %u\r\n", percent);
            digitalWrite(LED_BUILTIN, percent % 2);
        })
        .onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR)
                Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR)
                Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR)
                Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR)
                Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR)
                Serial.println("End Failed");
            ESP.restart();
        });
    ArduinoOTA.setHostname(hostname.c_str());
    ArduinoOTA.setPort(3232);
    ArduinoOTA.begin();
}

void setClock()
{
    configTime(0, 0, "pool.ntp.org", "time.nist.gov"); // UTC

    Serial.print(F("Waiting for NTP time sync: "));
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2)
    {
        yield();
        delay(500);
        now = time(nullptr);
    }

    Serial.println(F(""));
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeinfo));
}

void firmwareUpdate(void)
{
#ifdef VERSION
    static long lastFirmwareCheck;
    if (millis() - lastFirmwareCheck < CHECK_FOR_UPDATES_MILI || WiFi.status() != WL_CONNECTED)
        return;

    lastFirmwareCheck = millis();

    WiFiClientSecure client;
    client.setInsecure();

    httpUpdate.setLedPin(LED_BUILTIN, LOW);
    httpUpdate.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
#ifdef M5STICK
#ifdef PLUS
    String firmwareUrl = String("https://github.com/DTTerastar/ESP32-mqtt-room/releases/latest/download/m5stickc-plus.bin");
#else
    String firmwareUrl = String("https://github.com/DTTerastar/ESP32-mqtt-room/releases/latest/download/m5stickc.bin");
#endif
#else
#ifdef M5ATOM
    String firmwareUrl = String("https://github.com/DTTerastar/ESP32-mqtt-room/releases/latest/download/m5stack-atom.bin");
#else
    String firmwareUrl = String("https://github.com/DTTerastar/ESP32-mqtt-room/releases/latest/download/esp32.bin");
#endif
#endif

    HTTPClient http;
    if (!http.begin(client, firmwareUrl))
        return;

    int httpCode = http.sendRequest("HEAD");
    if (httpCode != 302 || http.getLocation().indexOf(String(VERSION)) > 0)
    {
        http.end();
        return;
    }

    Serial.printf("Updating from %s\n", firmwareUrl.c_str());
    t_httpUpdate_return ret = httpUpdate.update(client, firmwareUrl);

    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        break;

    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

    case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
#endif
}

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    SPIFFS.begin(true);

    //esp_log_level_set("*", ESP_LOG_DEBUG);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LED_ON);

#ifdef htuSensorTopic
    sensor_setup();
#endif

    connectToWifi();

    mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
    wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);

    mqttClient.setServer(mqttHost.c_str(), mqttPort);
    mqttClient.setWill((AVAILABILITY_TOPIC).c_str(), 0, 1, "DISCONNECTED");
    mqttClient.setKeepAlive(60);

    configureOTA();

    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), true);
    pBLEScan->setActiveScan(BLE_ACTIVE_SCAN);
    pBLEScan->setInterval(BLE_SCAN_INTERVAL);
    pBLEScan->setWindow(BLE_SCAN_WINDOW);

    xTaskCreatePinnedToCore(scanForDevices, "BLE Scan", 4096, pBLEScan, 1, &thBLEScan, 1);

#ifdef M5STICK
    M5.begin();
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
#endif

    setClock();
}

void loop()
{
    TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed = 1;
    TIMERG0.wdt_wprotect = 0;
    ArduinoOTA.handle();

    firmwareUpdate();
}
