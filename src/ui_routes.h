/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,350 bytes
 * ui_app_immutable_chunks_js: 44,725 bytes
 * ui_app_immutable_entry_js: 242 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 58,130 bytes
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
    server->on("/app/immutable/chunks/DsChtkdM.js", HTTP_GET, serveAppImmutableChunksDsChtkdMJs);
    server->on("/app/immutable/entry/app.C0qgYsfl.js", HTTP_GET, serveAppImmutableEntryAppC0qgYsflJs);
    server->on("/app/immutable/entry/start.UMh7cHAC.js", HTTP_GET, serveAppImmutableEntryStartUMh7cHacJs);
    server->on("/app/immutable/nodes/0.D-u3mYk6.js", HTTP_GET, serveAppImmutableNodes_0DU3mYk6Js);
    server->on("/app/immutable/nodes/1.Cb4UahsH.js", HTTP_GET, serveAppImmutableNodes_1Cb4UahsHJs);
    server->on("/app/immutable/nodes/2.DOi51XIe.js", HTTP_GET, serveAppImmutableNodes_2DOi51XIeJs);
    server->on("/app/immutable/nodes/3.Cww_bnlH.js", HTTP_GET, serveAppImmutableNodes_3CwwBnlHJs);
    server->on("/app/immutable/nodes/4.DndmFUVY.js", HTTP_GET, serveAppImmutableNodes_4DndmFuvyJs);
    server->on("/app/immutable/nodes/5.Ck-AXXvi.js", HTTP_GET, serveAppImmutableNodes_5CkAxXviJs);
    server->on("/app/immutable/nodes/6.DReMGqlD.js", HTTP_GET, serveAppImmutableNodes_6DReMGqlDJs);
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
