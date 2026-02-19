/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,332 bytes
 * ui_app_immutable_entry_js: 63,778 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,032 bytes
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
    server->on("/app/immutable/entry/app.BFzaHEBx.js", HTTP_GET, serveAppImmutableEntryAppBFzaHeBxJs);
    server->on("/app/immutable/entry/start.BTGaIyEW.js", HTTP_GET, serveAppImmutableEntryStartBtGaIyEwJs);
    server->on("/app/immutable/nodes/0.-N8CcT7d.js", HTTP_GET, serveAppImmutableNodes_0N8CcT7dJs);
    server->on("/app/immutable/nodes/1.CrIcqnv4.js", HTTP_GET, serveAppImmutableNodes_1CrIcqnv4Js);
    server->on("/app/immutable/nodes/2.8J1gCx20.js", HTTP_GET, serveAppImmutableNodes_2_8J1gCx20Js);
    server->on("/app/immutable/nodes/3.CwRjoeWT.js", HTTP_GET, serveAppImmutableNodes_3CwRjoeWtJs);
    server->on("/app/immutable/nodes/4.DdeIq7Uk.js", HTTP_GET, serveAppImmutableNodes_4DdeIq7UkJs);
    server->on("/app/immutable/nodes/5.xuohg6bQ.js", HTTP_GET, serveAppImmutableNodes_5Xuohg6bQJs);
    server->on("/app/immutable/nodes/6.BJ6-tF_y.js", HTTP_GET, serveAppImmutableNodes_6Bj6TFYJs);
    server->on("/app/immutable/nodes/7.DF0576j1.js", HTTP_GET, serveAppImmutableNodes_7Df0576j1Js);
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
