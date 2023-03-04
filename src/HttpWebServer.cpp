#include "HttpWebServer.h"

#include <AsyncMqttClient.h>
#include <AsyncWiFiSettings.h>

#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "Enrollment.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"
#include "ui_bundle_css.h"
#include "ui_index_html.h"
#include "ui_index_js.h"

namespace HttpWebServer {

void serializeInfo(JsonObject &root) {
    root["room"] = room;
}

void serializeState(JsonObject &root) {
    JsonObject node = root.createNestedObject("state");
    node["enrolling"] = enrolling;
    if (enrolling) node["remaining_ms"] = enrollingEndMillis - millis();
}

void serializeConfigs(JsonObject &root) {
    JsonArray devices = root.createNestedArray("configs");

    auto f = BleFingerprintCollection::deviceConfigs;
    for (auto it = f.begin(); it != f.end(); ++it) {
        JsonObject node = devices.createNestedObject();
        node["id"] = it->id;
        node["alias"] = it->alias;
        node["name"] = it->name;
        node["rss@1m"] = it->calRssi;
    }
}

void serializeDevices(JsonObject &root) {
    JsonArray devices = root.createNestedArray("devices");

    auto f = BleFingerprintCollection::GetCopy();
    for (auto it = f.begin(); it != f.end(); ++it) {
        if ((*it)->getVisible()) {
            JsonObject node = devices.createNestedObject();
            (*it)->fill(&node);
        }
    }
}

bool servingJson = false;

void serveJson(AsyncWebServerRequest *request) {
    if (servingJson) request->send(429, "Too Many Requests", "Too Many Requests");
    servingJson = true;
    const String &url = request->url();
    short subJson = 0;
    if (url.indexOf("devices") > 0) subJson = 1;
    if (url.indexOf("configs") > 0) subJson = 2;

    AsyncJsonResponse *response = new AsyncJsonResponse(false, JSON_BUFFER_SIZE);
    JsonObject root = response->getRoot();
    serializeInfo(root);
    switch (subJson) {
        case 1:
            serializeDevices(root);
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
        DynamicJsonDocument doc(JSON_BUFFER_SIZE);
        JsonObject root = doc.to<JsonObject>();
        serializeState(root);
        serializeInfo(root);
        size_t len = measureJson(doc);
        size_t heap1 = ESP.getFreeHeap();
        buffer = ws.makeBuffer(len);  // will not allocate correct memory sometimes
        size_t heap2 = ESP.getFreeHeap();
        if (!buffer || heap1 - heap2 < len) {
            ws.closeAll(1013);     // code 1013 = temporary overload, try again later
            ws.cleanupClients(0);  // disconnect all clients to release memory
            return;                // out of memory
        }
        serializeJson(doc, (char *)buffer->get(), len + 1);
    }
    if (client) {
        client->text(buffer);
    } else {
        ws.textAll(buffer);
    }
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        sendDataWs(NULL);
    } else if (type == WS_EVT_DATA) {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        if (info->final && info->index == 0 && info->len == len) {
            if (info->opcode == WS_TEXT) {
                DynamicJsonDocument doc(JSON_BUFFER_SIZE);
                DeserializationError error = deserializeJson(doc, data, len);
                JsonObject root = doc.as<JsonObject>();
                if (error || root.isNull()) {
                    return;
                }

                if (root.containsKey("command") && root.containsKey("payload")) {
                    String command = root["command"].as<String>();
                    String payload = root["payload"].as<String>();
                    Enrollment::Command(command, payload);
                }
            }
        }
    }
}

void Init(AsyncWebServer *server) {
    DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Origin"), "*");
    DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Methods"), "*");
    DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Headers"), "*");

    server->on("/", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(200);
        response->addHeader(F("Access-Control-Max-Age"), F("7200"));
        request->send(response);
    });

    server->on("/ui/", HTTP_GET, serveIndexHtml);
    server->on("/ui/bundle.css", HTTP_GET, serveBundleCss);
    server->on("/ui/index.js", HTTP_GET, serveIndexJs);
    server->on("/ui", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("/ui/");
    });
    server->on("/json", HTTP_GET, serveJson);

    AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler(
        "/json", [](AsyncWebServerRequest *request, JsonVariant &json) {
            {
                DynamicJsonDocument doc(1500);
                DeserializationError error =
                    deserializeJson(doc, (uint8_t *)(request->_tempObject));
                JsonObject root = doc.as<JsonObject>();
                if (error || root.isNull()) {
                    request->send(400, "application/json", F("{\"error\":9}"));
                    return;
                }
                const String &url = request->url();
            }
            request->send(200, "application/json", F("{\"success\":true}"));
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

void SendState() { sendDataWs(NULL); }

}  // namespace HttpServer
