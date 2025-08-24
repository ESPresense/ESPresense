#include "HeadlessWiFiSettings.h"

#define ESPFS SPIFFS
#define ESPMAC (Sprintf("%06" PRIx32, ((uint32_t)(ESP.getEfuseMac() >> 24))))

#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <esp_task_wdt.h>
#include <esp_wifi.h>
#include <limits.h>

#include <vector>
#include "json_utils.h"

#define Sprintf(f, ...) ({ char* s; asprintf(&s, f, __VA_ARGS__); String r = s; free(s); r; })

namespace { // Helpers
    String slurp(const String &fn) {
        File f = ESPFS.open(fn, "r");
        String r = f.readString();
        f.close();
        return r;
    }

    bool spurt(const String &fn, const String &content) {
        if (content.isEmpty())
            return ESPFS.exists(fn) ? ESPFS.remove(fn) : true;
        File f = ESPFS.open(fn, "w");
        if (!f) return false;
        auto w = f.print(content);
        f.close();
        return w == content.length();
    }

    enum class ParamType {
        Dropdown,
        String,
        Password,
        Int,
        Float,
        Bool
    };

    struct HeadlessWiFiSettingsParameter {
        String name;
        String label;
        String value;
        String init;
        long min = LONG_MIN;
        long max = LONG_MAX;
        ParamType type;

        String filename() {
            String fn = "/";
            fn += name;
            return fn;
        }

        bool store() { return (name && name.length()) ? spurt(filename(), value) : true; }

        void fill() { if (name && name.length()) value = slurp(filename()); }

        virtual void set(const String &) = 0;

        virtual String jsonValue() = 0;
        virtual String jsonDefault() = 0;

        ParamType getType() const { return type; }
    };

    struct HeadlessWiFiSettingsDropdown : HeadlessWiFiSettingsParameter {
        HeadlessWiFiSettingsDropdown() { type = ParamType::Dropdown; }
        virtual void set(const String &v) { value = v; }

        std::vector<String> options;

        String jsonValue() {
            if (value == "") return "";
            String j = F("\"{name}\":\"{value}\"");
            j.replace("{name}", json_encode(name));
            j.replace("{value}", json_encode(value));
            return j;
        }

        String jsonDefault() {
            if (init == "") return "";
            String j = F("\"{name}\":\"{value}\"");
            j.replace("{name}", json_encode(name));
            j.replace("{value}", json_encode(init));
            return j;
        }
    };

    struct HeadlessWiFiSettingsString : HeadlessWiFiSettingsParameter {
        HeadlessWiFiSettingsString() { type = ParamType::String; }
        virtual void set(const String &v) { value = v; }

        String jsonValue() {
            if (value == "") return "";
            String j = F("\"{name}\":\"{value}\"");
            j.replace("{name}", json_encode(name));
            j.replace("{value}", json_encode(value));
            return j;
        }

        String jsonDefault() {
            if (init == "") return "";
            String j = F("\"{name}\":\"{value}\"");
            j.replace("{name}", json_encode(name));
            j.replace("{value}", json_encode(init));
            return j;
        }
    };

    static const char* const MASKED_PASSWORD = "***###***";

    struct HeadlessWiFiSettingsPassword : HeadlessWiFiSettingsParameter {
        HeadlessWiFiSettingsPassword() { type = ParamType::Password; }
        virtual void set(const String &v) {
            if (v == MASKED_PASSWORD) return;
            value = v;
        }

        String jsonValue() {
            if (!value.length()) return "";
            String j = F("\"{name}\":\"{value}\"");
            j.replace("{name}", json_encode(name));
            j.replace("{value}", json_encode(MASKED_PASSWORD));
            return j;
        }

        String jsonDefault() {
            return "";
        }
    };  // HeadlessWiFiSettingsPassword

    struct HeadlessWiFiSettingsInt : HeadlessWiFiSettingsParameter {
        HeadlessWiFiSettingsInt() { type = ParamType::Int; }
        virtual void set(const String &v) { value = v; }

