/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,370 bytes
 * ui_app_immutable_chunks_js: 44,639 bytes
 * ui_app_immutable_entry_js: 245 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 58,067 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_chunks_js.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/index.DN2mWWgh.css", HTTP_GET, serveAppImmutableAssetsIndexDn2mWWghCss);
    server->on("/app/immutable/assets/index.BfkQNPT2.css", HTTP_GET, serveAppImmutableAssetsIndexBfkQnpt2Css);
    server->on("/app/immutable/chunks/HLtIz1GX.js", HTTP_GET, serveAppImmutableChunksHLtIz1GxJs);
    server->on("/app/immutable/entry/app.B3OJyQIt.js", HTTP_GET, serveAppImmutableEntryAppB3OJyQItJs);
    server->on("/app/immutable/entry/start._Bm_RwaV.js", HTTP_GET, serveAppImmutableEntryStartBmRwaVJs);
    server->on("/app/immutable/nodes/0.IdZGUmEC.js", HTTP_GET, serveAppImmutableNodes_0IdZgUmEcJs);
    server->on("/app/immutable/nodes/1.CrcohOqO.js", HTTP_GET, serveAppImmutableNodes_1CrcohOqOJs);
    server->on("/app/immutable/nodes/2.Co_Lf3Fu.js", HTTP_GET, serveAppImmutableNodes_2CoLf3FuJs);
    server->on("/app/immutable/nodes/3.du0kE0dV.js", HTTP_GET, serveAppImmutableNodes_3Du0kE0dVJs);
    server->on("/app/immutable/nodes/4.DH_dnsgm.js", HTTP_GET, serveAppImmutableNodes_4DhDnsgmJs);
    server->on("/app/immutable/nodes/5.DivzRXyO.js", HTTP_GET, serveAppImmutableNodes_5DivzRXyOJs);
    server->on("/app/immutable/nodes/6.BBQf4-2V.js", HTTP_GET, serveAppImmutableNodes_6BbQf4_2VJs);
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
