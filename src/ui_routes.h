/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,338 bytes
 * ui_app_immutable_entry_js: 63,339 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 93,599 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsInternalDAt95k88Css);
    server->on("/app/immutable/assets/start.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsStartDAt95k88Css);
    server->on("/app/immutable/entry/app.C9NbzLTS.js", HTTP_GET, serveAppImmutableEntryAppC9NbzLtsJs);
    server->on("/app/immutable/entry/start.BUobedoW.js", HTTP_GET, serveAppImmutableEntryStartBUobedoWJs);
    server->on("/app/immutable/nodes/0.Dhns5WSG.js", HTTP_GET, serveAppImmutableNodes_0Dhns5WsgJs);
    server->on("/app/immutable/nodes/1.BlRwHnMe.js", HTTP_GET, serveAppImmutableNodes_1BlRwHnMeJs);
    server->on("/app/immutable/nodes/2.BRpz4E8U.js", HTTP_GET, serveAppImmutableNodes_2BRpz4E8UJs);
    server->on("/app/immutable/nodes/3.B1ButkEA.js", HTTP_GET, serveAppImmutableNodes_3B1ButkEaJs);
    server->on("/app/immutable/nodes/4.DWVvEDsD.js", HTTP_GET, serveAppImmutableNodes_4DwVvEDsDJs);
    server->on("/app/immutable/nodes/5.DjgU4Ixh.js", HTTP_GET, serveAppImmutableNodes_5DjgU4IxhJs);
    server->on("/app/immutable/nodes/6.ZZju-ElC.js", HTTP_GET, serveAppImmutableNodes_6ZZjuElCJs);
    server->on("/app/immutable/nodes/7.CA7V6j7g.js", HTTP_GET, serveAppImmutableNodes_7Ca7V6j7gJs);
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
