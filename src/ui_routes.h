/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,338 bytes
 * ui_app_immutable_entry_js: 63,676 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 93,936 bytes
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
    server->on("/app/immutable/entry/app.JAhowAhR.js", HTTP_GET, serveAppImmutableEntryAppJAhowAhRJs);
    server->on("/app/immutable/entry/start.CAyxKNiE.js", HTTP_GET, serveAppImmutableEntryStartCAyxKNiEJs);
    server->on("/app/immutable/nodes/0.B9yimWqS.js", HTTP_GET, serveAppImmutableNodes_0B9yimWqSJs);
    server->on("/app/immutable/nodes/1.DpxQZqkc.js", HTTP_GET, serveAppImmutableNodes_1DpxQZqkcJs);
    server->on("/app/immutable/nodes/2.NBcSGmAv.js", HTTP_GET, serveAppImmutableNodes_2NBcSGmAvJs);
    server->on("/app/immutable/nodes/3.BXpCw80n.js", HTTP_GET, serveAppImmutableNodes_3BXpCw80nJs);
    server->on("/app/immutable/nodes/4.CkaXoGkv.js", HTTP_GET, serveAppImmutableNodes_4CkaXoGkvJs);
    server->on("/app/immutable/nodes/5.D3vMzMEL.js", HTTP_GET, serveAppImmutableNodes_5D3vMzMelJs);
    server->on("/app/immutable/nodes/6.k0aoAMPu.js", HTTP_GET, serveAppImmutableNodes_6K0aoAmPuJs);
    server->on("/app/immutable/nodes/7.COVi4aSX.js", HTTP_GET, serveAppImmutableNodes_7CoVi4aSxJs);
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
