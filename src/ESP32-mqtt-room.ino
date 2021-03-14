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
#include <WiFiClientSecure.h>

#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include "NimBLEEddystoneURL.h"
#include "NimBLEEddystoneTLM.h"
#include "NimBLEBeacon.h"

#include "Settings.h"

#include <TrivialKalmanFilter.h>

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

static const int scanTime = singleScanTime;
static const uint16_t beaconUUID = 0xFEAA;
static const uint16_t tileUUID = 0xFEED;
static const uint16_t exposureUUID = 0xFD6F;
#ifdef TxDefault
static const int defaultTxPower = TxDefault;
#else
static const int defaultTxPower = -59;
#endif
#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))

#define MAX_MAC_ADDRESSES 50
#define DT_COVARIANCE_RK 2
#define DT_COVARIANCE_QK 0.1

WiFiClient espClient;
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;
bool updateInProgress = false;
String localIp;
byte retryAttempts = 0;
unsigned long last = 0;
BLEScan *pBLEScan;
TaskHandle_t BLEScan;
TrivialKalmanFilter<float> *filters[MAX_MAC_ADDRESSES];

int mac_pos(const uint8_t mac[6])
{
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

String getProximityUUIDString(BLEBeacon beacon)
{
    std::string serviceData = beacon.getProximityUUID().toString().c_str();
    int serviceDataLength = serviceData.length();
    String returnedString = "";
    int i = serviceDataLength;
    while (i > 0)
    {
        if (serviceData[i - 1] == '-')
        {
            i--;
        }
        char a = serviceData[i - 1];
        char b = serviceData[i - 2];
        returnedString += b;
        returnedString += a;

        i -= 2;
    }

    return returnedString;
}

void initFilters()
{
    for (int i = 0; i < MAX_MAC_ADDRESSES; i++)
        filters[i] = new TrivialKalmanFilter<float>(DT_COVARIANCE_RK, DT_COVARIANCE_QK);
}

void calcDistance(int pos, int rssi, int calRssi, StaticJsonDocument<500> *doc)
{
    if (!calRssi)
        calRssi = defaultTxPower;

    float ratio = (calRssi - rssi) / 25.0f;
    float distFl = pow(10, ratio);
    float filtered = filters[pos]->update(distFl);
    float distance = round(filtered * 10) / 10;
    float original = round(distFl * 10) / 10;

    Serial.printf(", RSSI: %d (@1m %d)", rssi, calRssi);
    Serial.printf(", Dist: %.1f (orig %.1f)", distance, original);

    (*doc)["rssi@1m"] = calRssi;
    (*doc)["rssi"] = rssi;
    (*doc)["distance"] = distance;
    (*doc)["original"] = original;
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
    tele["scan_dur"] = scanTime;
    tele["max_dist"] = maxDistance;
    tele["uptime"] = CalculateUptimeSeconds();

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

    if (mqttClient.publish(telemetryTopic, 0, 0, teleMessageBuffer) == true)
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
    WiFi.begin(ssid, password);
    WiFi.setHostname(hostname);
}

void connectToMqtt()
{
    Serial.println("Connecting to MQTT");
    if (WiFi.isConnected() && !updateInProgress)
    {
        mqttClient.setCredentials(mqttUser, mqttPassword);
        mqttClient.setClientId(hostname);
        mqttClient.connect();
    }
    else
    {
        Serial.println("Cannot reconnect MQTT - WiFi error");
        handleWifiDisconnect();
    }
}

bool handleMqttDisconnect()
{
    if (updateInProgress)
    {
        Serial.println("Not retrying MQTT connection - OTA update in progress");
        return true;
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
    if (WiFi.isConnected() && !updateInProgress)
    {
        Serial.println("Starting MQTT reconnect timer");
        if (xTimerReset(mqttReconnectTimer, 0) == pdFAIL)
        {
            Serial.println("failed to restart");
            xTimerStart(mqttReconnectTimer, 0);
        }
        else
        {
            Serial.println("restarted");
        }
    }
    else
    {
        Serial.print("Disconnected from WiFi; starting WiFi reconnect timiler\t");
        handleWifiDisconnect();
    }
    return false;
}

bool handleWifiDisconnect()
{
    if (WiFi.isConnected())
    {
        Serial.println("WiFi appears to be connected. Not retrying.");
        return true;
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
    if (mqttClient.connected())
    {
        mqttClient.disconnect();
    }
    if (xTimerIsTimerActive(mqttReconnectTimer) != pdFALSE)
    {
        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    }

    if (xTimerReset(wifiReconnectTimer, 0) == pdFAIL)
    {
        Serial.println("failed to restart");
        xTimerStart(wifiReconnectTimer, 0);
        return false;
    }
    else
    {
        Serial.println("restarted");
        return true;
    }
}

void WiFiEvent(WiFiEvent_t event)
{
    Serial.printf("[WiFi-event] event: %x\n\r", event);

    switch (event)
    {
    case SYSTEM_EVENT_STA_GOT_IP:
        digitalWrite(LED_BUILTIN, !LED_ON);
        Serial.print("IP address: \t");
        Serial.println(WiFi.localIP());
        localIp = WiFi.localIP().toString().c_str();
        Serial.print("Hostname: \t");
        Serial.println(WiFi.getHostname());
        connectToMqtt();
        if (xTimerIsTimerActive(wifiReconnectTimer) != pdFALSE)
        {
            Serial.println("Stopping wifi reconnect timer");
            xTimerStop(wifiReconnectTimer, 0);
        }
        retryAttempts = 0;
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        digitalWrite(LED_BUILTIN, LED_ON);
        Serial.println("WiFi lost connection, resetting timer\t");
        handleWifiDisconnect();
        break;
    case SYSTEM_EVENT_WIFI_READY:
        Serial.println("Wifi Ready");
        handleWifiDisconnect();
        break;
    case SYSTEM_EVENT_STA_START:
        Serial.println("STA Start");
        tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, hostname);
        if (xTimerIsTimerActive(wifiReconnectTimer) != pdFALSE)
        {
            TickType_t xRemainingTime = xTimerGetExpiryTime(wifiReconnectTimer) - xTaskGetTickCount();
            Serial.print("WiFi Time remaining: ");
            Serial.println(xRemainingTime);
        }
        else
        {
            Serial.println("WiFi Timer is inactive; resetting");
            handleWifiDisconnect();
        }
        break;
    case SYSTEM_EVENT_STA_STOP:
        Serial.println("STA Stop");
        handleWifiDisconnect();
        break;
    default:
        break;
    }
}

void onMqttConnect(bool sessionPresent)
{
    Serial.println("Connected to MQTT");
    retryAttempts = 0;

    if (mqttClient.publish(availabilityTopic, 0, 1, "CONNECTED") == true)
    {
        Serial.print("Success sending message to topic:\t");
        Serial.println(availabilityTopic);
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

bool reportDevice(BLEAdvertisedDevice advertisedDevice)
{
    StaticJsonDocument<500> doc;

    String mac_address = advertisedDevice.getAddress().toString().c_str();
    mac_address.replace(":", "");
    mac_address.toLowerCase();

    //Check scanned MAC Address against a list of allowed MAC Addresses
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
    // --------------

    int pos = mac_pos(advertisedDevice.getAddress().getNative());
    //Serial.printf("%d ", pos);

    Serial.print("MAC: ");
    Serial.print(mac_address);
    int rssi = advertisedDevice.getRSSI();

    if (advertisedDevice.haveName())
    {
        String nameBLE = String(advertisedDevice.getName().c_str());
        Serial.print(", Name: ");
        Serial.print(nameBLE);
        doc["name"] = nameBLE;
    }

    doc["id"] = mac_address;
    doc["mac"] = mac_address;

    std::string strServiceData = advertisedDevice.getServiceData();
    uint8_t cServiceData[100];
    strServiceData.copy((char *)cServiceData, strServiceData.length(), 0);

    if (advertisedDevice.haveServiceUUID())
    {
        for (int i = 0; i < advertisedDevice.getServiceUUIDCount(); i++)
        {
            std::string sid = advertisedDevice.getServiceUUID(i).toString();
            Serial.printf(", sID: %s", sid.c_str());
            doc["sid" + String(i)] = sid;
        }
    }

    if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceDataUUID().equals(BLEUUID(tileUUID)) == true)
    {
        Serial.print(", Tile");
        doc["name"] = "Tile";
        calcDistance(pos, rssi, advertisedDevice.haveTXPower() ? (-advertisedDevice.getTXPower()) - 41 : 0, &doc);
    }
    else if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceDataUUID().equals(BLEUUID(exposureUUID)) == true)
    { // found covid exposure tracker
        Serial.print(", Exposure");
        doc["id"] = "exp:" + String(strServiceData.length());
        calcDistance(pos, rssi, advertisedDevice.haveTXPower() ? (-advertisedDevice.getTXPower()) - 41 : 0, &doc);
    }
    else if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceDataUUID().equals(BLEUUID(beaconUUID)) == true)
    { // found Eddystone UUID
        Serial.print(", Eddystone");
        // Update distance v ariable for Eddystone BLE devices
        if (cServiceData[0] == 0x10)
        {
            BLEEddystoneURL oBeacon = BLEEddystoneURL();
            oBeacon.setData(strServiceData);
            // Serial.printf("Eddystone Frame Type (Eddystone-URL) ");
            // Serial.printf(oBeacon.getDecodedURL().c_str());
            doc["url"] = oBeacon.getDecodedURL().c_str();
            Serial.print(" URL: ");
            Serial.print(oBeacon.getDecodedURL().c_str());
            calcDistance(pos, rssi, oBeacon.getPower(), &doc);
        }
        else if (cServiceData[0] == 0x20)
        {
            BLEEddystoneTLM oBeacon = BLEEddystoneTLM();
            oBeacon.setData(strServiceData);
            Serial.printf(" TLM: ");
            Serial.printf(oBeacon.toString().c_str());
        }
    }
    else
    {
        if (advertisedDevice.haveManufacturerData() == true)
        {
            std::string strManufacturerData = advertisedDevice.getManufacturerData();

            uint8_t cManufacturerData[100];
            strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);
            char *mdHex = NimBLEUtils::buildHexData(nullptr, (uint8_t *)strManufacturerData.data(), strManufacturerData.length());

            if (strManufacturerData.length() > 2 && cManufacturerData[0] == 0x4C && cManufacturerData[1] == 0x00) // Apple
            {
                if (strManufacturerData.length() == 25 && cManufacturerData[2] == 0x02 && cManufacturerData[3] == 0x15)
                {
                    BLEBeacon oBeacon = BLEBeacon();
                    oBeacon.setData(strManufacturerData);

                    String proximityUUID = getProximityUUIDString(oBeacon);

                    int major = ENDIAN_CHANGE_U16(oBeacon.getMajor());
                    int minor = ENDIAN_CHANGE_U16(oBeacon.getMinor());

                    Serial.print(", iBeacon: ");
                    Serial.print(proximityUUID);
                    Serial.printf("-%d-%d", major, minor);

                    doc["major"] = major;
                    doc["minor"] = minor;

                    doc["id"] = proximityUUID + "-" + String(major) + "-" + String(minor);
                    calcDistance(pos, rssi, oBeacon.getSignalPower(), &doc);
                }
                else
                {
                    String fingerprint = "apple:" + String(mdHex).substring(4, 8) + ":" + String(strManufacturerData.length());
                    if (advertisedDevice.haveTXPower())
                        fingerprint = fingerprint + String(-advertisedDevice.getTXPower());

                    doc["id"] = fingerprint;

                    Serial.printf(", Fingerprint: %s", fingerprint.c_str());
                    calcDistance(pos, rssi, advertisedDevice.haveTXPower() ? (-advertisedDevice.getTXPower()) - 41 : 0, &doc);
                }
            }
            else
            {
                if (advertisedDevice.haveTXPower())
                    doc["txPower"] = -advertisedDevice.getTXPower();

                Serial.print(", MD: ");
                for (int x = 0; x < strManufacturerData.length(); x++)
                    Serial.print(strManufacturerData[x], HEX);

                calcDistance(pos, rssi, advertisedDevice.haveTXPower() ? (-advertisedDevice.getTXPower()) - 41 : 0, &doc);
            }
            free(mdHex);
        }
        else
        {
            calcDistance(pos, rssi, advertisedDevice.haveTXPower() ? (-advertisedDevice.getTXPower()) - 41 : 0, &doc);
        }
    }

    Serial.println();

    char JSONmessageBuffer[512];
    serializeJson(doc, JSONmessageBuffer);
    String id = doc["id"];
    String publishTopic = String(channel) + "/" + room;
    String publishTopic2 = (String(channel) + "/" + room + "/") + id;

    if (mqttClient.connected())
    {
        if (maxDistance == 0 || doc["distance"] < maxDistance)
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

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice *advertisedDevice)
    {
        digitalWrite(LED_BUILTIN, LED_ON);
        //Serial.printf("Advertised Device: %s \n", advertisedDevice->toString().c_str());
        vTaskDelay(advertisedDevice->getRSSI() > -60 ? 2 : 1);
        digitalWrite(LED_BUILTIN, !LED_ON);
    }
};

