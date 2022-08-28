#include <Arduino.h>
#include <ArduinoOTA.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "AsyncWiFiSettings.h"
#include "GUI.h"
#include "HttpReleaseUpdate.h"
#include "HttpWebServer.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"

namespace Updater {

bool autoUpdateEnabled, prerelease;
bool arduinoOtaEnabled, arduinoOtaConfgured = false;
unsigned long updateStartedMillis = 0;
unsigned long lastFirmwareCheck = 0;
unsigned short autoUpdateAttempts = 0;

String getFirmwareUrl() {
#ifdef FIRMWARE
    if (!prerelease) return "https://github.com/ESPresense/ESPresense/releases/latest/download/" FIRMWARE ".bin";
    #ifdef BRANCH
        return "https://espresense.com/artifacts/latest/download/" BRANCH "/" FIRMWARE ".bin";
    #else
        return "https://espresense.com/releases/latest-any/download/" FIRMWARE ".bin";
    #endif
#else
    return "https://github.com/ESPresense/ESPresense/releases/latest/download/esp32.bin";
#endif
}

String getVersionMarker() {
#ifdef VERSION
    return String("/") + VERSION + String("/");
#else
    return "";
#endif
}

bool hasNewVersion(WiFiClientSecure& client, const String& url, const String& version) {
    static bool foundNewVersion = false;
    if (foundNewVersion) return true;

    Serial.printf("Checking for new firmware version at '%s'\n", url.c_str());
    HTTPClient http;
    if (!http.begin(client, url))
        return false;
    int httpCode = http.sendRequest("HEAD");
    bool isRedirect = httpCode > 300 && httpCode < 400;
    if (isRedirect) {
        if (http.getLocation().indexOf(version) < 0) {
            Serial.printf("Found new version: %s\n", http.getLocation().c_str());
            foundNewVersion = true;
        }
    } else
        Serial.printf("Error on checking for update (sc=%d)\n", httpCode);
    http.end();

    return foundNewVersion;
}

void checkForUpdates() {
    WiFiClientSecure client;
    client.setTimeout(12);
    client.setInsecure();
    if (hasNewVersion(client, getFirmwareUrl(), getVersionMarker())) {
        spurt("/update", "NOW");
        ESP.restart();
    }
}

void firmwareUpdate() {
    WiFiClientSecure client;
    client.setTimeout(12);
    client.setInsecure();

    HttpReleaseUpdate httpUpdate;
    httpUpdate.setTimeout(12000);
    httpUpdate.onStart([](void) {
        autoUpdateAttempts++;
        updateStartedMillis = millis();
        GUI::Update(UPDATE_STARTED);
    });
    httpUpdate.onEnd([](void) {
        if (autoUpdateAttempts > 3) {
            spurt("/update", "NOW");
            ESP.restart();
        }
        updateStartedMillis = 0;
        GUI::Update(UPDATE_COMPLETE);
    });
    httpUpdate.onProgress([](int progress, int total) {
        GUI::Update((progress / (total / 100)));
    });
    auto ret = httpUpdate.update(client, getFirmwareUrl());
    switch (ret) {
        case HTTP_UPDATE_FAILED:
            Serial.printf("Http Update Failed (Error=%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Serial.printf("No Update!\n");
            break;
    }
}

void configureOTA(void) {
    if (arduinoOtaConfgured) return;
    arduinoOtaConfgured = true;

    ArduinoOTA
        .onStart([]() {
            updateStartedMillis = millis();
            GUI::Update(UPDATE_STARTED);
        })
        .onEnd([]() {
            updateStartedMillis = 0;
            GUI::Update(UPDATE_COMPLETE);
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            GUI::Update((progress / (total / 100)));
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
            updateStartedMillis = 0;
        });
    ArduinoOTA.setHostname(WiFi.getHostname());
    ArduinoOTA.setPort(3232);
    ArduinoOTA.setMdnsEnabled(false);  // We just don't have the memory
    ArduinoOTA.begin();

    Serial.println("ArduinoOTA configured and ready");
}

bool setup = false;
void Setup() {
    auto uf = SPIFFS.exists("/update") && SPIFFS.remove("/update");
    if (uf) firmwareUpdate();
}

void Loop() {
    if (autoUpdateEnabled) {
        unsigned long now = millis();
        if (now - lastFirmwareCheck > CHECK_FOR_UPDATES_INTERVAL) {
            lastFirmwareCheck = now;
            checkForUpdates();
        }
    }

    if (arduinoOtaEnabled) {
        configureOTA();
        ArduinoOTA.handle();
    }
}

bool SendOnline() {
    return pub((roomsTopic + "/arduino_ota").c_str(), 0, true, arduinoOtaEnabled ? "ON" : "OFF") && pub((roomsTopic + "/auto_update").c_str(), 0, true, autoUpdateEnabled ? "ON" : "OFF") && pub((roomsTopic + "/prerelease").c_str(), 0, true, prerelease ? "ON" : "OFF");
}

bool SendDiscovery() {
    return sendSwitchDiscovery("Auto Update", EC_CONFIG) && sendSwitchDiscovery("Arduino OTA", EC_CONFIG) && sendSwitchDiscovery("Prerelease", EC_CONFIG) && sendButtonDiscovery("Update", EC_DIAGNOSTIC);
}

void ConnectToWifi() {
    autoUpdateEnabled = AsyncWiFiSettings.checkbox("auto_update", DEFAULT_AUTO_UPDATE, "Automatically update");
    prerelease = AsyncWiFiSettings.checkbox("prerelease", false, "Include pre-released versions in auto-update");
    arduinoOtaEnabled = AsyncWiFiSettings.checkbox("arduino_ota", DEFAULT_ARDUINO_OTA, "Arduino OTA Update");
}

bool Command(String& command, String& pay) {
    if (command == "arduino_ota") {
        arduinoOtaEnabled = pay == "ON";
        spurt("/arduino_ota", String(arduinoOtaEnabled));
    } else if (command == "auto_update") {
        autoUpdateEnabled = pay == "ON";
        spurt("/auto_update", String(autoUpdateEnabled));
    } else if (command == "prerelease") {
        prerelease = pay == "ON";
        spurt("/prerelease", String(prerelease));
    } else if (command == "update") {
        spurt("/update", "NOW");
        ESP.restart();
    } else
        return false;
    return true;
}
}  // namespace Updater
