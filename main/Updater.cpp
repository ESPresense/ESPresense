#include "Updater.h"

#include "GUI.h"
#include "HttpWebServer.h"
#include "Settings.h"
#include "defaults.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_ota_ops.h"
#include "globals.h"
#include "mqtt.h"

namespace Updater {

bool autoUpdateEnabled, prerelease;
unsigned long lastFirmwareCheck = 0;
unsigned short autoUpdateAttempts = 0;
std::string updateUrl;

std::string getFirmwareUrl() {
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

std::string getVersionMarker() {
#ifdef VERSION
    return std::string("/") + VERSION + "/";
#else
    return "";
#endif
}

static const char* userAgent() {
#ifdef VERSION
    return "ESPresense/" VERSION;
#else
    return "ESPresense/0.0";
#endif
}

namespace {
std::string location;
esp_err_t headEvent(esp_http_client_event_t* ev) {
    if (ev->event_id == HTTP_EVENT_ON_HEADER && strcasecmp(ev->header_key, "Location") == 0) location = ev->header_value;
    return ESP_OK;
}
}  // namespace

// HEAD the release URL without following the redirect: a Location that lacks our version
// marker is a newer build. Store it and reboot into the update path.
void checkForUpdates() {
    auto versionMarker = getVersionMarker();
    if (versionMarker.empty()) return;
    auto url = getFirmwareUrl();
    Log.printf("Checking for new firmware version at '%s'\r\n", url.c_str());
    location.clear();
    esp_http_client_config_t cfg = {};
    cfg.url = url.c_str();
    cfg.method = HTTP_METHOD_HEAD;
    cfg.timeout_ms = 12000;
    cfg.disable_auto_redirect = true;
    cfg.event_handler = headEvent;
    cfg.crt_bundle_attach = esp_crt_bundle_attach;
    cfg.user_agent = userAgent();
    esp_http_client_handle_t http = esp_http_client_init(&cfg);
    if (!http) return;
    esp_err_t err = esp_http_client_perform(http);
    int code = esp_http_client_get_status_code(http);
    esp_http_client_cleanup(http);
    if (err != ESP_OK) {
        Log.printf("Error on checking for update (%s)\r\n", esp_err_to_name(err));
        return;
    }
    if (code > 300 && code < 400) {
        if (location.find(versionMarker) == std::string::npos) {
            Log.printf("Found new version: %s\r\n", location.c_str());
            spurt("/update", location);
            Log.println("Rebooting to start update");
            esp_restart();
        }
    } else
        Log.printf("Error on checking for update (sc=%d)\r\n", code);
}

void firmwareUpdate() {
    std::string url = startsWith(updateUrl, "http") ? updateUrl : getFirmwareUrl();
    autoUpdateAttempts++;
    GUI::Update(UPDATE_STARTED);
    HttpWebServer::UpdateStart();
    Log.printf("Starting firmware update from: %s\n", url.c_str());

    esp_http_client_config_t http = {};
    http.url = url.c_str();
    http.timeout_ms = 12000;
    http.keep_alive_enable = true;
    http.crt_bundle_attach = esp_crt_bundle_attach;
    http.user_agent = userAgent();
    esp_https_ota_config_t ota = {};
    ota.http_config = &http;
    esp_https_ota_handle_t handle = nullptr;
    esp_err_t err = esp_https_ota_begin(&ota, &handle);
    if (err != ESP_OK) {
        Log.printf("Http Update Failed: %s\r\n", esp_err_to_name(err));
        GUI::Update(UPDATE_COMPLETE);
        HttpWebServer::UpdateEnd();
        return;
    }
    int total = esp_https_ota_get_image_size(handle), lastPct = -1;
    while ((err = esp_https_ota_perform(handle)) == ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
        int pct = total > 0 ? esp_https_ota_get_image_len_read(handle) * 100 / total : 0;
        if (pct / 10 != lastPct / 10) {
            lastPct = pct;
            GUI::Update(pct);
        }
    }
    bool ok = err == ESP_OK && esp_https_ota_is_complete_data_received(handle);
    esp_err_t fin = esp_https_ota_finish(handle);
    GUI::Update(UPDATE_COMPLETE);
    HttpWebServer::UpdateEnd();
    if (ok && fin == ESP_OK) {
        Settings::remove("/update");
        Log.println("Firmware update completed successfully!");
        Log.println("Update complete, rebooting...");
        delay(200);
        esp_restart();
    }
    Log.printf("Firmware update failed to apply (%s / %s)\r\n", esp_err_to_name(err), esp_err_to_name(fin));
}

void Setup() {
    if (Settings::exists("/update")) firmwareUpdate();
}

void Loop() {
    if (!autoUpdateEnabled) return;
    unsigned long now = millis();
    if (now - lastFirmwareCheck > CHECK_FOR_UPDATES_INTERVAL) {
        lastFirmwareCheck = now;
        checkForUpdates();
    }
}

bool SendOnline() {
    return pub((roomsTopic + "/auto_update").c_str(), 0, true, autoUpdateEnabled ? "ON" : "OFF") &&
           pub((roomsTopic + "/prerelease").c_str(), 0, true, prerelease ? "ON" : "OFF");
}

bool SendDiscovery() {
    // ponytail: ArduinoOTA (espota) is gone with the Arduino core; retire its HA switch.
    return sendSwitchDiscovery("Auto Update", EC_CONFIG) && sendDeleteDiscovery("switch", "Arduino OTA") &&
           sendSwitchDiscovery("Prerelease", EC_CONFIG) && sendButtonDiscovery("Update", EC_DIAGNOSTIC);
}

void ConnectToWifi(bool updating) {
    autoUpdateEnabled = Settings::checkbox("auto_update", DEFAULT_AUTO_UPDATE, "Automatically update");
    prerelease = Settings::checkbox("prerelease", false, "Include pre-released versions in auto-update");
    updateUrl = Settings::string("update", "", "If set will update from this url on next boot");
}

void MarkOtaSuccess() {
    const esp_partition_t* running = esp_ota_get_running_partition();
    esp_ota_img_states_t state;
    if (esp_ota_get_state_partition(running, &state) == ESP_OK && state == ESP_OTA_IMG_PENDING_VERIFY)
        esp_ota_mark_app_valid_cancel_rollback();
}

bool Command(std::string& command, std::string& pay) {
    if (command == "auto_update") {
        autoUpdateEnabled = pay == "ON";
        spurt("/auto_update", toStr(autoUpdateEnabled));
    } else if (command == "prerelease") {
        prerelease = pay == "ON";
        spurt("/prerelease", toStr(prerelease));
    } else if (command == "update") {
        spurt("/update", pay);
        esp_restart();
    } else
        return false;
    return true;
}
}  // namespace Updater
