/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,291 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 44,978 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 59,494 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.L4iuwCjw.css", HTTP_GET, serveAppImmutableAssetsInternalL4iuwCjwCss);
    server->on("/app/immutable/assets/start.B3RPpGV3.css", HTTP_GET, serveAppImmutableAssetsStartB3RPpGv3Css);
    server->on("/app/immutable/entry/app.DXTGt8nI.js", HTTP_GET, serveAppImmutableEntryAppDxtGt8nIJs);
    server->on("/app/immutable/entry/start.B0MYzVnx.js", HTTP_GET, serveAppImmutableEntryStartB0MYzVnxJs);
    server->on("/app/immutable/nodes/0.3Ec3yt57.js", HTTP_GET, serveAppImmutableNodes_0_3Ec3yt57Js);
    server->on("/app/immutable/nodes/1.DKGl9xCu.js", HTTP_GET, serveAppImmutableNodes_1DkGl9xCuJs);
    server->on("/app/immutable/nodes/2.Buzj-jDK.js", HTTP_GET, serveAppImmutableNodes_2BuzjJDkJs);
    server->on("/app/immutable/nodes/3.DMRNBPyj.js", HTTP_GET, serveAppImmutableNodes_3DmrnbPyjJs);
    server->on("/app/immutable/nodes/4.BATWPxc4.js", HTTP_GET, serveAppImmutableNodes_4BatwPxc4Js);
    server->on("/app/immutable/nodes/5.BrE20iN8.js", HTTP_GET, serveAppImmutableNodes_5BrE20iN8Js);
    server->on("/app/immutable/nodes/6.CCI7jqM8.js", HTTP_GET, serveAppImmutableNodes_6Cci7jqM8Js);
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
