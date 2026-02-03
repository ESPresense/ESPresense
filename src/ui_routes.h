/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,522 bytes
 * ui_html: 3,918 bytes
 * ui_app_immutable_entry_js: 62,552 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 92,038 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.Bb3R8x5k.css", HTTP_GET, serveAppImmutableAssetsInternalBb3R8x5kCss);
    server->on("/app/immutable/assets/start.Bb3R8x5k.css", HTTP_GET, serveAppImmutableAssetsStartBb3R8x5kCss);
    server->on("/app/immutable/entry/app.DEKygVvZ.js", HTTP_GET, serveAppImmutableEntryAppDeKygVvZJs);
    server->on("/app/immutable/entry/start.NFA1E3Fs.js", HTTP_GET, serveAppImmutableEntryStartNfa1E3FsJs);
    server->on("/app/immutable/nodes/0.BeUEOzO0.js", HTTP_GET, serveAppImmutableNodes_0BeUeOzO0Js);
    server->on("/app/immutable/nodes/1.Df4ykflJ.js", HTTP_GET, serveAppImmutableNodes_1Df4ykflJJs);
    server->on("/app/immutable/nodes/2.i2bysOWL.js", HTTP_GET, serveAppImmutableNodes_2I2bysOwlJs);
    server->on("/app/immutable/nodes/3.DEec7hqS.js", HTTP_GET, serveAppImmutableNodes_3DEec7hqSJs);
    server->on("/app/immutable/nodes/4.Bkx-3Oxy.js", HTTP_GET, serveAppImmutableNodes_4Bkx_3OxyJs);
    server->on("/app/immutable/nodes/5.CQKkWMJr.js", HTTP_GET, serveAppImmutableNodes_5CqKkWmJrJs);
    server->on("/app/immutable/nodes/6.Ck9wszjX.js", HTTP_GET, serveAppImmutableNodes_6Ck9wszjXJs);
    server->on("/app/immutable/nodes/7.DhfM_kvQ.js", HTTP_GET, serveAppImmutableNodes_7DhfMKvQJs);
    server->on("/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes
    server->on("/devices", HTTP_GET, serveDevicesHtml);
    server->on("/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/hardware", HTTP_GET, serveHardwareHtml);
    server->on("/hardware.html", HTTP_GET, serveHardwareHtml);
    server->on("/", HTTP_GET, serveIndexHtml);
    server->on("/network", HTTP_GET, serveNetworkHtml);
    server->on("/network.html", HTTP_GET, serveNetworkHtml);
    server->on("/settings", HTTP_GET, serveSettingsHtml);
    server->on("/settings.html", HTTP_GET, serveSettingsHtml);
}
