#pragma once

#include <ESPAsyncWebServer.h>
#include <Arduino.h>
#include "ui_app_css.h"
#include "ui_app_js.h"
#include "ui_root_html.h"

inline void setupUIRoutes(AsyncWebServer* server) {
    server->on("/ui/app/immutable/assets/index.DUGlLD2F.css", HTTP_GET, serveAppImmutableAssetsIndexDuGlLd2FCss);
    server->on("/ui/app/immutable/chunks/index.BmvMkDwf.js", HTTP_GET, serveAppImmutableChunksIndexBmvMkDwfJs);
    server->on("/ui/app/immutable/entry/start.B3BE1t1x.js", HTTP_GET, serveAppImmutableEntryStartB3Be1t1xJs);
    server->on("/ui/app/immutable/entry/app.4p5-oTvo.js", HTTP_GET, serveAppImmutableEntryApp_4p5OTvoJs);
    server->on("/ui/app/immutable/nodes/0.DbuA8h7J.js", HTTP_GET, serveAppImmutableNodes_0DbuA8h7JJs);
    server->on("/ui/app/immutable/nodes/1.BgjnE4it.js", HTTP_GET, serveAppImmutableNodes_1BgjnE4itJs);
    server->on("/ui/app/immutable/nodes/2.DfJExTbZ.js", HTTP_GET, serveAppImmutableNodes_2DfJExTbZJs);
    server->on("/ui/app/immutable/nodes/3.BvkBeeXb.js", HTTP_GET, serveAppImmutableNodes_3BvkBeeXbJs);
    server->on("/ui/app/immutable/nodes/4.DlBVN1uD.js", HTTP_GET, serveAppImmutableNodes_4DlBvn1uDJs);
    server->on("/ui/app/immutable/nodes/5.5Tt33p5p.js", HTTP_GET, serveAppImmutableNodes_5_5Tt33p5pJs);

    // HTML routes with and without .html extension
    server->on("/ui/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/ui/devices", HTTP_GET, serveDevicesHtml);
    server->on("/ui/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/ui/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/ui/index.html", HTTP_GET, serveIndexHtml);
    server->on("/ui/settings.html", HTTP_GET, serveSettingsHtml);
    server->on("/ui/settings", HTTP_GET, serveSettingsHtml);

    // Serve index.html for the root path
    server->on("/ui/", HTTP_GET, serveIndexHtml);
}