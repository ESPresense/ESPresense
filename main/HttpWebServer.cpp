#include "HttpWebServer.h"

#include <algorithm>
#include <cstring>
#include <memory>

#include "ArduinoJson.h"
#include "BleFingerprintCollection.h"
#include "Enrollment.h"
#include "HttpStatic.h"
#include "Network.h"
#include "Settings.h"
#include "TeleJson.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "ui_routes.h"

namespace HttpWebServer {

namespace {
httpd_handle_t server = nullptr;
bool captivePortal = false;
bool wsEnabled = true;

esp_err_t sendJsonStr(httpd_req_t* req, const char* status, const std::string& body) {
    commonHeaders(req);
    httpd_resp_set_status(req, status);
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, body.c_str(), body.size());
}

// Streams the document straight into chunked responses: no second 12KB copy of /json.
struct ChunkWriter {
    httpd_req_t* req;
    char buf[512];
    size_t n = 0;
    bool ok = true;
    size_t write(uint8_t c) { return write(&c, 1); }
    size_t write(const uint8_t* d, size_t len) {
        size_t done = 0;
        while (done < len) {
            size_t take = std::min(len - done, sizeof(buf) - n);
            memcpy(buf + n, d + done, take);
            n += take;
            done += take;
            if (n == sizeof(buf)) flush();
        }
        return len;
    }
    void flush() {
        if (n && ok) ok = httpd_resp_send_chunk(req, buf, n) == ESP_OK;
        n = 0;
    }
};

esp_err_t sendJsonDoc(httpd_req_t* req, JsonVariantConst v) {
    commonHeaders(req);
    httpd_resp_set_type(req, "application/json");
    ChunkWriter w{req};
    serializeJson(v, w);
    w.flush();
    if (!w.ok) return ESP_FAIL;
    return httpd_resp_send_chunk(req, nullptr, 0);
}

void serializeInfo(JsonObject& root) {
    root["room"] = room;
#ifdef VERSION
    root["ver"] = VERSION;
#endif
#ifdef FIRMWARE
    root["firm"] = FIRMWARE;
#endif
}

void serializeState(JsonObject& root) {
    JsonObject node = root.createNestedObject("state");
    node["enrolling"] = enrolling;
    if (!enrolledId.empty()) node["enrolledId"] = enrolledId;
    if (enrolling) node["remain"] = (enrollingEndMillis - millis()) / 1000;
}

void serializeConfigs(JsonObject& root) {
    JsonArray configs = root.createNestedArray("configs");
    for (auto& c : BleFingerprintCollection::deviceConfigs) {
        JsonObject node = configs.createNestedObject();
        node["id"] = c.id;
        node["alias"] = c.alias;
        node["name"] = c.name;
        if (c.calRssi > -128) node["rssi@1m"] = c.calRssi;
    }
}

void serializeDevices(JsonObject& root, bool showAll) {
    JsonArray devices = root.createNestedArray("devices");
    size_t cursor = 0;
    while (auto lease = BleFingerprintCollection::AcquireNext(cursor)) {
        auto* fingerprint = lease.fingerprint;
        bool visible = fingerprint->getVisible();
        if (showAll || visible) {
            JsonObject node = devices.createNestedObject();
            if (fingerprint->fill(&node)) {
                if (showAll && visible) node["vis"] = true;
            } else
                devices.remove(devices.size() - 1);
        }
        BleFingerprintCollection::Release(lease);
    }
}

std::string uriPath(httpd_req_t* req) {
    std::string u = req->uri;
    auto q = u.find('?');
    return q == std::string::npos ? u : u.substr(0, q);
}

// /json/tele: allocation-free so it answers at any heap level (see TeleJson.h).
esp_err_t serveTele(httpd_req_t* req) {
    char buf[256];
    size_t const len = buildTeleJson(buf, sizeof(buf), room.c_str(), freeHeap(), maxAllocHeap(),
                                     BleFingerprintCollection::Size(false));
    if (len == 0) return sendJsonStr(req, "500 Internal Server Error", "{\"error\":\"telemetry did not fit\"}");
    commonHeaders(req);
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, buf, len);
}

bool servingJson = false;

