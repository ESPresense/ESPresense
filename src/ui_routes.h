/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 23,452 bytes
 * ui_html: 3,240 bytes
 * ui_app_immutable_entry_js: 63,840 bytes
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
    server->on("/app/immutable/entry/app.DsZozvHa.js", HTTP_GET, serveAppImmutableEntryAppDsZozvHaJs);
    server->on("/app/immutable/entry/start.CaBVr6n3.js", HTTP_GET, serveAppImmutableEntryStartCaBVr6n3Js);
    server->on("/app/immutable/nodes/0.C3jvZkjx.js", HTTP_GET, serveAppImmutableNodes_0C3jvZkjxJs);
    server->on("/app/immutable/nodes/1.BSPFXQZE.js", HTTP_GET, serveAppImmutableNodes_1BspfxqzeJs);
    server->on("/app/immutable/nodes/2.Tr0GruqA.js", HTTP_GET, serveAppImmutableNodes_2Tr0GruqAJs);
    server->on("/app/immutable/nodes/3.DHSEkZKs.js", HTTP_GET, serveAppImmutableNodes_3DhsEkZKsJs);
    server->on("/app/immutable/nodes/4.Ds46e5Tg.js", HTTP_GET, serveAppImmutableNodes_4Ds46e5TgJs);
    server->on("/app/immutable/nodes/5.BDoQhwg5.js", HTTP_GET, serveAppImmutableNodes_5BDoQhwg5Js);
    server->on("/app/immutable/nodes/6.NOtMFfd1.js", HTTP_GET, serveAppImmutableNodes_6NOtMFfd1Js);
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
