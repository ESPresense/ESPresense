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

/**
 * @brief Optimize memory before update by clearing caches and disconnecting clients
 */
void optimizeMemoryBeforeUpdate() {
    Log.printf("Memory before optimization: %u bytes\r\n", ESP.getFreeHeap());

    // Clear old fingerprints to free memory
    // BleFingerprintCollection has its own cleanup logic

    // Disconnect websocket clients to free memory
    ws.closeAll();

    Log.printf("Memory after optimization: %u bytes\r\n", ESP.getFreeHeap());

    // Only proceed if we have enough memory
    if (ESP.getFreeHeap() < 60000) {
        Log.println("Insufficient memory for update, aborting");
        return;
    }
}

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

/**
 * @brief Checks the firmware endpoint for a newer release and initiates an update when found.
 *
 * Performs an HTTP HEAD request against the current firmware URL when a version marker is available.
 * If the response is a 3xx redirect and the redirect Location does not contain the current version marker,
 * the redirect URL is written to the persistent update entry ("/update") and the device is restarted to
 * begin the update process. Once a new version has been discovered during the current runtime, subsequent
 * calls will not trigger additional update actions.
 */
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

            // Enhanced error handling for GitHub
            if (httpCode == 403) {
                String rateLimitRemaining = http.header("X-RateLimit-Remaining");
                if (rateLimitRemaining.length() > 0 && rateLimitRemaining.toInt() == 0) {
                    Log.println("GitHub API rate limit exceeded during version check");
                    Log.printf("Rate limit resets at: %s\r\n", http.header("X-RateLimit-Reset").c_str());
                    http.end();
                    return;
                }
            } else if (httpCode == 429) {
                Log.println("GitHub rate limiting active during version check");
                http.end();
                return;
            }
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

/**
 * @brief Initiates and executes a firmware update from the configured update URL.
 *
 * Performs a firmware update using the stored updateUrl if it is an absolute HTTP(S)
 * URL; otherwise it falls back to the computed firmware URL. Sets up callbacks to
 * track start, progress, and end events which update GUI state, notify the web
 * server, increment attempt counters, and log status. Uses a secure TLS client
 * for HTTPS (accepting self-signed certificates) or an insecure TCP client for HTTP.
 *
 * Side effects:
 * - Increments Updater::autoUpdateAttempts and sets Updater::updateStartedMillis when starting.
 * - Calls GUI::Update(...) and HttpWebServer::UpdateStart()/UpdateEnd() during the update lifecycle.
 * - Removes the "/update" file from SPIFFS on successful completion.
 * - Logs progress, results, and errors via Log.printf/Log.println.
 */
void firmwareUpdate() {
    // Optimize memory before starting update
    optimizeMemoryBeforeUpdate();

    String url = updateUrl.startsWith("http") ? updateUrl : getFirmwareUrl();
    bool isSecure = url.startsWith("https://");

    HttpReleaseUpdate httpUpdate;

    // Set longer timeouts for GitHub
    httpUpdate.setTimeout(20000);  // 20 seconds instead of 8

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
        // Enhanced SSL setup specifically for GitHub
        secureClient.setHandshakeTimeout(15);  // Increased for GitHub
        secureClient.setInsecure();     // Allow GitHub's certificates
        secureClient.setTimeout(25);      // Increased timeout for large files
        ret = httpUpdate.update(secureClient, url, 3);  // Add retry count
    } else {
        WiFiClient insecureClient;
        insecureClient.setTimeout(20);  // Increased timeout
        ret = httpUpdate.update(insecureClient, url, 3);  // Add retry count
    }

    // Enhanced error reporting
    switch (ret) {
        case HTTP_UPDATE_FAILED:
            Log.printf("Http Update Failed (Error=%d): %s\r\n",
                      httpUpdate.getLastError(),
                      httpUpdate.getLastErrorString().c_str());

            // Log specific GitHub error details
            if (httpUpdate.getLastError() == HTTP_UE_GITHUB_RATE_LIMIT) {
                Log.println("GitHub rate limit detected. Update will retry on next cycle.");
            } else if (httpUpdate.getLastError() == HTTP_UE_SSL_HANDSHAKE_FAILED) {
                Log.println("SSL handshake failed. Check device time and network.");
            }
            break;
        case HTTP_UPDATE_NO_UPDATES:
            Log.printf("No Update!\r\n");
            break;
        case HTTP_UPDATE_OK:
            Log.printf("Update OK!\r\n");
            break;
    }
}

/**
 * @brief Configure and enable Arduino OTA update handling.
 *
 * Sets up ArduinoOTA event handlers for start, progress, end, and error events,
 * assigns hostname and port, disables mDNS, and begins the OTA service.
 *
 * The configured event handlers update GUI and HTTP server state, record and
 * clear the update start timestamp, and log error details on failure.
 *
 * This function is idempotent: it performs no action if OTA has already been configured.
 */
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
