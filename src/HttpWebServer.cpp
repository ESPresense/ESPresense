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
    if (!enrolledId.isEmpty()) node["enrolledId"] = enrolledId;
    if (enrolling) node["remain"] = (enrollingEndMillis - millis()) / 1000;
}

void serializeConfigs(JsonObject &root) {
    JsonArray configs = root.createNestedArray("configs");

    auto deviceConfigs = BleFingerprintCollection::deviceConfigs;
    for (auto it = deviceConfigs.begin(); it != deviceConfigs.end(); ++it) {
        const JsonObject &node = configs.createNestedObject();
        node["id"] = it->id;
        node["alias"] = it->alias;
        node["name"] = it->name;
        if (it->calRssi > -128) node["rssi@1m"] = it->calRssi;
    }
}

void serializeDevices(JsonObject &root, bool showAll) {
    JsonArray devices = root.createNestedArray("devices");

    auto f = BleFingerprintCollection::GetCopy();
    for (auto it = f.begin(); it != f.end(); ++it) {
        bool visible = (*it)->getVisible();
        if (showAll || visible) {
            JsonObject node = devices.createNestedObject();
            if ((*it)->fill(&node)) {
                if (showAll && visible) node[F("vis")] = true;
            } else
                devices.remove(devices.size() - 1);
        }
    }
}

bool servingJson = false;

void serveJson(AsyncWebServerRequest *request) {
    if (servingJson) request->send(429, "Too Many Requests", "Too Many Requests");
    servingJson = true;
    bool showAll = false;
    const String &url = request->url();
    short subJson = 0;
    if (url.indexOf("devices") > 0) subJson = 1;
    if (url.indexOf("configs") > 0) subJson = 2;

    int paramsNr = request->params();
    for (int i = 0; i < paramsNr; i++) {
        AsyncWebParameter *p = request->getParam(i);
        if (p->name() == "showAll") showAll = true;
    }

    auto *response = new AsyncJsonResponse(false, JSON_BUFFER_SIZE);
    JsonObject root = response->getRoot();
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

void SendState() { sendDataWs(nullptr); }

}  // namespace HttpWebServer
