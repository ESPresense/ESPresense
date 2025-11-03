/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,367 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 48,543 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 63,135 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.C8cCt54N.css", HTTP_GET, serveAppImmutableAssetsInternalC8cCt54NCss);
    server->on("/app/immutable/assets/start.CB0Tr19N.css", HTTP_GET, serveAppImmutableAssetsStartCb0Tr19NCss);
    server->on("/app/immutable/entry/app.DWt04WxU.js", HTTP_GET, serveAppImmutableEntryAppDWt04WxUJs);
    server->on("/app/immutable/entry/start.C0g94Sf6.js", HTTP_GET, serveAppImmutableEntryStartC0g94Sf6Js);
    server->on("/app/immutable/nodes/0.D78c5qYf.js", HTTP_GET, serveAppImmutableNodes_0D78c5qYfJs);
    server->on("/app/immutable/nodes/1.DnQRUZxo.js", HTTP_GET, serveAppImmutableNodes_1DnQruZxoJs);
    server->on("/app/immutable/nodes/2.DhIKZQUA.js", HTTP_GET, serveAppImmutableNodes_2DhIkzquaJs);
    server->on("/app/immutable/nodes/3.CH7hXg3V.js", HTTP_GET, serveAppImmutableNodes_3Ch7hXg3VJs);
    server->on("/app/immutable/nodes/4.DKwEAMCA.js", HTTP_GET, serveAppImmutableNodes_4DKwEamcaJs);
    server->on("/app/immutable/nodes/5.CDrHCO2X.js", HTTP_GET, serveAppImmutableNodes_5CDrHco2XJs);
    server->on("/app/immutable/nodes/6.D0lGA0y5.js", HTTP_GET, serveAppImmutableNodes_6D0lGa0y5Js);
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
