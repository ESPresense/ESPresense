/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,350 bytes
 * ui_app_immutable_chunks_js: 44,297 bytes
 * ui_app_immutable_entry_js: 224 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 57,684 bytes
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
    server->on("/app/immutable/chunks/of6r8KH0.js", HTTP_GET, serveAppImmutableChunksOf6r8Kh0Js);
    server->on("/app/immutable/entry/app.CJl_hGX0.js", HTTP_GET, serveAppImmutableEntryAppCJlHGx0Js);
    server->on("/app/immutable/entry/start.CpjOrpi6.js", HTTP_GET, serveAppImmutableEntryStartCpjOrpi6Js);
    server->on("/app/immutable/nodes/0.QPcleJqP.js", HTTP_GET, serveAppImmutableNodes_0QPcleJqPJs);
    server->on("/app/immutable/nodes/1.DQd0o01I.js", HTTP_GET, serveAppImmutableNodes_1DQd0o01IJs);
    server->on("/app/immutable/nodes/2.CiYz4RFk.js", HTTP_GET, serveAppImmutableNodes_2CiYz4RFkJs);
    server->on("/app/immutable/nodes/3.DE5Dlmym.js", HTTP_GET, serveAppImmutableNodes_3De5DlmymJs);
    server->on("/app/immutable/nodes/4.DOenAE0i.js", HTTP_GET, serveAppImmutableNodes_4DOenAe0iJs);
    server->on("/app/immutable/nodes/5.qOfus1N-.js", HTTP_GET, serveAppImmutableNodes_5QOfus1NJs);
    server->on("/app/immutable/nodes/6.DdRQprSG.js", HTTP_GET, serveAppImmutableNodes_6DdRQprSgJs);
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
