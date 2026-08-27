#include "HttpWebServer.h"

#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "Enrollment.h"
#include "SPIFFS.h"
#include "TeleJson.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "ui_routes.h"

namespace HttpWebServer {

namespace {
// Matches HeadlessWiFiSettings' internal mask sentinel: posting a masked export back must
// never overwrite the stored secret with the mask itself. Any value equal to this is skipped
// on import, so `GET /json/settings | POST /json/settings` round-trips without losing secrets.
constexpr const char *SETTINGS_MASK = "***###***";

// Keys registered via HeadlessWiFiSettings::pstring() - masked from exports unless ?includeSecrets=1.
bool isSecretSetting(const String &name) {
    return name == F("wifi-password") || name == F("mqtt_user") || name == F("mqtt_pass");
}

// Every HeadlessWiFiSettings value lives in a flat SPIFFS file named /<key>. Restrict imports to
// the charset actually used by registered keys so a crafted body cannot stuff arbitrary paths.
bool validSettingName(const String &name) {
    if (name.isEmpty() || name.length() > 31) return false;
    for (size_t i = 0; i < name.length(); i++) {
        char const c = name.charAt(i);
        if (!isalnum(c) && c != '_' && c != '-' && c != '.') return false;
    }
    return true;
}

String jsonEscape(const String &s) {
    String out;
    out.reserve(s.length() + 8);
    for (size_t i = 0; i < s.length(); i++) {
        char const c = s.charAt(i);
        switch (c) {
            case '"': out += F("\\\""); break;
            case '\\': out += F("\\\\"); break;
            case '\n': out += F("\\n"); break;
            case '\r': out += F("\\r"); break;
            case '\t': out += F("\\t"); break;
            default:
                if ((unsigned char)c < 0x20) {
                    char buf[7];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    out += buf;
                } else {
                    out += c;
                }
        }
    }
    return out;
}
}  // namespace

void serializeInfo(JsonObject &root) {
    root["room"] = room;
#ifdef VERSION
    root["ver"] = VERSION;
#endif
#ifdef FIRMWARE
    root["firm"] = FIRMWARE;
#endif
}

// The three numbers telemetry already publishes over MQTT, on their own endpoint so a heap
// complaint can be diagnosed with curl and no broker: freeHeap falling while fingerprints
// holds steady is a leak, maxHeap falling while freeHeap holds is fragmentation, both
// moving with the device count is churn. Working that out took months of graph-swapping
// on #2309.
//
// Deliberately allocation-free, and deliberately not routed through serveJson. That path
// refuses with 429 when it cannot afford a 12KB document — so hanging these numbers off it
// would hide them precisely when the node is in the trouble they describe. A fixed stack
// buffer answers at any heap level. It also keeps the cost off /json, which the UI polls:
// no fingerprintMutex acquisition (contending the scan task) and no second walk of the
// free-block list on every poll.
void serveTele(AsyncWebServerRequest *request) {
    char buf[256];
    // Size(false): a GET reports what is there, it does not expire fingerprints as a side
    // effect of being observed.
    size_t const len = buildTeleJson(buf, sizeof(buf), room.c_str(),
                                     ESP.getFreeHeap(),
                                     ESP.getMaxAllocHeap(),
                                     BleFingerprintCollection::Size(false));
    if (len == 0) {
        // Only reachable via an absurdly long room. Refusing beats the alternative: a
        // truncated body under a 200 is indistinguishable from a corrupt response, and
        // this endpoint is what people reach for when they already distrust the node.
        request->send(500, "application/json", F("{\"error\":\"telemetry did not fit\"}"));
        return;
    }
    request->send(200, "application/json", buf);
}

void serializeState(JsonObject &root) {
    JsonObject node = root.createNestedObject("state");
    node["enrolling"] = enrolling;
    if (!enrolledId.isEmpty()) node["enrolledId"] = enrolledId;
    if (enrolling) node["remain"] = (enrollingEndMillis - millis()) / 1000;
}

void serializeConfigs(JsonObject &root) {
    JsonArray configs = root.createNestedArray("configs");

    for (auto it = BleFingerprintCollection::deviceConfigs.begin(); it != BleFingerprintCollection::deviceConfigs.end(); ++it) {
        const JsonObject &node = configs.createNestedObject();
        node["id"] = it->id;
        node["alias"] = it->alias;
        node["name"] = it->name;
        if (it->calRssi > -128) node["rssi@1m"] = it->calRssi;
    }
}

void serializeDevices(JsonObject &root, bool showAll) {
    JsonArray devices = root.createNestedArray("devices");

    size_t cursor = 0;
    while (auto lease = BleFingerprintCollection::AcquireNext(cursor)) {
        auto *fingerprint = lease.fingerprint;
        bool visible = fingerprint->getVisible();
        if (showAll || visible) {
            JsonObject node = devices.createNestedObject();
            if (fingerprint->fill(&node)) {
                if (showAll && visible) node[F("vis")] = true;
            } else
                devices.remove(devices.size() - 1);
        }
        BleFingerprintCollection::Release(lease);
    }
}

bool servingJson = false;

// Full-node settings backup/restore (#2493). Every HeadlessWiFiSettings-backed value (wifi, mqtt,
// room, ble tuning, pins, leds, sensors - everything the /wifi* endpoints edit) persists as a flat
// SPIFFS file named /<key>, so enumerating the filesystem root exports exactly what a node has
// explicitly configured - including board- and build-specific settings no static list can keep up
// with. Values are emitted as the exact stored strings (checkboxes are "1"/"0", ints/floats their
// decimal form), which is what makes GET -> POST lossless: whatever was exported re-imports
// byte-identical. Defaults live in firmware code, not on disk, so unset settings are simply absent
// and a restored node falls back to the same defaults it was built with.
void serveSettings(AsyncWebServerRequest *request) {
    // Same doctrine as serveTele: the backup endpoint is most valuable on a troubled node, so
    // refuse cleanly instead of dying mid-String when the heap cannot afford the document.
    if (ESP.getMaxAllocHeap() < 8192 || ESP.getFreeHeap() < 16384) {
        request->send(429, "application/json", F("{\"error\":\"low memory\"}"));
        return;
    }

    bool includeSecrets = false;
    int const paramsNr = request->params();
    for (int i = 0; i < paramsNr; i++) {
        const AsyncWebParameter *p = request->getParam(i);
        if (p->name() == "includeSecrets") includeSecrets = p->value() == "1" || p->value() == "true";
    }

    String out = F("{\"settings\":{");
    bool needsComma = false;
    File dir = SPIFFS.open("/");
    if (dir) {
        while (File entry = dir.openNextFile()) {
            // Core 2.x reports "/key", 3.x reports "key" - SPIFFS is flat, both mean the same file.
            String key = entry.name();
            if (key.startsWith("/")) key = key.substring(1);
            if (!key.isEmpty()) {
                String value = entry.readString();
                if (!includeSecrets && isSecretSetting(key)) value = SETTINGS_MASK;
                if (needsComma) out += ',';
                out += '"';
                out += jsonEscape(key);
                out += F("\":\"");
                out += jsonEscape(value);
                out += '"';
                needsComma = true;
            }
            entry.close();
        }
        dir.close();
    }
    out += F("}}");
    request->send(200, "application/json", out);
}

bool applySettings(JsonObject settings, int &written, int &removed, int &skippedMasked) {
    for (JsonPair kv : settings) {
        String key = kv.key().c_str();
        if (!validSettingName(key)) return false;

        String value;
        JsonVariant v = kv.value();
        if (v.is<bool>())
            value = v.as<bool>() ? "1" : "0";  // checkboxes store "1"/"0"; "true" would read back as set
        else if (v.is<const char *>())
            value = v.as<const char *>();
        else if (v.isNull())
            value = "";  // explicit null clears the override and falls back to the firmware default
        else
            serializeJson(v, value);

        if (value == SETTINGS_MASK) {
            skippedMasked++;
            continue;  // masked secret from a default GET - keep whatever is stored on this node
        }

        String fn = "/";
        fn += key;
        if (value.isEmpty()) {
            // HeadlessWiFiSettings::store() removes the file for empty values; mirror that so a
            // restore can undo an override instead of pinning an empty string over the default.
            if (SPIFFS.exists(fn) && !SPIFFS.remove(fn)) return false;
            removed++;
        } else {
            File f = SPIFFS.open(fn, "w");
            if (!f) return false;
            auto const w = f.print(value);
            f.close();
            if (w != value.length()) return false;
            written++;
        }
    }
    return true;
}

void serveJson(AsyncWebServerRequest *request) {
    if (servingJson) {
        request->send(429, "Too Many Requests", "Too Many Requests");
        return;  // without this we send twice and leak the first response, plus a second 12KB buffer
    }
    // Refuse rather than emit a 200 with a null or truncated body when we can't afford the
    // response buffer: under memory pressure the JSON_BUFFER_SIZE document fails to allocate,
    // serializes as `null`, and gets sent as a 200 (or AsyncTCP resets mid-body). 429 to
    // match the concurrent-request guard four lines up — same "come back later" meaning.
    //
    // The document needs one *contiguous* JSON_BUFFER_SIZE block, so getMaxAllocHeap (largest
    // free block) is the binding check — getFreeHeap alone lies under fragmentation, where
    // total free is tens of KB but no single 12KB block exists and the alloc still fails.
    // The getFreeHeap floor stays as headroom for the serialized copy + TCP send buffers.
    // ponytail: +4KB slack over the doc for AsyncJson overhead; tune on hardware.
    if (ESP.getMaxAllocHeap() < JSON_BUFFER_SIZE + 4096 || ESP.getFreeHeap() < JSON_BUFFER_SIZE * 2) {
        request->send(429, "application/json", F("{\"error\":\"low memory\"}"));
        return;
    }
    servingJson = true;
    bool showAll = false;
    const String &url = request->url();
    short subJson = 0;
    if (url.indexOf("devices") > 0) subJson = 1;
    if (url.indexOf("configs") > 0) subJson = 2;

    int const paramsNr = request->params();
    for (int i = 0; i < paramsNr; i++) {
        const AsyncWebParameter *p = request->getParam(i);
        if (p->name() == "showAll") showAll = true;
    }

    auto *response = new AsyncJsonResponse(false, JSON_BUFFER_SIZE);
    JsonObject root = response->getRoot();
    // The heap pre-check above is racy — BLE/WiFi can fragment between it and this alloc.
    // If the document buffer didn't allocate, root is null and would serialize as a bare
    // `null` sent with a 200. Catch it here and refuse instead; this is the airtight guard.
    if (root.isNull()) {
        delete response;
        servingJson = false;
        request->send(429, "application/json", F("{\"error\":\"low memory\"}"));
        return;
    }
    serializeInfo(root);
    switch (subJson) {
        case 1:
            serializeDevices(root, showAll);
            break;
        case 2:
            serializeConfigs(root);
            break;
    }
    response->setLength();
    request->send(response);
    servingJson = false;
}

void sendDataWs(AsyncWebSocketClient *client) {
    if (!ws.count()) return;
    AsyncWebSocketMessageBuffer *buffer;

    {  // scope JsonDocument so it releases its buffer
        DynamicJsonDocument doc(256);
        JsonObject root = doc.to<JsonObject>();
        serializeState(root);
        serializeInfo(root);
        size_t len = measureJson(doc);
        size_t const heap1 = ESP.getFreeHeap();
        buffer = ws.makeBuffer(len);  // will not allocate correct memory sometimes
        size_t const heap2 = ESP.getFreeHeap();
        if (!buffer || heap1 - heap2 < len) {
            ws.closeAll(1013);     // code 1013 = temporary overload, try again later
            ws.cleanupClients(0);  // disconnect all clients to release memory
            return;                // out of memory
        }
        serializeJson(doc, buffer->get(), len);
    }
    if (client) {
        client->text(buffer);
    } else {
        ws.textAll(buffer);
    }
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        sendDataWs(nullptr);
    } else if (type == WS_EVT_DATA) {
        auto *info = static_cast<AwsFrameInfo *>(arg);
        if (info->final && info->index == 0 && info->len == len) {
            if (info->opcode == WS_TEXT) {
                auto doc = DynamicJsonDocument(256);
                auto error = deserializeJson(doc, data, len);
                auto root = doc.as<JsonObject>();
                if (error || root.isNull()) {
                    return;
                }

                if (root.containsKey("command")) {
                    auto command = root["command"].as<String>();
                    auto payload = root.containsKey("payload") ? root["payload"].as<String>() : "";
                    Enrollment::Command(command, payload);
                }
            }
        }
    }
}

