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
    bool deserializeState(JsonObject root ){return false;}
    bool deserializeConfig(JsonObject doc, bool fromFS = false){return false;}
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

    void serveJson(AsyncWebServerRequest* request) {
        byte subJson = 0;
        const String& url = request->url();
        if (url.indexOf("devices") > 0) subJson = 1;
        AsyncJsonResponse* response = new AsyncJsonResponse(false, JSON_BUFFER_SIZE);
        JsonObject root = response->getRoot();
        //root["room"] = room;
        switch (subJson)
        {
            case 1:
                serializeDevices(root); break;
        }
        response->setLength();
        request->send(response);
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

        server->on("/devices", HTTP_GET, [](AsyncWebServerRequest *request)
                    { serveIndexHtml(request); });
        server->on("/bundle.css", HTTP_GET, [](AsyncWebServerRequest *request)
                    { serveBundleCss(request); });
        server->on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request)
                    { serveIndexJs(request); });
        server->on("/json", HTTP_GET, [](AsyncWebServerRequest *request)
                    { serveJson(request); });

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
                isConfig = url.indexOf("cfg") > -1;
                if (!isConfig)
                    verboseResponse = deserializeState(root);
                else
                    verboseResponse = deserializeConfig(root);
            }
            if (verboseResponse)
            {
                if (!isConfig)
                {
                    serveJson(request);
                    return; //if JSON contains "v"
                }
                else
                {
                    serializeConfig(); //Save new settings to FS
                }
            }
        request->send(200, "application/json", F("{\"success\":true}")); });
        server->addHandler(handler);
    }
}
