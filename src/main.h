#include "BleFingerprint.h"
#include "BleFingerprintCollection.h"

#include "GUI.h"
#include "defaults.h"
#include "strings.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <AsyncMqttClient.h>
#include <AsyncTCP.h>
#include <DHTesp.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <NimBLEDevice.h>
#include <SPIFFS.h>
#include <Ticker.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiSettings.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <rom/rtc.h>

AsyncMqttClient mqttClient;
TimerHandle_t reconnectTimer;
TaskHandle_t scannerTask;

bool updateInProgress = false;
String localIp;
unsigned long lastTeleMillis;
int reconnectTries = 0;
int teleFails = 0;
bool online; // Have we successfully sent status=online

String mqttHost;
int mqttPort;
String mqttUser;
String mqttPass;
String room;
String statusTopic;
String teleTopic;
String roomsTopic;
String subTopic;
bool autoUpdate, otaUpdate;
bool discovery;
bool activeScan;
bool publishTele;
bool publishRooms;
bool publishDevices;
float maxDistance;
float skipDistance;
int refRssi;
int forgetMs;
int skipMs;
int pirPin;
int radarPin;
int dht11Pin;
int dht22Pin;

int lastPirValue = -1;
int lastRadarValue = -1;

/** Initialize DHT sensor 1 */
DHTesp dhtSensor;

/** Task handle for the light value read task */
TaskHandle_t dhtTempTaskHandle = NULL;

/** Ticker for temperature reading */
Ticker tempTicker;

/** Flags for temperature readings finished */
bool gotNewTemperature = false;

/** Data from dht sensor 1 */
TempAndHumidity dhtSensorData;

/* Flag if main loop is running */
bool dhtTasksEnabled = false;

/* update time */
int dhtUpdateTime = 10; //ToDo: maybe make this a user choise via settings menu

BleFingerprintCollection fingerprints;

String resetReason(RESET_REASON reason)
{
    switch (reason)
    {
    case POWERON_RESET:
        return "PowerOn"; /**<1, Vbat power on reset*/
    case SW_RESET:
        return "Sofware"; /**<3, Software reset digital core*/
    case OWDT_RESET:
        return "LegacyWdt"; /**<4, Legacy watch dog reset digital core*/
    case DEEPSLEEP_RESET:
        return "DeepSleep"; /**<5, Deep Sleep reset digital core*/
    case SDIO_RESET:
        return "Sdio"; /**<6, Reset by SLC module, reset digital core*/
    case TG0WDT_SYS_RESET:
        return "Tg0WdtSys"; /**<7, Timer Group0 Watch dog reset digital core*/
    case TG1WDT_SYS_RESET:
        return "Tg1WdtSys"; /**<8, Timer Group1 Watch dog reset digital core*/
    case RTCWDT_SYS_RESET:
        return "RtcWdtSys"; /**<9, RTC Watch dog Reset digital core*/
    case INTRUSION_RESET:
        return "Intrusion"; /**<10, Instrusion tested to reset CPU*/
    case TGWDT_CPU_RESET:
        return "TgWdtCpu"; /**<11, Time Group reset CPU*/
    case SW_CPU_RESET:
        return "SoftwareCpu"; /**<12, Software reset CPU*/
    case RTCWDT_CPU_RESET:
        return "RtcWdtCpu"; /**<13, RTC Watch dog Reset CPU*/
    case EXT_CPU_RESET:
        return "ExtCpu"; /**<14, for APP CPU, reseted by PRO CPU*/
    case RTCWDT_BROWN_OUT_RESET:
        return "RtcWdtBrownOut"; /**<15, Reset when the vdd voltage is not stable*/
    case RTCWDT_RTC_RESET:
        return "RtcWdtRtc"; /**<16, RTC Watch dog reset digital core and rtc module*/
    default:
        return "Unknown";
    }
}

unsigned long getUptimeSeconds(void)
{
    return esp_timer_get_time() / 1e6;
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
    log_i("NTP synced, current time: %s", asctime(&timeinfo));
}

