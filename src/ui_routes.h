/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,360 bytes
 * ui_app_immutable_chunks_js: 44,556 bytes
 * ui_app_immutable_entry_js: 246 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 57,975 bytes
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
    server->on("/app/immutable/chunks/C0WgTpY6.js", HTTP_GET, serveAppImmutableChunksC0WgTpY6Js);
    server->on("/app/immutable/entry/app.BRpwXRV3.js", HTTP_GET, serveAppImmutableEntryAppBRpwXrv3Js);
    server->on("/app/immutable/entry/start.BueWwrfv.js", HTTP_GET, serveAppImmutableEntryStartBueWwrfvJs);
    server->on("/app/immutable/nodes/0.D-D3lh-g.js", HTTP_GET, serveAppImmutableNodes_0DD3lhGJs);
    server->on("/app/immutable/nodes/1.De4E0PJy.js", HTTP_GET, serveAppImmutableNodes_1De4E0PJyJs);
    server->on("/app/immutable/nodes/2.Vf75G6gz.js", HTTP_GET, serveAppImmutableNodes_2Vf75G6gzJs);
    server->on("/app/immutable/nodes/3.BIp3fd9a.js", HTTP_GET, serveAppImmutableNodes_3BIp3fd9aJs);
    server->on("/app/immutable/nodes/4.Bk33JeZW.js", HTTP_GET, serveAppImmutableNodes_4Bk33JeZwJs);
    server->on("/app/immutable/nodes/5.CzgH9ol2.js", HTTP_GET, serveAppImmutableNodes_5CzgH9ol2Js);
    server->on("/app/immutable/nodes/6.BYyQUciV.js", HTTP_GET, serveAppImmutableNodes_6BYyQUciVJs);
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