        String jsonValue() {
            if (value == "") return "";
            String j = F("\"{name}\":{value}");
            j.replace("{name}", json_encode(name));
            j.replace("{value}", String(value.toInt()));
            return j;
        }

        String jsonDefault() {
            if (init == "") return "";
            String j = F("\"{name}\":{value}");
            j.replace("{name}", json_encode(name));
            j.replace("{value}", String(init.toInt()));
            return j;
        }
    };

    struct HeadlessWiFiSettingsFloat : HeadlessWiFiSettingsParameter {
        HeadlessWiFiSettingsFloat() { type = ParamType::Float; }
        virtual void set(const String &v) { value = v; }

        String jsonValue() {
            if (value == "") return "";
            String j = F("\"{name}\":{value}");
            j.replace("{name}", json_encode(name));
            j.replace("{value}", String(value.toFloat()));
            return j;
        }

        String jsonDefault() {
            if (init == "") return "";
            String j = F("\"{name}\":{value}");
            j.replace("{name}", json_encode(name));
            j.replace("{value}", String(init.toFloat()));
            return j;
        }
    };

    struct HeadlessWiFiSettingsBool : HeadlessWiFiSettingsParameter {
        HeadlessWiFiSettingsBool() { type = ParamType::Bool; }
        virtual void set(const String &v) { value = v.length() ? "1" : "0"; }

        String jsonValue() {
            if (value == "") return "";
            String j = F("\"{name}\":{value}");
            j.replace("{name}", json_encode(name));
            j.replace("{value}", value.toInt() ? "true" : "false");
            return j;
        }

        String jsonDefault() {
            if (init == "") return "";
            String j = F("\"{name}\":{value}");
            j.replace("{name}", json_encode(name));
            j.replace("{value}", init.toInt() ? "true" : "false");
            return j;
        }
    };

    // Parallel vectors for endpoint names and parameters
    std::vector<String> endpointNames;
    std::vector<std::vector<HeadlessWiFiSettingsParameter *>> endpointParams;
    uint8_t currentEndpointIndex = 0;

    std::vector<HeadlessWiFiSettingsParameter *> *params() {
        // Ensure we have at least the main endpoint
        if (endpointNames.empty()) {
            endpointNames.push_back("main");
            endpointParams.push_back({});
        }
        return &endpointParams[currentEndpointIndex];
    }

    // Find or create endpoint
    uint8_t findOrCreateEndpoint(const String& name) {
        // Look for existing endpoint
        for (size_t i = 0; i < endpointNames.size(); i++) {
            if (endpointNames[i] == name) {
                return i;
            }
        }
        // Create new endpoint
        endpointNames.push_back(name);
        endpointParams.push_back({});
        return endpointNames.size() - 1;
    }
} // namespace

String HeadlessWiFiSettingsClass::pstring(const String &name, const String &init, const String &label) {
    begin();
    auto *x = new HeadlessWiFiSettingsPassword();
    x->name = name;
    x->label = label.length() ? label : name;
    x->init = init;
    x->fill();

    params()->push_back(x);
    return x->value.length() ? x->value : x->init;
}

String HeadlessWiFiSettingsClass::string(const String &name, const String &init, const String &label) {
    begin();
    auto *x = new HeadlessWiFiSettingsString();
    x->name = name;
    x->label = label.length() ? label : name;
    x->init = init;
    x->fill();

    params()->push_back(x);
    return x->value.length() ? x->value : x->init;
}

String HeadlessWiFiSettingsClass::string(const String &name, unsigned int max_length, const String &init, const String &label) {
    String rv = string(name, init, label);
    params()->back()->max = max_length;
    return rv;
}

String HeadlessWiFiSettingsClass::string(const String &name, unsigned int min_length, unsigned int max_length, const String &init, const String &label) {
    String rv = string(name, init, label);
    params()->back()->min = min_length;
    params()->back()->max = max_length;
    return rv;
}