esp_err_t serveJson(httpd_req_t* req) {
    std::string path = uriPath(req);
    if (path == "/json/tele") return serveTele(req);
    if (servingJson) return sendJsonStr(req, "429 Too Many Requests", "Too Many Requests");
    // Refuse rather than emit a 200 with a null/truncated body when the 12KB document cannot be
    // allocated. Largest free block is the binding check under fragmentation.
    if (maxAllocHeap() < JSON_BUFFER_SIZE + 4096 || freeHeap() < JSON_BUFFER_SIZE * 2)
        return sendJsonStr(req, "429 Too Many Requests", "{\"error\":\"low memory\"}");
    servingJson = true;
    short subJson = 0;
    if (path.find("devices") != std::string::npos) subJson = 1;
    if (path.find("configs") != std::string::npos) subJson = 2;
    std::string dummy;
    bool showAll = queryParam(req, "showAll", dummy);

    esp_err_t rc;
    {
        DynamicJsonDocument doc(JSON_BUFFER_SIZE);
        JsonObject root = doc.to<JsonObject>();
        if (doc.capacity() == 0 || root.isNull()) {
            servingJson = false;
            return sendJsonStr(req, "429 Too Many Requests", "{\"error\":\"low memory\"}");
        }
        serializeInfo(root);
        if (subJson == 1) serializeDevices(root, showAll);
        if (subJson == 2) serializeConfigs(root);
        rc = sendJsonDoc(req, doc);
    }
    servingJson = false;
    return rc;
}

esp_err_t postJson(httpd_req_t* req) {
    std::string path = uriPath(req);
    if (path.find("configs") == std::string::npos) return sendJsonStr(req, "404 Not Found", "{\"error\":\"Unknown endpoint\"}");
    std::string body;
    if (!readBody(req, body, 2048)) return sendJsonStr(req, "413 Payload Too Large", "{\"error\":\"Body too large\"}");
    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, body) || !doc.is<JsonObject>()) return sendJsonStr(req, "400 Bad Request", "{\"error\":\"Invalid JSON\"}");
    JsonObject root = doc.as<JsonObject>();
    if (!root.containsKey("id")) return sendJsonStr(req, "400 Bad Request", "{\"error\":\"Missing required field: id\"}");
    std::string id = root["id"].as<std::string>();
    std::string alias = root.containsKey("alias") && !root["alias"].as<std::string>().empty() ? root["alias"].as<std::string>() : id;
    std::string name = root.containsKey("name") ? root["name"].as<std::string>() : "";
    int calRssi = root.containsKey("rssi@1m") ? root["rssi@1m"].as<int>() : -128;
    if (sendConfig(id, alias, name, calRssi)) return sendJsonStr(req, "200 OK", "{\"success\":true}");
    return sendJsonStr(req, "500 Internal Server Error", "{\"error\":\"Failed to save config\"}");
}

esp_err_t deleteJson(httpd_req_t* req) {
    std::string path = uriPath(req);
    if (path.find("configs") == std::string::npos) return sendJsonStr(req, "404 Not Found", "{\"error\":\"Unknown endpoint\"}");
    std::string id;
    if (!queryParam(req, "id", id)) return sendJsonStr(req, "400 Bad Request", "{\"error\":\"Missing required parameter: id\"}");
    if (deleteConfig(id)) return sendJsonStr(req, "200 OK", "{\"success\":true}");
    return sendJsonStr(req, "500 Internal Server Error", "{\"error\":\"Failed to delete config\"}");
}

esp_err_t onRestart(httpd_req_t* req) {
    commonHeaders(req);
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, "Restarting...", HTTPD_RESP_USE_STRLEN);
    delay(100);
    esp_restart();
    return ESP_OK;
}

esp_err_t onOptions(httpd_req_t* req) {
    commonHeaders(req);
    httpd_resp_set_hdr(req, "Access-Control-Max-Age", "7200");
    return httpd_resp_send(req, nullptr, 0);
}

// Captive portal: iOS/Android probes any host; redirect them to our IP.
esp_err_t notFound(httpd_req_t* req, httpd_err_code_t);
// Registered last for GET /*: with the OPTIONS /* handler present, an unknown GET matches the
// URI and gets a 405 instead of reaching the 404 handler, which is where the redirect lives.
esp_err_t catchAllGet(httpd_req_t* req) { return notFound(req, HTTPD_404_NOT_FOUND); }