void onRestart(AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Restarting...");
    ESP.restart();
}

void Init(AsyncWebServer *server) {
    DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Origin"), "*");
    DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Methods"), "GET, POST, DELETE, OPTIONS");
    DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Headers"), "Content-Type");

    // Low-risk browser hardening headers for the local Web UI and JSON endpoints.
    DefaultHeaders::Instance().addHeader(F("X-Content-Type-Options"), F("nosniff"));
    DefaultHeaders::Instance().addHeader(F("Referrer-Policy"), F("no-referrer"));
    DefaultHeaders::Instance().addHeader(F("Permissions-Policy"),
                                         F("accelerometer=(), camera=(), geolocation=(), gyroscope=(), microphone=(), usb=()"));

    server->on("/", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(200);
        response->addHeader(F("Access-Control-Max-Age"), F("7200"));
        request->send(response);
    });

    setupRoutes(server); // from ui_routes.h

    server->on("/restart", HTTP_POST, onRestart);
    server->on("/reboot", HTTP_POST, onRestart);
    server->on("/json/tele", HTTP_GET, serveTele);
    // Must be registered before the catch-all "/json" GET route below, which also matches
    // subpaths (that is how serveJson sees /json/configs and /json/devices).
    server->on("/json/settings", HTTP_GET, serveSettings);
    server->on("/json", HTTP_GET, serveJson);

    server->on("/json/configs", HTTP_DELETE, [](AsyncWebServerRequest *request) {
        if (!request->hasParam("id")) {
            request->send(400, "application/json", F("{\"error\":\"Missing required parameter: id\"}"));
            return;
        }

        String const id = request->getParam("id")->value();
        if (deleteConfig(id)) {
            request->send(200, "application/json", F("{\"success\":true}"));
        } else {
            request->send(500, "application/json", F("{\"error\":\"Failed to delete config\"}"));
        }
    });

    // Settings import (#2493). Registered before the generic "/json" handler so this body gets
    // the larger document it needs without changing the memory profile of the configs POST.
    AsyncCallbackJsonWebHandler *settingsHandler = new AsyncCallbackJsonWebHandler(
        "/json/settings", [](AsyncWebServerRequest *request, JsonVariant &json) {
            if (request->method() != HTTP_POST) {
                request->send(405, "application/json", F("{\"error\":\"Method not allowed\"}"));
                return;
            }

            JsonObject root = json.as<JsonObject>();
            if (root.isNull() || !root["settings"].is<JsonObject>()) {
                request->send(400, "application/json", F("{\"error\":\"Expected {\\\"settings\\\":{...}}\"}"));
                return;
            }

            int written = 0, removed = 0, skippedMasked = 0;
            if (!applySettings(root["settings"].as<JsonObject>(), written, removed, skippedMasked)) {
                request->send(500, "application/json", F("{\"error\":\"Failed to persist settings\"}"));
                return;
            }

            String out = F("{\"success\":true,\"written\":");
            out += written;
            out += F(",\"removed\":");
            out += removed;
            out += F(",\"skippedMasked\":");
            out += skippedMasked;
            out += F(",\"rebootRequired\":true}");
            request->send(200, "application/json", out);
        },
        8192);  // a full ~100-setting backup does not fit the 1KB default document
    settingsHandler->setMaxContentLength(8192);
    server->addHandler(settingsHandler);

    AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler(
        "/json", [](AsyncWebServerRequest *request, JsonVariant &json) {
            const String &url = request->url();

            // Handle configs endpoint
            if (url.indexOf("configs") > 0) {
                JsonObject root = json.as<JsonObject>();

                if (root.isNull()) {
                    request->send(400, "application/json", F("{\"error\":\"Invalid JSON\"}"));
                    return;
                }

                // Extract required fields
                if (!root.containsKey("id")) {
                    request->send(400, "application/json", F("{\"error\":\"Missing required field: id\"}"));
                    return;
                }

                String id = root["id"].as<String>();
                // Use id as alias if none provided
                String alias = root.containsKey("alias") && !root["alias"].as<String>().isEmpty() ? root["alias"].as<String>() : id;

                // Extract optional fields
                String name = root.containsKey("name") ? root["name"].as<String>() : "";
                int calRssi = root.containsKey("rssi@1m") ? root["rssi@1m"].as<int>() : -128;

                // Save the config
                if (sendConfig(id, alias, name, calRssi)) {
                    request->send(200, "application/json", F("{\"success\":true}"));
                } else {
                    request->send(500, "application/json", F("{\"error\":\"Failed to save config\"}"));
                }
                return;
            }

            // Default response for unhandled endpoints
            request->send(404, "application/json", F("{\"error\":\"Unknown endpoint\"}"));
        });
    server->addHandler(handler);
    server->addHandler(&ws);

    ws.onEvent(onWsEvent);
}

void Loop() { ws.cleanupClients(); }

void UpdateStart() {
    ws.enable(false);
    ws.textAll("OTA Update Started");
    ws.closeAll();
}

void UpdateEnd() { ws.enable(true); }

void SendState() { sendDataWs(nullptr); }

}  // namespace HttpWebServer
