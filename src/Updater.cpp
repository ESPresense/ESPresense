#include <Arduino.h>
#include <ArduinoOTA.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <esp_ota_ops.h>

#include "HeadlessWiFiSettings.h"
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
String updateUrl;

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

void checkForUpdates() {
    auto versionMarker = getVersionMarker();
    if (versionMarker.length() > 0) {
        static bool foundNewVersion = false;
        WiFiClientSecure client;
        client.setTimeout(12);
        client.setInsecure();
        {
            auto url = getFirmwareUrl();
            Log.printf("Checking for new firmware version at '%s'\r\n", url.c_str());
            HTTPClient http;
            if (!http.begin(client, url))
                return;
            int httpCode = http.sendRequest("HEAD");
            bool isRedirect = httpCode > 300 && httpCode < 400;
            if (isRedirect) {
                if (http.getLocation().indexOf(versionMarker) < 0) {
                    Log.printf("Found new version: %s\r\n", http.getLocation().c_str());
                    spurt("/update", http.getLocation());
                    foundNewVersion = true;
                }
            } else
                Log.printf("Error on checking for update (sc=%d)\r\n", httpCode);
            http.end();

            if (foundNewVersion) {
                Log.println("Rebooting to start update");
                ESP.restart();
            }
        }
    }
}

void firmwareUpdate() {
    String url = updateUrl.startsWith("http") ? updateUrl : getFirmwareUrl();
    bool isSecure = url.startsWith("https://");

    HttpReleaseUpdate httpUpdate;

    httpUpdate.onStart([url]() {
        autoUpdateAttempts++;
        updateStartedMillis = millis();
        GUI::Update(UPDATE_STARTED);
        HttpWebServer::UpdateStart();
        Log.printf("Starting firmware update from: %s\n", url.c_str());
    });

    httpUpdate.onProgress([](int progress, int total) {
        int percentage = total > 0 ? (progress * 100) / total : 0;
        GUI::Update(percentage);
    });

    httpUpdate.onEnd([](bool success) {
        if (success) {
            SPIFFS.remove("/update");
            Log.println("Firmware update completed successfully!");
        } else {
            Log.println("Firmware update failed to apply");
        }
        updateStartedMillis = 0;
        GUI::Update(UPDATE_COMPLETE);
        HttpWebServer::UpdateEnd();
    });

    HttpUpdateResult ret;

    if (isSecure) {
        WiFiClientSecure secureClient;
        secureClient.setHandshakeTimeout(8);
        secureClient.setInsecure();     // Allow self-signed certificates
        secureClient.setTimeout(12);
        ret = httpUpdate.update(secureClient, url);
    } else {
        WiFiClient insecureClient;
        insecureClient.setTimeout(12);
        ret = httpUpdate.update(insecureClient, url);
    }

    switch (ret) {
        case HTTP_UPDATE_FAILED:
            Log.printf("Http Update Failed (Error=%d): %s\r\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
            break;
        case HTTP_UPDATE_NO_UPDATES:
            Log.printf("No Update!\r\n");
            break;
        case HTTP_UPDATE_OK:
            Log.printf("Update OK!\r\n");
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
            HttpWebServer::UpdateStart();
        })
        .onEnd([]() {
            updateStartedMillis = 0;
            GUI::Update(UPDATE_COMPLETE);
            HttpWebServer::UpdateEnd();
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            GUI::Update((progress / (total / 100)));
        })
        .onError([](ota_error_t error) {
            Log.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR)
                Log.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR)
                Log.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR)
                Log.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR)
                Log.println("Receive Failed");
            else if (error == OTA_END_ERROR)
                Log.println("End Failed");
            updateStartedMillis = 0;
        });
    ArduinoOTA.setHostname(WiFi.getHostname());
    ArduinoOTA.setPort(3232);
    ArduinoOTA.setMdnsEnabled(false);  // We just don't have the memory
    ArduinoOTA.begin();

    Log.println("ArduinoOTA configured and ready");
}

bool setup = false;
void Setup() {
    auto uf = SPIFFS.exists("/update");
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
    autoUpdateEnabled = HeadlessWiFiSettings.checkbox("auto_update", DEFAULT_AUTO_UPDATE, "Automatically update");
    prerelease = HeadlessWiFiSettings.checkbox("prerelease", false, "Include pre-released versions in auto-update");
    arduinoOtaEnabled = HeadlessWiFiSettings.checkbox("arduino_ota", DEFAULT_ARDUINO_OTA, "Arduino OTA Update");
    updateUrl = HeadlessWiFiSettings.string("update", "", "If set will update from this url on next boot");
}

void MarkOtaSuccess() {
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            esp_ota_mark_app_valid_cancel_rollback();
        }
    }
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
        spurt("/update", pay);
        ESP.restart();
    } else
        return false;
    return true;
}
}  // namespace Updater