esp_err_t notFound(httpd_req_t* req, httpd_err_code_t) {
    if (captivePortal) {
        char host[64] = {0};
        httpd_req_get_hdr_value_str(req, "Host", host, sizeof(host));
        std::string ip = Network::localIP();
        if (ip != host) {
            httpd_resp_set_status(req, "302 Found");
            httpd_resp_set_hdr(req, "Location", ("http://" + ip + "/").c_str());
            return httpd_resp_send(req, nullptr, 0);
        }
    }
    commonHeaders(req);
    httpd_resp_set_status(req, "404 Not Found");
    httpd_resp_set_type(req, "text/plain");
    return httpd_resp_send(req, "404", HTTPD_RESP_USE_STRLEN);
}

// --- websocket ---------------------------------------------------------------------------------

std::string stateJson() {
    DynamicJsonDocument doc(256);
    JsonObject root = doc.to<JsonObject>();
    serializeState(root);
    serializeInfo(root);
    std::string out;
    serializeJson(doc, out);
    return out;
}

// Runs on the httpd task (queued from wherever SendState was called).
void wsBroadcast(void* arg) {
    std::unique_ptr<std::string> msg((std::string*)arg);
    if (!server || !wsEnabled) return;
    size_t n = CONFIG_LWIP_MAX_SOCKETS;
    int fds[CONFIG_LWIP_MAX_SOCKETS];
    if (httpd_get_client_list(server, &n, fds) != ESP_OK) return;
    for (size_t i = 0; i < n; i++) {
        if (httpd_ws_get_fd_info(server, fds[i]) != HTTPD_WS_CLIENT_WEBSOCKET) continue;
        httpd_ws_frame_t frame = {};
        frame.type = HTTPD_WS_TYPE_TEXT;
        frame.payload = (uint8_t*)msg->c_str();
        frame.len = msg->size();
        httpd_ws_send_frame_async(server, fds[i], &frame);
    }
}

esp_err_t wsHandler(httpd_req_t* req) {
    if (req->method == HTTP_GET) {  // handshake done; push current state
        if (!wsEnabled) return ESP_FAIL;
        httpd_ws_frame_t frame = {};
        std::string msg = stateJson();
        frame.type = HTTPD_WS_TYPE_TEXT;
        frame.payload = (uint8_t*)msg.c_str();
        frame.len = msg.size();
        return httpd_ws_send_frame(req, &frame);
    }
    httpd_ws_frame_t frame = {};
    frame.type = HTTPD_WS_TYPE_TEXT;
    if (httpd_ws_recv_frame(req, &frame, 0) != ESP_OK) return ESP_FAIL;
    if (frame.len == 0 || frame.len > 512) return ESP_OK;
    std::unique_ptr<uint8_t[]> buf(new (std::nothrow) uint8_t[frame.len + 1]);
    if (!buf) return ESP_FAIL;
    frame.payload = buf.get();
    if (httpd_ws_recv_frame(req, &frame, frame.len) != ESP_OK) return ESP_FAIL;
    if (frame.type != HTTPD_WS_TYPE_TEXT) return ESP_OK;
    DynamicJsonDocument doc(256);
    if (deserializeJson(doc, buf.get(), frame.len)) return ESP_OK;
    JsonObject root = doc.as<JsonObject>();
    if (root.isNull() || !root.containsKey("command")) return ESP_OK;
    std::string command = root["command"].as<std::string>();
    std::string payload = root.containsKey("payload") ? root["payload"].as<std::string>() : "";
    Enrollment::Command(command, payload);
    return ESP_OK;
}

void on(const char* uri, httpd_method_t method, esp_err_t (*handler)(httpd_req_t*)) {
    httpd_uri_t u = {};
    u.uri = uri;
    u.method = method;
    u.handler = handler;
    httpd_register_uri_handler(server, &u);
}
}  // namespace

void commonHeaders(httpd_req_t* req) {
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");
    httpd_resp_set_hdr(req, "X-Content-Type-Options", "nosniff");
    httpd_resp_set_hdr(req, "Referrer-Policy", "no-referrer");
    httpd_resp_set_hdr(req, "Permissions-Policy", "accelerometer=(), camera=(), geolocation=(), gyroscope=(), microphone=(), usb=()");
}

