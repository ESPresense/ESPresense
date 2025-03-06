/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,360 bytes
 * ui_app_immutable_chunks_js: 44,306 bytes
 * ui_app_immutable_entry_js: 222 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 57,701 bytes
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
    server->on("/app/immutable/chunks/dbq0V9fk.js", HTTP_GET, serveAppImmutableChunksDbq0V9fkJs);
    server->on("/app/immutable/entry/app.oD47fpeL.js", HTTP_GET, serveAppImmutableEntryAppOD47fpeLJs);
    server->on("/app/immutable/entry/start.6UzJ3mXZ.js", HTTP_GET, serveAppImmutableEntryStart_6UzJ3mXzJs);
    server->on("/app/immutable/nodes/0.DaxsOZCT.js", HTTP_GET, serveAppImmutableNodes_0DaxsOzctJs);
    server->on("/app/immutable/nodes/1.Dxwzkq2L.js", HTTP_GET, serveAppImmutableNodes_1Dxwzkq2LJs);
    server->on("/app/immutable/nodes/2.CrOlYW46.js", HTTP_GET, serveAppImmutableNodes_2CrOlYw46Js);
    server->on("/app/immutable/nodes/3.D9ZvY3DH.js", HTTP_GET, serveAppImmutableNodes_3D9ZvY3DhJs);
    server->on("/app/immutable/nodes/4.BfQN1sC7.js", HTTP_GET, serveAppImmutableNodes_4BfQn1sC7Js);
    server->on("/app/immutable/nodes/5.NjmwKyy4.js", HTTP_GET, serveAppImmutableNodes_5NjmwKyy4Js);
    server->on("/app/immutable/nodes/6.DZMKhGkY.js", HTTP_GET, serveAppImmutableNodes_6DzmKhGkYJs);
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