long HeadlessWiFiSettingsClass::dropdown(const String &name, std::vector<String> options, long init, const String &label) {
    begin();
    auto *x = new HeadlessWiFiSettingsDropdown();
    x->name = name;
    x->label = label.length() ? label : name;
    x->init = init;
    x->options = options;
    x->fill();

    params()->push_back(x);
    return (x->value.length() ? x->value : x->init).toInt();
}

long HeadlessWiFiSettingsClass::integer(const String &name, long init, const String &label) {
    begin();
    auto *x = new HeadlessWiFiSettingsInt();
    x->name = name;
    x->label = label.length() ? label : name;
    x->init = init;
    x->fill();

    params()->push_back(x);
    return (x->value.length() ? x->value : x->init).toInt();
}

long HeadlessWiFiSettingsClass::integer(const String &name, long min, long max, long init, const String &label) {
    long rv = integer(name, init, label);
    params()->back()->min = min;
    params()->back()->max = max;
    return rv;
}

float HeadlessWiFiSettingsClass::floating(const String &name, float init, const String &label) {
    begin();
    auto *x = new HeadlessWiFiSettingsFloat();
    x->name = name;
    x->label = label.length() ? label : name;
    x->init = init;
    x->fill();

    params()->push_back(x);
    return (x->value.length() ? x->value : x->init).toFloat();
}

float HeadlessWiFiSettingsClass::floating(const String &name, long min, long max, float init, const String &label) {
    float rv = floating(name, init, label);
    params()->back()->min = min;
    params()->back()->max = max;
    return rv;
}

bool HeadlessWiFiSettingsClass::checkbox(const String &name, bool init, const String &label) {
    begin();
    auto *x = new HeadlessWiFiSettingsBool();
    x->name = name;
    x->label = label.length() ? label : name;
    x->init = String((int)init);
    x->fill();

    if (!x->value.length()) x->value = x->init;

    params()->push_back(x);
    return x->value.toInt();
}

void HeadlessWiFiSettingsClass::markEndpoint(const String& name) {
    currentEndpointIndex = findOrCreateEndpoint(name);
}

void HeadlessWiFiSettingsClass::markExtra() {
    currentEndpointIndex = findOrCreateEndpoint("extras");
}

