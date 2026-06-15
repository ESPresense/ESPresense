/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,374 bytes
 * ui_html: 4,715 bytes
 * ui_app_immutable_chunks_js: 65,186 bytes
 * ui_app_immutable_entry_js: 253 bytes
 * ui_app_immutable_nodes_js: 542 bytes
 * ui_svg: 456 bytes
 * Total: 96,526 bytes
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
    server->on("/app/immutable/assets/index.DBQMSuNF.css", HTTP_GET, serveAppImmutableAssetsIndexDbqmSuNfCss);
    server->on("/app/immutable/assets/index.DJ1DKDGd.css", HTTP_GET, serveAppImmutableAssetsIndexDj1DkdGdCss);
    server->on("/app/immutable/chunks/CBqLe9x-.js", HTTP_GET, serveAppImmutableChunksCBqLe9xJs);
    server->on("/app/immutable/chunks/DK3Fl9T5.js", HTTP_GET, serveAppImmutableChunksDk3Fl9T5Js);
    server->on("/app/immutable/entry/app.BVdu0F2O.js", HTTP_GET, serveAppImmutableEntryAppBVdu0F2OJs);
    server->on("/app/immutable/entry/start.DdcqQ0TE.js", HTTP_GET, serveAppImmutableEntryStartDdcqQ0TeJs);
    server->on("/app/immutable/nodes/0.iZKjktMD.js", HTTP_GET, serveAppImmutableNodes_0IZKjktMdJs);
    server->on("/app/immutable/nodes/1.C5fswAH0.js", HTTP_GET, serveAppImmutableNodes_1C5fswAh0Js);
    server->on("/app/immutable/nodes/2.CvnNF5El.js", HTTP_GET, serveAppImmutableNodes_2CvnNf5ElJs);
    server->on("/app/immutable/nodes/3.BwO7A24D.js", HTTP_GET, serveAppImmutableNodes_3BwO7A24DJs);
    server->on("/app/immutable/nodes/4.CxTg00gc.js", HTTP_GET, serveAppImmutableNodes_4CxTg00gcJs);
    server->on("/app/immutable/nodes/5.BZBBCEGh.js", HTTP_GET, serveAppImmutableNodes_5BzbbceGhJs);
    server->on("/app/immutable/nodes/6.Dh03oXF8.js", HTTP_GET, serveAppImmutableNodes_6Dh03oXf8Js);
    server->on("/app/immutable/nodes/7.CHxIVJ4Y.js", HTTP_GET, serveAppImmutableNodes_7CHxIvj4YJs);
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