void configureOTA()
{
    if (!otaUpdate) return;
    ArduinoOTA
        .onStart([]()
                 {
                     Serial.println("OTA Start");
                     updateInProgress = true;
                     fingerprints.setDisable(updateInProgress);
                 })
        .onEnd([]()
               {
                   updateInProgress = false;
                   fingerprints.setDisable(updateInProgress);
                   Display.updateEnd();
                   Serial.println("\n\rEnd");
               })
        .onProgress([](unsigned int progress, unsigned int total)
                    {
                        byte percent = (progress / (total / 100));
                        Serial.printf("Progress: %u\r\n", percent);
                        Display.updateProgress(progress);
                    })
        .onError([](ota_error_t error)
                 {
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
                     updateInProgress = false;
                 });
    ArduinoOTA.setHostname(WiFi.getHostname());
    ArduinoOTA.setPort(3232);
    ArduinoOTA.begin();
}

void firmwareUpdate()
{
    if (!autoUpdate) return;
    static long lastFirmwareCheck = 0;
    long uptime = getUptimeSeconds();
    if (uptime - lastFirmwareCheck < CHECK_FOR_UPDATES_INTERVAL)
        return;

    lastFirmwareCheck = uptime;

    HTTPClient http;
    WiFiClientSecure client;
    client.setInsecure();

    String firmwareUrl = Sprintf("https://github.com/ESPresense/ESPresense/releases/latest/download/%s.bin", FIRMWARE);
    if (!http.begin(client, firmwareUrl))
        return;

#ifdef VERSION
    int httpCode = http.sendRequest("HEAD");
    if (httpCode < 300 || httpCode > 400 || http.getLocation().indexOf(String(VERSION)) > 0)
    {
        Serial.printf("Not updating from (sc=%d): %s\n", httpCode, http.getLocation().c_str());
        http.end();
        return;
    }
    else
    {
        Serial.printf("Updating from (sc=%d): %s\n", httpCode, http.getLocation().c_str());
    }
#endif

    updateInProgress = true;
    fingerprints.setDisable(updateInProgress);
#ifdef LED_BUILTIN
    httpUpdate.setLedPin(LED_BUILTIN, LED_BUILTIN_ON);
#endif
    httpUpdate.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    t_httpUpdate_return ret = httpUpdate.update(client, firmwareUrl);

    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        log_e("Http Update Failed (Error=%d): %s", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        break;

    case HTTP_UPDATE_NO_UPDATES:
        log_i("No Update!");
        break;

    case HTTP_UPDATE_OK:
        log_w("Update OK!");
        break;
    }

    updateInProgress = false;
    fingerprints.setDisable(updateInProgress);
}

void spiffsInit()
{
#ifdef BUTTON
    pinMode(BUTTON, INPUT);
    int flashes = 0;
    unsigned long debounceDelay = 250;

    long lastDebounceTime = millis();
    while (digitalRead(BUTTON) == BUTTON_PRESSED)
    {
        if ((millis() - lastDebounceTime) > debounceDelay)
        {
            Display.connecting();
            lastDebounceTime = millis();
            flashes++;

            if (flashes > 10)
            {

                Display.erasing();
                SPIFFS.format();
                SPIFFS.begin(true);
                Display.erased();

                return;
            }
        }
    }

#endif

    SPIFFS.begin(true);
}

bool sendOnline()
{
    return mqttClient.publish(statusTopic.c_str(), 0, 1, "online") && mqttClient.publish((roomsTopic + "/max_distance").c_str(), 0, 1, String(maxDistance).c_str());
}

void commonDiscovery(JsonDocument *doc)
{
    auto identifiers = (*doc)["dev"].createNestedArray("ids");
    identifiers.add(WiFi.macAddress());
    auto connections = (*doc)["dev"].createNestedArray("cns");
    auto mac = connections.createNestedArray();
    mac.add("MAC");
    mac.add(WiFi.macAddress());
    (*doc)["dev"]["name"] = "ESPresense " + room;
    (*doc)["dev"]["sa"] = room;
    (*doc)["dev"]["mdl"] = ESP.getChipModel();
}

bool sendDiscoveryConnectivity()
{
    if (!discovery) return true;
    String discoveryTopic = "homeassistant/binary_sensor/espresense_" + room + "/connectivity/config";

    DynamicJsonDocument doc(1200);
    char buffer[1200];

    doc["~"] = roomsTopic;
    doc["name"] = "ESPresense " + room;
    doc["unique_id"] = WiFi.macAddress() + "_connectivity";
    doc["json_attr_t"] = "~/telemetry";
    doc["stat_t"] = "~/status";
    doc["frc_upd"] = true;
    doc["dev_cla"] = "connectivity";
    doc["pl_on"] = "online";
    doc["pl_off"] = "offline";

    commonDiscovery(&doc);
    serializeJson(doc, buffer);

    for (int i = 0; i < 10; i++)
    {
        if (mqttClient.publish(discoveryTopic.c_str(), 0, true, buffer))
            return true;
        delay(50);
    }

    return false;
}

