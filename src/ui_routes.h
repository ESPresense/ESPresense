/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,144 bytes
 * ui_html: 4,314 bytes
 * ui_app_immutable_entry_js: 64,223 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,727 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.BieM-44V.css", HTTP_GET, serveAppImmutableAssetsInternalBieM_44VCss);
    server->on("/app/immutable/assets/start.BieM-44V.css", HTTP_GET, serveAppImmutableAssetsStartBieM_44VCss);
    server->on("/app/immutable/entry/app.YjdU97r9.js", HTTP_GET, serveAppImmutableEntryAppYjdU97r9Js);
    server->on("/app/immutable/entry/start.TpsrXsRB.js", HTTP_GET, serveAppImmutableEntryStartTpsrXsRbJs);
    server->on("/app/immutable/nodes/0.BaEx9LcY.js", HTTP_GET, serveAppImmutableNodes_0BaEx9LcYJs);
    server->on("/app/immutable/nodes/1.98IlvfVn.js", HTTP_GET, serveAppImmutableNodes_1_98IlvfVnJs);
    server->on("/app/immutable/nodes/2.B1K-Uvtv.js", HTTP_GET, serveAppImmutableNodes_2B1KUvtvJs);
    server->on("/app/immutable/nodes/3.Bq0UVliy.js", HTTP_GET, serveAppImmutableNodes_3Bq0UVliyJs);
    server->on("/app/immutable/nodes/4.BOEKoSYC.js", HTTP_GET, serveAppImmutableNodes_4BoeKoSycJs);
    server->on("/app/immutable/nodes/5.DALKl8CU.js", HTTP_GET, serveAppImmutableNodes_5DalKl8CuJs);
    server->on("/app/immutable/nodes/6.BJKZZJB8.js", HTTP_GET, serveAppImmutableNodes_6Bjkzzjb8Js);
    server->on("/app/immutable/nodes/7._mnxcj37.js", HTTP_GET, serveAppImmutableNodes_7Mnxcj37Js);
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
