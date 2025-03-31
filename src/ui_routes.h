/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,370 bytes
 * ui_app_immutable_chunks_js: 44,630 bytes
 * ui_app_immutable_entry_js: 246 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 58,059 bytes
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
    server->on("/app/immutable/chunks/CaHD16Zf.js", HTTP_GET, serveAppImmutableChunksCaHd16ZfJs);
    server->on("/app/immutable/entry/app.CAE7mPt8.js", HTTP_GET, serveAppImmutableEntryAppCae7mPt8Js);
    server->on("/app/immutable/entry/start.jOu6qxvN.js", HTTP_GET, serveAppImmutableEntryStartJOu6qxvNJs);
    server->on("/app/immutable/nodes/0.DhxYr9-D.js", HTTP_GET, serveAppImmutableNodes_0DhxYr9DJs);
    server->on("/app/immutable/nodes/1.B8VEkkSO.js", HTTP_GET, serveAppImmutableNodes_1B8VEkkSoJs);
    server->on("/app/immutable/nodes/2.DgF9UyyS.js", HTTP_GET, serveAppImmutableNodes_2DgF9UyySJs);
    server->on("/app/immutable/nodes/3.D6y51Lf0.js", HTTP_GET, serveAppImmutableNodes_3D6y51Lf0Js);
    server->on("/app/immutable/nodes/4.BUU5BM1k.js", HTTP_GET, serveAppImmutableNodes_4Buu5Bm1kJs);
    server->on("/app/immutable/nodes/5.BFaVt7MF.js", HTTP_GET, serveAppImmutableNodes_5BFaVt7MfJs);
    server->on("/app/immutable/nodes/6.DufJJsIx.js", HTTP_GET, serveAppImmutableNodes_6DufJJsIxJs);
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
