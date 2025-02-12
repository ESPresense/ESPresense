/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,877 bytes
 * ui_html: 3,365 bytes
 * ui_app_immutable_chunks_js: 44,310 bytes
 * ui_app_immutable_entry_js: 226 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 57,718 bytes
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
    server->on("/app/immutable/assets/index.DMaOv9Cv.css", HTTP_GET, serveAppImmutableAssetsIndexDMaOv9CvCss);
    server->on("/app/immutable/assets/index.DprRi3Fi.css", HTTP_GET, serveAppImmutableAssetsIndexDprRi3FiCss);
    server->on("/app/immutable/chunks/C65T1tGY.js", HTTP_GET, serveAppImmutableChunksC65T1tGyJs);
    server->on("/app/immutable/entry/app.6HFY-A-T.js", HTTP_GET, serveAppImmutableEntryApp_6HfyATJs);
    server->on("/app/immutable/entry/start.CGAvQuUM.js", HTTP_GET, serveAppImmutableEntryStartCgAvQuUmJs);
    server->on("/app/immutable/nodes/0.DY-NgYlJ.js", HTTP_GET, serveAppImmutableNodes_0DyNgYlJJs);
    server->on("/app/immutable/nodes/1.DxPgrTxG.js", HTTP_GET, serveAppImmutableNodes_1DxPgrTxGJs);
    server->on("/app/immutable/nodes/2.DIgYim8i.js", HTTP_GET, serveAppImmutableNodes_2DIgYim8iJs);
    server->on("/app/immutable/nodes/3.BUaCh3wz.js", HTTP_GET, serveAppImmutableNodes_3BUaCh3wzJs);
    server->on("/app/immutable/nodes/4.Cu6sDVZq.js", HTTP_GET, serveAppImmutableNodes_4Cu6sDvZqJs);
    server->on("/app/immutable/nodes/5.CDKuV8fQ.js", HTTP_GET, serveAppImmutableNodes_5CdKuV8fQJs);
    server->on("/app/immutable/nodes/6.8NFZ2qel.js", HTTP_GET, serveAppImmutableNodes_6_8Nfz2qelJs);
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
