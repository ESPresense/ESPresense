/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,365 bytes
 * ui_app_immutable_chunks_js: 44,304 bytes
 * ui_app_immutable_entry_js: 243 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 57,725 bytes
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
    server->on("/app/immutable/chunks/CFb0YBsC.js", HTTP_GET, serveAppImmutableChunksCFb0YBsCJs);
    server->on("/app/immutable/entry/app.CIMDWf5R.js", HTTP_GET, serveAppImmutableEntryAppCimdWf5RJs);
    server->on("/app/immutable/entry/start.7tE2sxOU.js", HTTP_GET, serveAppImmutableEntryStart_7tE2sxOuJs);
    server->on("/app/immutable/nodes/0.pQHOfAKv.js", HTTP_GET, serveAppImmutableNodes_0PQhOfAKvJs);
    server->on("/app/immutable/nodes/1.CAZCqBmj.js", HTTP_GET, serveAppImmutableNodes_1CazCqBmjJs);
    server->on("/app/immutable/nodes/2.CSTnzTD0.js", HTTP_GET, serveAppImmutableNodes_2CsTnzTd0Js);
    server->on("/app/immutable/nodes/3.BL6-sP4i.js", HTTP_GET, serveAppImmutableNodes_3Bl6SP4iJs);
    server->on("/app/immutable/nodes/4.BTuz8_Y2.js", HTTP_GET, serveAppImmutableNodes_4BTuz8Y2Js);
    server->on("/app/immutable/nodes/5.CK1cbrhO.js", HTTP_GET, serveAppImmutableNodes_5Ck1cbrhOJs);
    server->on("/app/immutable/nodes/6.C3lwECUV.js", HTTP_GET, serveAppImmutableNodes_6C3lwEcuvJs);
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
