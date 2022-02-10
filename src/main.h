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

//GY-302 lux sensor
#include <Wire.h>
#include <hp_BH1750.h>
hp_BH1750 BH1750;
long ms_BH1750;
float lux_BH1750;
int lux_BH1750_MQTT;
String BH1750_I2c;
bool I2CDebug;

AsyncMqttClient mqttClient;
TimerHandle_t reconnectTimer;
TaskHandle_t scannerTask;

bool updateInProgress = false;
String localIp;
unsigned long lastTeleMillis;
int reconnectTries = 0;
int teleFails = 0;
bool online = false;        // Have we successfully sent status=online
bool sentDiscovery = false; // Have we successfully sent discovery
String offline = "offline";
String mqttHost;
int mqttPort;
String mqttUser;
String mqttPass;
String room;
String id;
String statusTopic;
String teleTopic;
String roomsTopic;
String subTopic;
bool autoUpdate, otaUpdate;
bool discovery, statusLed;
bool activeScan, allowQuery;
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
int lastPirValue = -1;
int lastRadarValue = -1;

#ifdef SENSORS
int dht11Pin;
int dht22Pin;

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
#endif
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
    Display.updateStart();
    httpUpdate.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    t_httpUpdate_return ret = httpUpdate.update(client, firmwareUrl);
    Display.updateEnd();

    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        Serial.printf("Http Update Failed (Error=%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        break;

    case HTTP_UPDATE_NO_UPDATES:
        Serial.printf("No Update!\n");
        break;

    case HTTP_UPDATE_OK:
        Serial.printf("Update OK!\n");
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

bool pub(const char *topic, uint8_t qos, bool retain, const char *payload, size_t length = 0, bool dup = false, uint16_t message_id = 0)
{
    for (int i = 0; i < 10; i++)
    {
        if (mqttClient.publish(topic, qos, retain, payload, length, dup, message_id))
            return true;
        delay(50);
    }
    return false;
}

bool sendOnline()
{
    return mqttClient.publish(statusTopic.c_str(), 0, 1, "online") && mqttClient.publish((roomsTopic + "/max_distance").c_str(), 0, 1, String(maxDistance).c_str()) && mqttClient.publish((roomsTopic + "/query").c_str(), 0, 1, String(allowQuery ? "ON" : "OFF").c_str()) && mqttClient.publish((roomsTopic + "/status_led").c_str(), 0, 1, String(statusLed ? "ON" : "OFF").c_str()) && mqttClient.publish((roomsTopic + "/active_scan").c_str(), 0, 1, String(activeScan ? "ON" : "OFF").c_str());
}

void commonDiscovery(JsonDocument *doc)
{
    auto identifiers = (*doc)["dev"].createNestedArray("ids");
    identifiers.add(Sprintf("espresense_%06" PRIx64, ESP.getEfuseMac() >> 24));
    auto connections = (*doc)["dev"].createNestedArray("cns");
    auto mac = connections.createNestedArray();
    mac.add("mac");
    mac.add(WiFi.macAddress());
    (*doc)["dev"]["name"] = "ESPresense " + room;
    (*doc)["dev"]["sa"] = room;
#ifdef VERSION
    (*doc)["dev"]["sw"] = VERSION;
#endif
    (*doc)["dev"]["mf"] = "ESPresense (" FIRMWARE ")";
    (*doc)["dev"]["cu"] = "http://" + localIp;
    (*doc)["dev"]["mdl"] = ESP.getChipModel();
}

bool sendDiscoveryConnectivity()
{
    DynamicJsonDocument doc(1200);
    commonDiscovery(&doc);
    doc["~"] = roomsTopic;
    doc["name"] = "ESPresense " + room;
    doc["uniq_id"] = Sprintf("espresense_%06" PRIx64 "_connectivity", ESP.getEfuseMac() >> 24);
    doc["json_attr_t"] = "~/telemetry";
    doc["stat_t"] = "~/status";
    doc["dev_cla"] = "connectivity";
    doc["pl_on"] = "online";
    doc["pl_off"] = "offline";

    char buffer[1200];
    serializeJson(doc, buffer);
    String discoveryTopic = "homeassistant/binary_sensor/espresense_" + ESPMAC + "/connectivity/config";

    return pub(discoveryTopic.c_str(), 0, true, buffer);
}

bool sendDiscoveryUptime()
{
    DynamicJsonDocument doc(1200);
    commonDiscovery(&doc);
    doc["~"] = roomsTopic;
    doc["name"] = "ESPresense " + room + " Uptime";
    doc["uniq_id"] = Sprintf("espresense_%06" PRIx64 "_uptime", ESP.getEfuseMac() >> 24);
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/telemetry";
    doc["entity_category"] = "diagnostic";
    doc["value_template"] = "{{ value_json.uptime }}";
    doc["unit_of_measurement"] = "s";

    char buffer[1200];
    serializeJson(doc, buffer);
    String discoveryTopic = "homeassistant/sensor/espresense_" + ESPMAC + "/uptime/config";

    return pub(discoveryTopic.c_str(), 0, true, buffer);
}

bool sendDiscoveryFreeMem()
{
    DynamicJsonDocument doc(1200);
    commonDiscovery(&doc);
    doc["~"] = roomsTopic;
    doc["name"] = "ESPresense " + room + " Free Memory";
    doc["uniq_id"] = Sprintf("espresense_%06" PRIx64 "_free_mem", ESP.getEfuseMac() >> 24);
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/telemetry";
    doc["entity_category"] = "diagnostic";
    doc["value_template"] = "{{ value_json.maxAllocHeap }}";
    doc["unit_of_measurement"] = "bytes";

    char buffer[1200];
    serializeJson(doc, buffer);
    String discoveryTopic = "homeassistant/sensor/espresense_" + ESPMAC + "/free_mem/config";

    return pub(discoveryTopic.c_str(), 0, true, buffer);
}

bool sendDiscoveryMotion()
{
    if (!pirPin && !radarPin) return true;

    DynamicJsonDocument doc(1200);
    commonDiscovery(&doc);
    doc["~"] = roomsTopic;
    doc["name"] = "ESPresense " + room + " Motion";
    doc["uniq_id"] = Sprintf("espresense_%06" PRIx64 "_motion", ESP.getEfuseMac() >> 24);
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/motion";
    doc["dev_cla"] = "motion";

    char buffer[1200];
    serializeJson(doc, buffer);
    String discoveryTopic = "homeassistant/binary_sensor/espresense_" + ESPMAC + "/motion/config";

    return pub(discoveryTopic.c_str(), 0, true, buffer);
}

#ifdef SENSORS
bool sendDiscoveryTemperature()
{
    if (!dht11Pin && !dht22Pin) return true;

    DynamicJsonDocument doc(1200);
    commonDiscovery(&doc);
    doc["~"] = roomsTopic;
    doc["name"] = "ESPresense " + room + " Temperature";
    doc["uniq_id"] = Sprintf("espresense_%06" PRIx64 "_temperature", ESP.getEfuseMac() >> 24);
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/temperature";
    doc["dev_cla"] = "temperature";
    doc["unit_of_meas"] = "°C";
    doc["frc_upd"] = true;

    char buffer[1200];
    serializeJson(doc, buffer);
    String discoveryTopic = "homeassistant/sensor/espresense_" + ESPMAC + "/temperature/config";
    return pub(discoveryTopic.c_str(), 0, true, buffer);
}

bool sendDiscoveryHumidity()
{
    if (!dht11Pin && !dht22Pin) return true;

    DynamicJsonDocument doc(1200);
    commonDiscovery(&doc);
    doc["~"] = roomsTopic;
    doc["name"] = "ESPresense " + room + " Humidity";
    doc["uniq_id"] = Sprintf("espresense_%06" PRIx64 "_humidity", ESP.getEfuseMac() >> 24);
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/humidity";
    doc["dev_cla"] = "humidity";
    doc["frc_upd"] = true;

    char buffer[1200];
    serializeJson(doc, buffer);
    String discoveryTopic = "homeassistant/sensor/espresense_" + ESPMAC + "/humidity/config";
    return pub(discoveryTopic.c_str(), 0, true, buffer);
}

bool sendDiscoveryLux()
{
    if (BH1750_I2c.isEmpty()) return true;

    DynamicJsonDocument doc(1200);
    commonDiscovery(&doc);
    doc["~"] = roomsTopic;
    doc["name"] = "ESPresense " + room + " Lux";
    doc["uniq_id"] = Sprintf("espresense_%06" PRIx64 "_lux", ESP.getEfuseMac() >> 24);
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/lux";
    doc["dev_cla"] = "illuminance";
    doc["unit_of_meas"] = "lx";
    doc["frc_upd"] = true;

    char buffer[1200];
    serializeJson(doc, buffer);
    String discoveryTopic = "homeassistant/sensor/espresense_" + ESPMAC + "/lux/config";
    return pub(discoveryTopic.c_str(), 0, true, buffer);
}
#endif

bool sendSwitchDiscovery(String name, String entityCategory)
{
    auto slug = slugify(name);

    DynamicJsonDocument doc(1200);
    commonDiscovery(&doc);
    doc["~"] = roomsTopic;
    doc["name"] = Sprintf("ESPresense %s %s", room.c_str(), name.c_str());
    doc["uniq_id"] = Sprintf("espresense_%06" PRIx64 "_%s", ESP.getEfuseMac() >> 24, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/" + slug;
    doc["cmd_t"] = "~/" + slug + "/set";
    doc["entity_category"] = entityCategory;

    char buffer[1200];
    serializeJson(doc, buffer);
    String discoveryTopic = "homeassistant/switch/espresense_" + ESPMAC + "/" + slug + "/config";
    return pub(discoveryTopic.c_str(), 0, true, buffer);
}

bool sendNumberDiscovery(String name, String entityCategory)
{
    auto slug = slugify(name);

    DynamicJsonDocument doc(1200);
    commonDiscovery(&doc);
    doc["~"] = roomsTopic;
    doc["name"] = Sprintf("ESPresense %s %s", room.c_str(), name.c_str());
    doc["uniq_id"] = Sprintf("espresense_%06" PRIx64 "_%s", ESP.getEfuseMac() >> 24, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/" + slug;
    doc["cmd_t"] = "~/" + slug + "/set";
    doc["step"] = "0.01";
    doc["entity_category"] = entityCategory;

    char buffer[1200];
    serializeJson(doc, buffer);
    String discoveryTopic = "homeassistant/number/espresense_" + ESPMAC + "/" + slug + "/config";
    return pub(discoveryTopic.c_str(), 0, true, buffer);
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
