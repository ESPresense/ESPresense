/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,320 bytes
 * ui_app_immutable_entry_js: 63,288 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 93,530 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsInternalDAt95k88Css);
    server->on("/app/immutable/assets/start.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsStartDAt95k88Css);
    server->on("/app/immutable/entry/app.ssNVAI2g.js", HTTP_GET, serveAppImmutableEntryAppSsNvai2gJs);
    server->on("/app/immutable/entry/start.KNd1bFG_.js", HTTP_GET, serveAppImmutableEntryStartKNd1bFgJs);
    server->on("/app/immutable/nodes/0.J0Qx1XKq.js", HTTP_GET, serveAppImmutableNodes_0J0Qx1XKqJs);
    server->on("/app/immutable/nodes/1.CZAotdk5.js", HTTP_GET, serveAppImmutableNodes_1CzAotdk5Js);
    server->on("/app/immutable/nodes/2.Dz27YKd4.js", HTTP_GET, serveAppImmutableNodes_2Dz27YKd4Js);
    server->on("/app/immutable/nodes/3.BQls7T6c.js", HTTP_GET, serveAppImmutableNodes_3BQls7T6cJs);
    server->on("/app/immutable/nodes/4.C_ybPzKV.js", HTTP_GET, serveAppImmutableNodes_4CYbPzKvJs);
    server->on("/app/immutable/nodes/5.DIiPiF0X.js", HTTP_GET, serveAppImmutableNodes_5DIiPiF0XJs);
    server->on("/app/immutable/nodes/6.DjOxa9PQ.js", HTTP_GET, serveAppImmutableNodes_6DjOxa9PqJs);
    server->on("/app/immutable/nodes/7.BJXWgR-V.js", HTTP_GET, serveAppImmutableNodes_7BjxWgRVJs);
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
