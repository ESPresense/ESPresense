/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,365 bytes
 * ui_app_immutable_chunks_js: 44,359 bytes
 * ui_app_immutable_entry_js: 245 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 57,782 bytes
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
    server->on("/app/immutable/chunks/B6kFXj_B.js", HTTP_GET, serveAppImmutableChunksB6kFXjBJs);
    server->on("/app/immutable/entry/app.BYGqYQzt.js", HTTP_GET, serveAppImmutableEntryAppByGqYQztJs);
    server->on("/app/immutable/entry/start.z9-i0rsm.js", HTTP_GET, serveAppImmutableEntryStartZ9I0rsmJs);
    server->on("/app/immutable/nodes/0.5rhE0TOw.js", HTTP_GET, serveAppImmutableNodes_0_5rhE0TOwJs);
    server->on("/app/immutable/nodes/1.0Y3eUeD-.js", HTTP_GET, serveAppImmutableNodes_1_0Y3eUeDJs);
    server->on("/app/immutable/nodes/2.CrcDEs0P.js", HTTP_GET, serveAppImmutableNodes_2CrcDEs0PJs);
    server->on("/app/immutable/nodes/3.CqEWNR5a.js", HTTP_GET, serveAppImmutableNodes_3CqEwnr5aJs);
    server->on("/app/immutable/nodes/4.CVH2YnCm.js", HTTP_GET, serveAppImmutableNodes_4Cvh2YnCmJs);
    server->on("/app/immutable/nodes/5.oJbGCFUD.js", HTTP_GET, serveAppImmutableNodes_5OJbGcfudJs);
    server->on("/app/immutable/nodes/6.CU1984Gx.js", HTTP_GET, serveAppImmutableNodes_6Cu1984GxJs);
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