void scanForDevices(void *parameter)
{
    int i = 0;
    while (1)
    {
        i++;
        if (!updateInProgress)
        {
            pBLEScan->setActiveScan(i % 4 == 0);
            if (i % 4 == 0)
                Serial.print("Scanning (ACTIVE)...\t");
            else
                Serial.print("Scanning...\t");
            BLEScanResults foundDevices = pBLEScan->start(scanTime);
            int devicesCount = foundDevices.getCount();
            Serial.printf("Scan done! Devices found: %d\n\r", devicesCount);

#ifdef M5STICK
            M5.Lcd.fillScreen(TFT_BLACK);
            M5.Lcd.setTextDatum(MC_DATUM);
            M5.Lcd.drawNumber(devicesCount, 40, 80, 7);
            M5.Lcd.setTextDatum(MC_DATUM);
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
    ArduinoOTA.setHostname(hostname);
    ArduinoOTA.setPort(3232);
    ArduinoOTA.begin();
}

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LED_ON);

    mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
    wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

#ifdef htuSensorTopic
    sensor_setup();
#endif

    WiFi.onEvent(WiFiEvent);

    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);

    mqttClient.setServer(mqttHost, mqttPort);
    mqttClient.setWill(availabilityTopic, 0, 1, "DISCONNECTED");
    mqttClient.setKeepAlive(60);

    connectToWifi();

    configureOTA();

    initFilters();

    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), true);
    pBLEScan->setActiveScan(activeScan);
    pBLEScan->setInterval(bleScanInterval);
    pBLEScan->setWindow(bleScanWindow);

    xTaskCreatePinnedToCore(
        scanForDevices,
        "BLE Scan",
        4096,
        pBLEScan,
        1,
        &BLEScan,
        1);

#ifdef M5STICK
    M5.begin();
    //M5.Lcd.setRotation(1);
    M5.Lcd.fillScreen(TFT_BLACK);
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
    static long lastFirmwareCheck;
    if (millis() - lastFirmwareCheck < 100000 || WiFi.status() != WL_CONNECTED)
        return;

    lastFirmwareCheck = millis();

    WiFiClientSecure client;
    client.setInsecure();
    httpUpdate.setLedPin(LED_BUILTIN, LOW);
    httpUpdate.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);

    t_httpUpdate_return ret = httpUpdate.update(client, "https://github.com/DTTerastar/ESP32-mqtt-room/releases/latest/download/firmware.bin");

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
}
