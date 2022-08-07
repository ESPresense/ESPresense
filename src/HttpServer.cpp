#include "MotionSensors.h"
#include <AsyncWiFiSettings.h>
#include <AsyncMqttClient.h>
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"
#include "defaults.h"
#include "GUI.h"
#include "HttpServer.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "ui_bundle_css.h"
#include "ui_index_html.h"
#include "ui_index_js.h"

namespace HttpServer
{
    void serializeConfigs(JsonObject& root)
    {
        JsonArray devices = root.createNestedArray("configs");

        auto f = BleFingerprintCollection::getConfigs();
        for (auto it = f.begin(); it != f.end(); ++it)
        {
            JsonObject node = devices.createNestedObject();
            node["id"]=it->id;
            node["alias"] = it->alias;
            node["name"]=it->name;
            node["rss@1m"]=it->calRssi;
        }
    }

    void serializeDevices(JsonObject& root)
    {
        JsonArray devices = root.createNestedArray("devices");

        auto f = fingerprints.getCopy();
        for (auto it = f.begin(); it != f.end(); ++it)
        {
            if ((*it)->getVisible()) {
                JsonObject node = devices.createNestedObject();
                (*it)->fill(&node);
            }
        }
    }

    bool servingJson = false;
    void serveJson(AsyncWebServerRequest* request) {
        if (servingJson) request->send(429, "Too Many Requests", "Too Many Requests");
        servingJson = true;
        const String& url = request->url();
        short subJson = 0;
        if (url.indexOf("devices") > 0) subJson = 1;
        if (url.indexOf("configs") > 0) subJson = 2;

        AsyncJsonResponse* response = new AsyncJsonResponse(false, JSON_BUFFER_SIZE);
        JsonObject root = response->getRoot();
        root["room"] = room;
        switch (subJson)
        {
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

    void serializeConfig(){};
    void Init(AsyncWebServer *server)
    {
        DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Origin"), "*");
        DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Methods"), "*");
        DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Headers"), "*");

        server->on("/", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
            AsyncWebServerResponse *response = request->beginResponse(200);
            response->addHeader(F("Access-Control-Max-Age"), F("7200"));
            request->send(response);
        });

        server->on("/devices", HTTP_GET, serveIndexHtml);
        server->on("/bundle.css", HTTP_GET, serveBundleCss);
        server->on("/index.js", HTTP_GET, serveIndexJs);
        server->on("/json", HTTP_GET, serveJson);

        AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/json", [](AsyncWebServerRequest *request, JsonVariant &json)
        {
            bool verboseResponse = false;
            bool isConfig = false;
            {
                DynamicJsonDocument doc(1500);
                DeserializationError error = deserializeJson(doc, (uint8_t *) (request->_tempObject));
                JsonObject root = doc.as<JsonObject>();
                if (error || root.isNull())
                {
                    request->send(400, "application/json", F("{\"error\":9}"));
                    return;
                }
                const String& url = request->url();
            }
            request->send(200, "application/json", F("{\"success\":true}"));
        });
        server->addHandler(handler);
    }
}