void HeadlessWiFiSettingsClass::httpSetup(bool wifi) {
    begin();

    static bool const configureWifi = wifi;
    static String ip = WiFi.softAPIP().toString();

    if (onHttpSetup) onHttpSetup(&http);

    auto redirect = [](AsyncWebServerRequest *request) {
        if (!configureWifi) return false;
        // iPhone doesn't deal well with redirects to http://hostname/ and
        // will wait 40 to 60 seconds before succesful retry. Works flawlessly
        // with http://ip/ though.
        if (request->host() == ip) return false;

        request->redirect("http://" + ip + "/");
        return true;
    };

    // Get dropdown options endpoint
    http.on("/wifi/options/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        String path = request->url();
        Serial.print("GET ");
        Serial.println(path);

        String paramName = path.substring(13); // Remove "/wifi/options/"

        // Search all endpoints for the parameter
        HeadlessWiFiSettingsDropdown* dropdown = nullptr;
        for (auto& params : endpointParams) {
            for (auto& p : params) {
                if (p->name == paramName) {
                    if (p->getType() == ParamType::Dropdown) {
                        dropdown = static_cast<HeadlessWiFiSettingsDropdown*>(p);
                        break;
                    }
                }
            }
            if (dropdown) break;
        }

        if (!dropdown) {
            request->send(404, "text/plain", "Dropdown not found");
            return;
        }

        AsyncResponseStream *response = request->beginResponseStream("application/json; charset=utf-8");
        response->print("[");
        bool needsComma = false;
        for (const auto& option : dropdown->options) {
            if (needsComma) response->print(",");
            response->printf("\"%s\"", json_encode(option).c_str());
            needsComma = true;
        }
        response->print("]");
        request->send(response);
    });

    http.on("/wifi/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
        String path = request->url();
        Serial.print("GET ");
        Serial.println(path);

        int numNetworks = WiFi.scanNetworks();
        AsyncResponseStream *response = request->beginResponseStream("application/json; charset=utf-8");
        response->print("{\"networks\":{");

        bool needsComma = false;
        struct Network {
            String ssid;
            int rssi;
        };
        std::vector<Network> networks;

        // First pass: collect all networks with their RSSI values
        for (int i = 0; i < numNetworks; i++) {
            String ssid = WiFi.SSID(i);
            if (ssid.isEmpty()) continue;  // Skip hidden networks

            int rssi = WiFi.RSSI(i);
            bool found = false;

            // Update existing network if we've seen it before
            for (auto& network : networks) {
                if (network.ssid == ssid) {
                    if (rssi > network.rssi) {
                        network.rssi = rssi;  // Keep the highest RSSI value
                    }
                    found = true;
                    break;
                }
            }

            // Add new network if we haven't seen it
            if (!found) {
                networks.push_back({ssid, rssi});
            }
        }

        // Second pass: output the networks with their highest RSSI values
        for (const auto& network : networks) {
            if (needsComma) response->print(",");
            response->printf("\"%s\":%d", json_encode(network.ssid).c_str(), network.rssi);
            needsComma = true;
        }

        response->print("}}");
        request->send(response);
        WiFi.scanDelete();
    });

    // Handler for /wifi/{name} endpoints
    http.on("/wifi", HTTP_GET, [this](AsyncWebServerRequest *request) {
        String path = request->url();
        Serial.print("GET ");
        Serial.println(path);
        String endpointName;
        size_t endpointIndex;

        if (path == "/wifi") {
            endpointName = "main";
        } else if (path.startsWith("/wifi/")) {
            endpointName = path.substring(6); // Remove "/wifi/"
        } else {
            request->send(404);
            return;
        }

        // Find the endpoint
        bool found = false;
        for (size_t i = 0; i < endpointNames.size(); i++) {
            if (endpointNames[i] == endpointName) {
                endpointIndex = i;
                found = true;
                break;
            }
        }

        if (!found) {
            request->send(404, "text/plain", "Endpoint not found");
            return;
        }

        AsyncResponseStream *response = request->beginResponseStream("application/json; charset=utf-8");
        response->print("{");

        // Output current values
        response->print("\"values\":{");
        bool needsComma = false;
        for (auto &p : endpointParams[endpointIndex]) {
            auto s = p->jsonValue();
            if (s == "") continue;
            if (needsComma) response->print(",");
            response->print(s);
            needsComma = true;
        }
        response->print("}");

        // Output defaults
        response->print(",\"defaults\":{");
        needsComma = false;
        for (auto &p : endpointParams[endpointIndex]) {
            auto s = p->jsonDefault();
            if (s == "") continue;
            if (needsComma) response->print(",");
            response->print(s);
            needsComma = true;
        }
        response->print("}}");
        request->send(response);
    });

    // Handler for /wifi/{name} POST endpoints
    http.on("/wifi", HTTP_POST, [this](AsyncWebServerRequest *request) {
        String path = request->url();
        Serial.print("POST ");
        Serial.println(path);

        String endpointName;
        size_t endpointIndex;

        if (path == "/wifi") {
            endpointName = "main";
        } else if (path.startsWith("/wifi/")) {
            endpointName = path.substring(6); // Remove "/wifi/"
        } else {
            request->send(404);
            return;
        }

        // Find the endpoint
        bool found = false;
        for (size_t i = 0; i < endpointNames.size(); i++) {
            if (endpointNames[i] == endpointName) {
                endpointIndex = i;
                found = true;
                break;
            }
        }

        if (!found) {
            request->send(404, "text/plain", "Endpoint not found");
            return;
        }

        bool ok = true;
        for (auto &p : endpointParams[endpointIndex]) {
            p->set(request->arg(p->name));
            if (!p->store()) ok = false;
        }

        if (ok) {
            request->send(200);
            if (onConfigSaved) onConfigSaved();
        } else {
            request->send(500, "text/plain", "Error writing to flash filesystem");
        }
    });

    http.onNotFound([this, &redirect](AsyncWebServerRequest *request) {
        String path = request->url();
        Serial.print("GET ");
        Serial.println(path);
        if (redirect(request)) return;
        request->send(404, "text/plain", "404");
    });

    http.begin();
}

