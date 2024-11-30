#pragma once

#include <ESPAsyncWebServer.h>
#include <Arduino.h>
#include "ui_app_css.h"
#include "ui_app_js.h"
#include "ui_root_html.h"

inline void setupUIRoutes(AsyncWebServer* server) {
    server->on("/ui/app/immutable/assets/index.BldWXf2Y.css", HTTP_GET, serveAppImmutableAssetsIndexBldWXf2YCss);
    server->on("/ui/app/immutable/chunks/index.Cfxs0Z48.js", HTTP_GET, serveAppImmutableChunksIndexCfxs0Z48Js);
    server->on("/ui/app/immutable/entry/start.7wBHyld-.js", HTTP_GET, serveAppImmutableEntryStart_7wBHyldJs);
    server->on("/ui/app/immutable/entry/app.CnArWRR5.js", HTTP_GET, serveAppImmutableEntryAppCnArWrr5Js);
    server->on("/ui/app/immutable/nodes/0.C58ML_Xs.js", HTTP_GET, serveAppImmutableNodes_0C58MlXsJs);
    server->on("/ui/app/immutable/nodes/1.C7fgOBYz.js", HTTP_GET, serveAppImmutableNodes_1C7fgObYzJs);
    server->on("/ui/app/immutable/nodes/2.CUEOGyt7.js", HTTP_GET, serveAppImmutableNodes_2CueoGyt7Js);
    server->on("/ui/app/immutable/nodes/3.lX1me3QY.js", HTTP_GET, serveAppImmutableNodes_3LX1me3QyJs);
    server->on("/ui/app/immutable/nodes/4.Cw6_1bY5.js", HTTP_GET, serveAppImmutableNodes_4Cw6_1bY5Js);
    server->on("/ui/app/immutable/nodes/5.BfFN_Km6.js", HTTP_GET, serveAppImmutableNodes_5BfFnKm6Js);

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