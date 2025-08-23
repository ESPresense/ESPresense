/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,350 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 48,417 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 62,982 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CXM_Puhr.css", HTTP_GET, serveAppImmutableAssetsInternalCxmPuhrCss);
    server->on("/app/immutable/assets/start.DgX9KEzp.css", HTTP_GET, serveAppImmutableAssetsStartDgX9KEzpCss);
    server->on("/app/immutable/entry/app.CuEGRr9C.js", HTTP_GET, serveAppImmutableEntryAppCuEgRr9CJs);
    server->on("/app/immutable/entry/start.D2EgFvzZ.js", HTTP_GET, serveAppImmutableEntryStartD2EgFvzZJs);
    server->on("/app/immutable/nodes/0.Bg3uW1sB.js", HTTP_GET, serveAppImmutableNodes_0Bg3uW1sBJs);
    server->on("/app/immutable/nodes/1.BDWfZwKs.js", HTTP_GET, serveAppImmutableNodes_1BdWfZwKsJs);
    server->on("/app/immutable/nodes/2.BMZ8Ejv8.js", HTTP_GET, serveAppImmutableNodes_2Bmz8Ejv8Js);
    server->on("/app/immutable/nodes/3.IYsr6hHy.js", HTTP_GET, serveAppImmutableNodes_3IYsr6hHyJs);
    server->on("/app/immutable/nodes/4.vW3EwCiu.js", HTTP_GET, serveAppImmutableNodes_4VW3EwCiuJs);
    server->on("/app/immutable/nodes/5.BhY8cGBw.js", HTTP_GET, serveAppImmutableNodes_5BhY8cGBwJs);
    server->on("/app/immutable/nodes/6.BnMBpsOg.js", HTTP_GET, serveAppImmutableNodes_6BnMBpsOgJs);
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
