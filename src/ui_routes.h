/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,355 bytes
 * ui_app_immutable_chunks_js: 44,420 bytes
 * ui_app_immutable_entry_js: 245 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 57,833 bytes
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
    server->on("/app/immutable/chunks/M8R_kpuu.js", HTTP_GET, serveAppImmutableChunksM8RKpuuJs);
    server->on("/app/immutable/entry/app.T6jopGAh.js", HTTP_GET, serveAppImmutableEntryAppT6jopGAhJs);
    server->on("/app/immutable/entry/start.CCdb6kD9.js", HTTP_GET, serveAppImmutableEntryStartCCdb6kD9Js);
    server->on("/app/immutable/nodes/0.CeP4w5Xx.js", HTTP_GET, serveAppImmutableNodes_0CeP4w5XxJs);
    server->on("/app/immutable/nodes/1.kRtxueHD.js", HTTP_GET, serveAppImmutableNodes_1KRtxueHdJs);
    server->on("/app/immutable/nodes/2.B94fa7sh.js", HTTP_GET, serveAppImmutableNodes_2B94fa7shJs);
    server->on("/app/immutable/nodes/3.DDBpbIar.js", HTTP_GET, serveAppImmutableNodes_3DdBpbIarJs);
    server->on("/app/immutable/nodes/4.B0GO167J.js", HTTP_GET, serveAppImmutableNodes_4B0Go167JJs);
    server->on("/app/immutable/nodes/5.ilMHURb9.js", HTTP_GET, serveAppImmutableNodes_5IlMhuRb9Js);
    server->on("/app/immutable/nodes/6.DlxYHuoz.js", HTTP_GET, serveAppImmutableNodes_6DlxYHuozJs);
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
