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

#define MAX_MAC_ADDRESSES 50
#define CHECK_FOR_UPDATES_MILI 100000

WiFiClient espClient;
AsyncMqttClient mqttClient;
TimerHandle_t reconnectTimer;
bool updateInProgress = false;
String localIp;
byte retryAttempts = 0;
BLEScan *pBLEScan;
TaskHandle_t thBLEScan;

String mqttHost;
int mqttPort;
String mqttUser;
String mqttPass;
String availabilityTopic;
String room;

static std::list<BleFingerprint *> fingerprints;

BleFingerprint *getFingerprint(BLEAdvertisedDevice *advertisedDevice)
{
    auto mac = advertisedDevice->getAddress();

    auto is_even = [mac](BleFingerprint *f) { return f->getAddress() == mac; };

    auto it = std::find_if(fingerprints.begin(), fingerprints.end(), is_even);
    if (it != fingerprints.end())
    {
        return *it;
    }

    if (fingerprints.size() > MAX_MAC_ADDRESSES)
    {
        delete fingerprints.back();
        fingerprints.pop_back();
    }

    auto created = new BleFingerprint(advertisedDevice, MAX_DISTANCE);
    fingerprints.push_front(created);
    return created;
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

    tele["free_heap"] = ESP.getFreeHeap();
    tele["min_free_heap"] = ESP.getMinFreeHeap();
    tele["heap_size"] = ESP.getHeapSize();
    tele["max_alloc_heap"] = ESP.getMaxAllocHeap();
#ifdef VERSION
    tele["ver"] = String(VERSION);
#endif

    if (deviceCount > -1)
        tele["disc_ct"] = deviceCount;

    if (reportCount > -1)
        tele["rept_ct"] = reportCount;

    char teleMessageBuffer[258];
    serializeJson(tele, teleMessageBuffer);

    if (mqttClient.publish((TELEMETRY_TOPIC).c_str(), 0, 0, teleMessageBuffer) == true)
    {
        return true;
    }
    else
    {
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
        if (CalculateUptimeSeconds() > 600)
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
    void onResult(BLEAdvertisedDevice *advertisedDevice)
    {
        digitalWrite(LED_BUILTIN, LED_BUILTIN_ON);
        //Serial.printf("Advertised Device: %s \n", advertisedDevice->toString().c_str());
        BleFingerprint *f = getFingerprint(advertisedDevice);
        f->seen(advertisedDevice);
        vTaskDelay(advertisedDevice->getRSSI() > -60 ? 2 : 1);
        digitalWrite(LED_BUILTIN, !LED_BUILTIN_ON);
    }
};

bool reportDevice(BLEAdvertisedDevice advertisedDevice)
{
    BleFingerprint *f = getFingerprint(&advertisedDevice);

    if (!f->shouldReport())
        return false;

    auto doc = f->report();
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
    int i = 0;
    while (1)
    {
        delay(0);
        if (updateInProgress)
            continue;

        if (mqttClient.connected())
        {
            pBLEScan->setActiveScan(i++ % 10 == 0 ? BLE_ACTIVE_SCAN : false);
            pBLEScan->clearResults();
            BLEScanResults foundDevices = pBLEScan->start(BLE_SCAN_DURATION);
            int devicesCount = foundDevices.getCount();

#ifdef M5STICK
            M5.Lcd.setCursor(0, 0);
            M5.Lcd.fillScreen(TFT_BLACK);

#endif

            int devicesReported = 0;
            for (uint32_t j = 0; j < devicesCount; j++)
            {
                bool included = reportDevice(foundDevices.getDevice(j));
                if (included)
                {
                    devicesReported++;
                }
            }
            sendTelemetry(devicesCount, devicesReported);
        }
        else
        {
            log_e("Cannot report; mqtt disconnected");
            delay(1000);
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
        })
        .onEnd([]() {
            updateInProgress = false;
            digitalWrite(LED_BUILTIN, !LED_BUILTIN_ON);
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
    ArduinoOTA.setHostname(WiFi.getHostname());
    ArduinoOTA.setPort(3232);
    ArduinoOTA.begin();
}

void setClock()
{
    configTime(0, 0, "pool.ntp.org", "time.nist.gov"); // UTC

    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2)
    {
        yield();
        delay(500);
        now = time(nullptr);
    }

    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    log_i(F("NTP synced, current time: %s"), asctime(&timeinfo));
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

    httpUpdate.setLedPin(LED_BUILTIN, LED_BUILTIN_ON);
    httpUpdate.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    String firmwareUrl = Sprintf("https://github.com/DTTerastar/ESP32-mqtt-room/releases/latest/download/%s.bin", FIRMWARE);

    HTTPClient http;
    if (!http.begin(client, firmwareUrl))
        return;

    int httpCode = http.sendRequest("HEAD");
    if (httpCode != 302 || http.getLocation().indexOf(String(VERSION)) > 0)
    {
        http.end();
        return;
    }

    updateInProgress = true;
    Serial.printf("Updating from %s\n", firmwareUrl.c_str());
    mqttClient.disconnect(true);

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

    updateInProgress = false;
#endif
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
