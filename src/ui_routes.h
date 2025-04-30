/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 44,907 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 58,005 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DN2mWWgh.css", HTTP_GET, serveAppImmutableAssetsInternalDn2mWWghCss);
    server->on("/app/immutable/assets/start.BfkQNPT2.css", HTTP_GET, serveAppImmutableAssetsStartBfkQnpt2Css);
    server->on("/app/immutable/entry/app.BOOw-yjh.js", HTTP_GET, serveAppImmutableEntryAppBoOwYjhJs);
    server->on("/app/immutable/entry/start.oH99o_PZ.js", HTTP_GET, serveAppImmutableEntryStartOH99oPzJs);
    server->on("/app/immutable/nodes/0.Cvzzuv_j.js", HTTP_GET, serveAppImmutableNodes_0CvzzuvJJs);
    server->on("/app/immutable/nodes/1._H20Nu5g.js", HTTP_GET, serveAppImmutableNodes_1H20Nu5gJs);
    server->on("/app/immutable/nodes/2.B_LcV5UP.js", HTTP_GET, serveAppImmutableNodes_2BLcV5UpJs);
    server->on("/app/immutable/nodes/3.DzD4jHq1.js", HTTP_GET, serveAppImmutableNodes_3DzD4jHq1Js);
    server->on("/app/immutable/nodes/4.Tf9gm8EJ.js", HTTP_GET, serveAppImmutableNodes_4Tf9gm8EjJs);
    server->on("/app/immutable/nodes/5.uPgUiJQW.js", HTTP_GET, serveAppImmutableNodes_5UPgUiJqwJs);
    server->on("/app/immutable/nodes/6.BjJPYOrW.js", HTTP_GET, serveAppImmutableNodes_6BjJpyOrWJs);
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
