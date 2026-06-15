/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,138 bytes
 * ui_html: 4,501 bytes
 * ui_app_immutable_entry_js: 67,441 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 98,126 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DPaphot8.css", HTTP_GET, serveAppImmutableAssetsInternalDPaphot8Css);
    server->on("/app/immutable/assets/start.DPaphot8.css", HTTP_GET, serveAppImmutableAssetsStartDPaphot8Css);
    server->on("/app/immutable/entry/app.Bsn-HQei.js", HTTP_GET, serveAppImmutableEntryAppBsnHQeiJs);
    server->on("/app/immutable/entry/start.DZoq1z3z.js", HTTP_GET, serveAppImmutableEntryStartDZoq1z3zJs);
    server->on("/app/immutable/nodes/0.CuS6zPpw.js", HTTP_GET, serveAppImmutableNodes_0CuS6zPpwJs);
    server->on("/app/immutable/nodes/1.KZsmYZbp.js", HTTP_GET, serveAppImmutableNodes_1KZsmYZbpJs);
    server->on("/app/immutable/nodes/2.C3XE0mZ4.js", HTTP_GET, serveAppImmutableNodes_2C3Xe0mZ4Js);
    server->on("/app/immutable/nodes/3.BAiFz73W.js", HTTP_GET, serveAppImmutableNodes_3BAiFz73WJs);
    server->on("/app/immutable/nodes/4.DBegW7nd.js", HTTP_GET, serveAppImmutableNodes_4DBegW7ndJs);
    server->on("/app/immutable/nodes/5.DZz5-b0D.js", HTTP_GET, serveAppImmutableNodes_5DZz5B0DJs);
    server->on("/app/immutable/nodes/6.DPK3Lpx-.js", HTTP_GET, serveAppImmutableNodes_6Dpk3LpxJs);
    server->on("/app/immutable/nodes/7.DC6-boRs.js", HTTP_GET, serveAppImmutableNodes_7Dc6BoRsJs);
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
