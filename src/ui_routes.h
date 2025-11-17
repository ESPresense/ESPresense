/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,571 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 66,266 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 81,057 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.wrGhOo2C.css", HTTP_GET, serveAppImmutableAssetsInternalWrGhOo2CCss);
    server->on("/app/immutable/assets/start.DOTUP31V.css", HTTP_GET, serveAppImmutableAssetsStartDotup31VCss);
    server->on("/app/immutable/entry/app.D9O-vHtR.js", HTTP_GET, serveAppImmutableEntryAppD9OVHtRJs);
    server->on("/app/immutable/entry/start.D46uwUHm.js", HTTP_GET, serveAppImmutableEntryStartD46uwUHmJs);
    server->on("/app/immutable/nodes/0.BcPSA0Wb.js", HTTP_GET, serveAppImmutableNodes_0BcPsa0WbJs);
    server->on("/app/immutable/nodes/1.BR-z7v2n.js", HTTP_GET, serveAppImmutableNodes_1BrZ7v2nJs);
    server->on("/app/immutable/nodes/2.B1TE6H1b.js", HTTP_GET, serveAppImmutableNodes_2B1Te6H1bJs);
    server->on("/app/immutable/nodes/3.Dou_zw-I.js", HTTP_GET, serveAppImmutableNodes_3DouZwIJs);
    server->on("/app/immutable/nodes/4.DVvBuZbY.js", HTTP_GET, serveAppImmutableNodes_4DVvBuZbYJs);
    server->on("/app/immutable/nodes/5.DYQGsesw.js", HTTP_GET, serveAppImmutableNodes_5DyqGseswJs);
    server->on("/app/immutable/nodes/6.D7H2AN3o.js", HTTP_GET, serveAppImmutableNodes_6D7H2An3oJs);
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
