/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 23,452 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 63,825 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 91,507 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CGT7G6TZ.css", HTTP_GET, serveAppImmutableAssetsInternalCgt7G6TzCss);
    server->on("/app/immutable/assets/start.CGT7G6TZ.css", HTTP_GET, serveAppImmutableAssetsStartCgt7G6TzCss);
    server->on("/app/immutable/entry/app.CUgP0yz1.js", HTTP_GET, serveAppImmutableEntryAppCUgP0yz1Js);
    server->on("/app/immutable/entry/start.DQnQl9Ng.js", HTTP_GET, serveAppImmutableEntryStartDQnQl9NgJs);
    server->on("/app/immutable/nodes/0.S45aCTle.js", HTTP_GET, serveAppImmutableNodes_0S45aCTleJs);
    server->on("/app/immutable/nodes/1.B13HebNn.js", HTTP_GET, serveAppImmutableNodes_1B13HebNnJs);
    server->on("/app/immutable/nodes/2.C2GjcVlS.js", HTTP_GET, serveAppImmutableNodes_2C2GjcVlSJs);
    server->on("/app/immutable/nodes/3.AKcTLF76.js", HTTP_GET, serveAppImmutableNodes_3AKcTlf76Js);
    server->on("/app/immutable/nodes/4.DBxfTove.js", HTTP_GET, serveAppImmutableNodes_4DBxfToveJs);
    server->on("/app/immutable/nodes/5.Bz_lSH5e.js", HTTP_GET, serveAppImmutableNodes_5BzLSh5eJs);
    server->on("/app/immutable/nodes/6.uWt2wJeg.js", HTTP_GET, serveAppImmutableNodes_6UWt2wJegJs);
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