bool sendDiscoveryMotion()
{
    if (!discovery) return true;
    if (!pirPin && !radarPin) return true;

    String discoveryTopic = "homeassistant/binary_sensor/espresense_" + room + "/motion/config";

    DynamicJsonDocument doc(1200);
    char buffer[1200];

    doc["~"] = roomsTopic;
    doc["name"] = "ESPresense " + room + " Motion";
    doc["unique_id"] = WiFi.macAddress() + "_motion";
    doc["availability_topic"] = "~/status";
    doc["stat_t"] = "~/motion";
    doc["dev_cla"] = "motion";

    commonDiscovery(&doc);
    serializeJson(doc, buffer);

    for (int i = 0; i < 10; i++)
    {
        if (mqttClient.publish(discoveryTopic.c_str(), 0, true, buffer))
            return true;
        delay(50);
    }
    return false;
}

bool sendDiscoveryTemperature()
{
    if (!discovery) return true;
    if (!dht11Pin && !dht22Pin) return true;

    String discoveryTopic = "homeassistant/sensor/espresense_" + room + "/temperature/config";

    DynamicJsonDocument doc(1200);
    char buffer[1200];

    doc["~"] = roomsTopic;
    doc["name"] = "ESPresense " + room + " Temperature";
    doc["unique_id"] = WiFi.macAddress() + "_temperature";
    doc["availability_topic"] = "~/status";
    doc["stat_t"] = "~/temperature";
    doc["dev_cla"] = "temperature";
    doc["unit_of_measurement"] = "C°";

    commonDiscovery(&doc);
    serializeJson(doc, buffer);

    for (int i = 0; i < 10; i++)
    {
        if (mqttClient.publish(discoveryTopic.c_str(), 0, true, buffer))
            return true;
        delay(50);
    }
    return false;
}

bool sendDiscoveryHumidity()
{
    if (!discovery) return true;
    if (!dht11Pin && !dht22Pin) return true;

    String discoveryTopic = "homeassistant/sensor/espresense_" + room + "/humidity/config";

    DynamicJsonDocument doc(1200);
    char buffer[1200];

    doc["~"] = roomsTopic;
    doc["name"] = "ESPresense " + room + " Humidity";
    doc["unique_id"] = WiFi.macAddress() + "_humidity";
    doc["availability_topic"] = "~/status";
    doc["stat_t"] = "~/humidity";
    doc["dev_cla"] = "humidity";

    commonDiscovery(&doc);
    serializeJson(doc, buffer);

    for (int i = 0; i < 10; i++)
    {
        if (mqttClient.publish(discoveryTopic.c_str(), 0, true, buffer))
            return true;
        delay(50);
    }
    return false;
}

bool sendDiscoveryMaxDistance()
{
    if (!discovery) return true;
    String discoveryTopic = "homeassistant/number/espresense_" + room + "/max_distance/config";

    DynamicJsonDocument doc(1200);
    char buffer[1200];

    doc["~"] = roomsTopic;
    doc["name"] = "ESPresense " + room + " Max Distance";
    doc["unique_id"] = WiFi.macAddress() + "_max_distance";
    doc["availability_topic"] = "~/status";
    doc["stat_t"] = "~/max_distance";
    doc["cmd_t"] = "~/max_distance/set";
    doc["step"] = "0.01";

    commonDiscovery(&doc);
    serializeJson(doc, buffer);

    for (int i = 0; i < 10; i++)
    {
        if (mqttClient.publish(discoveryTopic.c_str(), 0, true, buffer))
            return true;
        delay(50);
    }

    return false;
}

bool spurt(const String &fn, const String &content)
{
    File f = SPIFFS.open(fn, "w");
    if (!f) return false;
    auto w = f.print(content);
    f.close();
    return w == content.length();
}

#ifdef MACCHINA_A0
int a0_read_batt_mv()
{
    float vout = ((float)analogRead(GPIO_NUM_35) + 35) / 215.0;
    return vout * 1100; // V to mV with +10% correction
}
#endif