esp_err_t serveStatic(httpd_req_t* req, const char* contentType, const uint8_t* data, size_t len, bool gzip, bool immutable) {
    commonHeaders(req);
    httpd_resp_set_type(req, contentType);
    if (gzip) httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    if (immutable) httpd_resp_set_hdr(req, "Cache-Control", "public, max-age=31536000, immutable");
    return httpd_resp_send(req, (const char*)data, len);
}

void registerGet(httpd_handle_t s, const char* uri, esp_err_t (*handler)(httpd_req_t*)) {
    httpd_uri_t u = {};
    u.uri = uri;
    u.method = HTTP_GET;
    u.handler = handler;
    httpd_register_uri_handler(s, &u);
}

bool readBody(httpd_req_t* req, std::string& body, size_t cap) {
    if (req->content_len > cap) return false;
    body.resize(req->content_len);
    size_t got = 0;
    while (got < req->content_len) {
        int r = httpd_req_recv(req, &body[got], req->content_len - got);
        if (r <= 0) return false;
        got += r;
    }
    return true;
}

std::string urlDecode(const char* s, size_t len) {
    std::string out;
    out.reserve(len);
    for (size_t i = 0; i < len; i++) {
        if (s[i] == '+') out += ' ';
        else if (s[i] == '%' && i + 2 < len + 1 && isxdigit((unsigned char)s[i + 1]) && isxdigit((unsigned char)s[i + 2])) {
            char hex[3] = {s[i + 1], s[i + 2], 0};
            out += (char)strtol(hex, nullptr, 16);
            i += 2;
        } else out += s[i];
    }
    return out;
}

// Also accepts a bare key ("?showAll"), which httpd_query_key_value does not.
bool queryParam(httpd_req_t* req, const char* key, std::string& value) {
    size_t qlen = httpd_req_get_url_query_len(req);
    if (qlen == 0) return false;
    std::string q(qlen + 1, '\0');
    if (httpd_req_get_url_query_str(req, &q[0], q.size()) != ESP_OK) return false;
    q.resize(strlen(q.c_str()));
    size_t pos = 0;
    while (pos <= q.size()) {
        size_t amp = q.find('&', pos);
        std::string pair = q.substr(pos, amp == std::string::npos ? std::string::npos : amp - pos);
        size_t eq = pair.find('=');
        if (urlDecode(pair.c_str(), eq == std::string::npos ? pair.size() : eq) == key) {
            value = eq == std::string::npos ? "" : urlDecode(pair.c_str() + eq + 1, pair.size() - eq - 1);
            return true;
        }
        if (amp == std::string::npos) break;
        pos = amp + 1;
    }
    return false;
}

void Init(bool captive) {
    if (server) return;
    captivePortal = captive;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 5120;
    config.max_uri_handlers = 48;
    config.max_open_sockets = 7;
    config.lru_purge_enable = true;
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.core_id = 0;
    if (httpd_start(&server, &config) != ESP_OK) {
        Log.println("Failed to start web server");
        server = nullptr;
        return;
    }
    httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, notFound);

    on("/ws", HTTP_GET, wsHandler);
    {
        httpd_uri_t u = {};
        u.uri = "/ws";
        u.method = HTTP_GET;
        u.handler = wsHandler;
        u.is_websocket = true;
        httpd_unregister_uri_handler(server, "/ws", HTTP_GET);
        httpd_register_uri_handler(server, &u);
    }
    on("/json*", HTTP_GET, serveJson);
    on("/json*", HTTP_POST, postJson);
    on("/json*", HTTP_DELETE, deleteJson);
    on("/restart", HTTP_POST, onRestart);
    on("/reboot", HTTP_POST, onRestart);
    on("/*", HTTP_OPTIONS, onOptions);
    Settings::registerHttp(server);
    setupRoutes(server);  // from ui_routes.h
    on("/*", HTTP_GET, catchAllGet);
}

void Loop() {}

void UpdateStart() {
    wsEnabled = false;
    SendState();
}

void UpdateEnd() { wsEnabled = true; }

void SendState() {
    if (!server) return;
    auto* msg = new (std::nothrow) std::string(stateJson());
    if (!msg) return;
    if (httpd_queue_work(server, wsBroadcast, msg) != ESP_OK) delete msg;
}

}  // namespace HttpWebServer
