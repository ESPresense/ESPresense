/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,361 bytes
 * ui_html: 3,240 bytes
 * ui_app_immutable_entry_js: 48,644 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 63,215 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CNjsDNpd.css", HTTP_GET, serveAppImmutableAssetsInternalCNjsDNpdCss);
    server->on("/app/immutable/assets/start.BSuysxAz.css", HTTP_GET, serveAppImmutableAssetsStartBSuysxAzCss);
    server->on("/app/immutable/entry/app.7dtHkpEW.js", HTTP_GET, serveAppImmutableEntryApp_7dtHkpEwJs);
    server->on("/app/immutable/entry/start.Dzmn941u.js", HTTP_GET, serveAppImmutableEntryStartDzmn941uJs);
    server->on("/app/immutable/nodes/0.BtPhaK5F.js", HTTP_GET, serveAppImmutableNodes_0BtPhaK5FJs);
    server->on("/app/immutable/nodes/1.CElOCYDT.js", HTTP_GET, serveAppImmutableNodes_1CElOcydtJs);
    server->on("/app/immutable/nodes/2.BDirQlug.js", HTTP_GET, serveAppImmutableNodes_2BDirQlugJs);
    server->on("/app/immutable/nodes/3.5bxRoLfr.js", HTTP_GET, serveAppImmutableNodes_3_5bxRoLfrJs);
    server->on("/app/immutable/nodes/4.B7WARWtd.js", HTTP_GET, serveAppImmutableNodes_4B7WarWtdJs);
    server->on("/app/immutable/nodes/5.DqvhvS8H.js", HTTP_GET, serveAppImmutableNodes_5DqvhvS8HJs);
    server->on("/app/immutable/nodes/6.DJpBR1do.js", HTTP_GET, serveAppImmutableNodes_6DJpBr1doJs);
    server->on("/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes
    server->on("/devices", HTTP_GET, serveDevicesHtml);
    server->on("/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/", HTTP_GET, serveIndexHtml);
    server->on("/network", HTTP_GET, serveNetworkHtml);
    server->on("/network.html", HTTP_GET, serveNetworkHtml);
    server->on("/settings", HTTP_GET, serveSettingsHtml);
    server->on("/settings.html", HTTP_GET, serveSettingsHtml);
}
