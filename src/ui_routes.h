/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 12,830 bytes
 * ui_app_immutable_chunks_js: 65,354 bytes
 * ui_app_immutable_entry_js: 271 bytes
 * ui_app_immutable_nodes_js: 542 bytes
 * ui_html: 4,651 bytes
 * ui_svg: 456 bytes
 * Total: 84,104 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_app_immutable_chunks_js.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_html.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/index.DTyJnhbN.css", HTTP_GET, serveAppImmutableAssetsIndexDTyJnhbNCss);
    server->on("/app/immutable/chunks/uHdxZTqW.js", HTTP_GET, serveAppImmutableChunksUHdxZTqWJs);
    server->on("/app/immutable/entry/app.BCzqVH35.js", HTTP_GET, serveAppImmutableEntryAppBCzqVh35Js);
    server->on("/app/immutable/entry/start.CBceVdH9.js", HTTP_GET, serveAppImmutableEntryStartCBceVdH9Js);
    server->on("/app/immutable/nodes/0.d-_2PaGb.js", HTTP_GET, serveAppImmutableNodes_0D_2PaGbJs);
    server->on("/app/immutable/nodes/1.DweXcI8M.js", HTTP_GET, serveAppImmutableNodes_1DweXcI8MJs);
    server->on("/app/immutable/nodes/2.DBJMYDHI.js", HTTP_GET, serveAppImmutableNodes_2DbjmydhiJs);
    server->on("/app/immutable/nodes/3.DxDIw8YM.js", HTTP_GET, serveAppImmutableNodes_3DxDIw8YmJs);
    server->on("/app/immutable/nodes/4.qVdd6Rns.js", HTTP_GET, serveAppImmutableNodes_4QVdd6RnsJs);
    server->on("/app/immutable/nodes/5.BqZ8YcYT.js", HTTP_GET, serveAppImmutableNodes_5BqZ8YcYtJs);
    server->on("/app/immutable/nodes/6.4fEkExEb.js", HTTP_GET, serveAppImmutableNodes_6_4fEkExEbJs);
    server->on("/app/immutable/nodes/7.BmtUlKf8.js", HTTP_GET, serveAppImmutableNodes_7BmtUlKf8Js);
    server->on("/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes
    server->on("/", HTTP_GET, serveIndexHtml);
    server->on("/devices", HTTP_GET, serveDevicesHtml);
    server->on("/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/hardware", HTTP_GET, serveHardwareHtml);
    server->on("/hardware.html", HTTP_GET, serveHardwareHtml);
    server->on("/network", HTTP_GET, serveNetworkHtml);
    server->on("/network.html", HTTP_GET, serveNetworkHtml);
    server->on("/settings", HTTP_GET, serveSettingsHtml);
    server->on("/settings.html", HTTP_GET, serveSettingsHtml);
}