void HeadlessWiFiSettingsClass::portal() {
    begin();

    // Just disconnect and set AP mode, no need to scan since we have /wifi/scan endpoint
    WiFi.disconnect(true, true);
    WiFi.mode(WIFI_AP);

    Serial.println(F("Starting access point for configuration portal."));
    if (secure && password.length()) {
        Serial.printf("SSID: '%s', Password: '%s'\n", hostname.c_str(), password.c_str());
        if (!WiFi.softAP(hostname.c_str(), password.c_str()))
            Serial.println("Failed to start access point!");
    } else {
        Serial.printf("SSID: '%s'\n", hostname.c_str());
        if (!WiFi.softAP(hostname.c_str()))
            Serial.println("Failed to start access point!");
    }
    delay(500);
    DNSServer dns;
    dns.setTTL(0);
    dns.start(53, "*", WiFi.softAPIP());

    if (onPortal) onPortal();
    String ip = WiFi.softAPIP().toString();
    Serial.printf("IP: %s\n", ip.c_str());

    httpSetup(true);

    unsigned long starttime = millis();
    int desired = 0;
    for (;;) {
        dns.processNextRequest();
        if (onPortalWaitLoop && (millis() - starttime) > desired) {
            desired = onPortalWaitLoop();
            starttime = millis();
        }
        esp_task_wdt_reset();
        delay(1);
    }
}

bool HeadlessWiFiSettingsClass::connect(bool portal, int wait_seconds) {
    begin();

    if (WiFi.getMode() != WIFI_OFF) {
        WiFi.mode(WIFI_OFF);
    }

    esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_BW_HT20);

    WiFi.persistent(false);
    WiFi.setAutoReconnect(false);

    String ssid = slurp("/wifi-ssid");
    String pw = slurp("/wifi-password");
    if (ssid.length() == 0) {
        Serial.println(F("First contact!\n"));
        this->portal();
    }

    Serial.print(F("Connecting to WiFi SSID '"));
    Serial.print(ssid);
    Serial.print(F("'"));
    if (onConnect) onConnect();

    WiFi.setHostname(hostname.c_str());
    auto status = WiFi.begin(ssid.c_str(), pw.c_str());

    unsigned long const wait_ms = wait_seconds * 1000UL;
    unsigned long starttime = millis();
    unsigned long lastbegin = starttime;
    while (status != WL_CONNECTED) {
        if (millis() - lastbegin > 60000) {
            lastbegin = millis();
            Serial.print("*");
            WiFi.disconnect(true, true);
            status = WiFi.begin(ssid.c_str(), pw.c_str());
        } else {
            Serial.print(".");
            status = WiFi.status();
        }
        delay(onWaitLoop ? onWaitLoop() : 100);
        if (wait_seconds >= 0 && millis() - starttime > wait_ms)
            break;
    }

    if (status != WL_CONNECTED) {
        Serial.printf(" failed (status=%d).\n", status);
        if (onFailure) onFailure();
        if (portal) this->portal();
        return false;
    }

    Serial.println(WiFi.localIP().toString());
    if (onSuccess) onSuccess();
    return true;
}

void HeadlessWiFiSettingsClass::begin() {
    if (begun) return;
    begun = true;
    if (hostname.endsWith("-")) hostname += ESPMAC;
}

HeadlessWiFiSettingsClass::HeadlessWiFiSettingsClass() : http(80) {
    hostname = F("esp32-");
}

HeadlessWiFiSettingsClass HeadlessWiFiSettings;