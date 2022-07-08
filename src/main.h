#include "BleFingerprint.h"
#include "BleFingerprintCollection.h"

#include "globals.h"
#include "mqtt.h"
#include "GUI.h"
#include "defaults.h"
#include "string_utils.h"

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <AsyncTCP.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <NimBLEDevice.h>
#include <SPIFFS.h>
#include <WebServer.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiSettings.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <rom/rtc.h>
#include "Network.h"

#include "MotionSensors.h"
#ifdef SENSORS
#include <Wire.h>

#include "BME280Sensor.h"
#include "TSL2561Sensor.h"
#include "HX711.h"
#include "DHT.h"
#include "BH1750.h"
#endif

TimerHandle_t reconnectTimer;
TaskHandle_t scanTaskHandle, reportTaskHandle;

bool updateInProgress = false;
unsigned long lastTeleMillis;
int reconnectTries = 0;
int teleFails = 0;
bool online = false;        // Have we successfully sent status=online
bool sentDiscovery = false; // Have we successfully sent discovery
String offline = "offline";

int ethernetType = 0;
String mqttHost, mqttUser, mqttPass;
uint16_t mqttPort;

bool autoUpdate, arduinoOta, prerelease;
bool discovery, activeScan, publishTele, publishRooms, publishDevices;

BleFingerprintCollection fingerprints;

String resetReason(RESET_REASON reason)
{
#ifdef ARDUINO_ARCH_ESP32C3
    return "";
#else
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
#endif
}

unsigned long getUptimeSeconds()
{
    return (unsigned long)(esp_timer_get_time() / 1000000ULL);
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
    if (!arduinoOta) return;
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
                   GUI::updateEnd();
                   Serial.println("\n\rEnd");
               })
        .onProgress([](unsigned int progress, unsigned int total)
                    {
                        GUI::updateProgress((progress / (total / 100)));
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
    ArduinoOTA.setMdnsEnabled(false); // We just don't have the memory
    ArduinoOTA.begin();
}

void firmwareUpdate()
{
#ifdef FIRMWARE
    if (!autoUpdate) return;
    static unsigned long lastFirmwareCheck = 0;
    unsigned long uptime = getUptimeSeconds();
    if (uptime - lastFirmwareCheck < CHECK_FOR_UPDATES_INTERVAL)
        return;

    lastFirmwareCheck = uptime;

    HTTPClient http;
    WiFiClientSecure client;
    client.setInsecure();

    String firmwareUrl = prerelease
                             ? "https://espresense.com/releases/latest-any/download/" FIRMWARE ".bin"
                             : "https://github.com/ESPresense/ESPresense/releases/latest/download/" FIRMWARE ".bin";
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
    mqttClient.disconnect();
    NimBLEDevice::getScan()->stop();
    fingerprints.setDisable(updateInProgress);
    GUI::updateStart();
    httpUpdate.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    httpUpdate.onProgress([](int progress, int total)
                            {
                                GUI::updateProgress((progress / (total / 100)));
                            });
    t_httpUpdate_return ret = httpUpdate.update(client, firmwareUrl);
    GUI::updateEnd();

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
#endif
}

void spiffsInit()
{
#ifdef BUTTON
    pinMode(BUTTON, INPUT);
    int flashes = 0;
    unsigned long debounceDelay = 250;

    unsigned long lastDebounceTime = millis();
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

#ifdef MACCHINA_A0

int smoothMilliVolts;
int a0_read_batt_mv()
{
    int mv = round(((float)analogRead(GPIO_NUM_35) + 35) / 0.215);
    if (smoothMilliVolts)
        smoothMilliVolts = round(0.1 * (mv - smoothMilliVolts) + smoothMilliVolts);
    else
        smoothMilliVolts = mv;
    return smoothMilliVolts;
}
#endif
