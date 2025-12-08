/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,410 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 66,392 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 81,022 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.VjTAGEOH.css", HTTP_GET, serveAppImmutableAssetsInternalVjTageohCss);
    server->on("/app/immutable/assets/start.Bt_nQDmA.css", HTTP_GET, serveAppImmutableAssetsStartBtNQDmACss);
    server->on("/app/immutable/entry/app.EqeowF5T.js", HTTP_GET, serveAppImmutableEntryAppEqeowF5TJs);
    server->on("/app/immutable/entry/start.BY1mhfpS.js", HTTP_GET, serveAppImmutableEntryStartBy1mhfpSJs);
    server->on("/app/immutable/nodes/0.Bcok5-GQ.js", HTTP_GET, serveAppImmutableNodes_0Bcok5GqJs);
    server->on("/app/immutable/nodes/1.Dc3gTf5W.js", HTTP_GET, serveAppImmutableNodes_1Dc3gTf5WJs);
    server->on("/app/immutable/nodes/2.CxAQtZ6i.js", HTTP_GET, serveAppImmutableNodes_2CxAQtZ6iJs);
    server->on("/app/immutable/nodes/3.C8iJ3NfJ.js", HTTP_GET, serveAppImmutableNodes_3C8iJ3NfJJs);
    server->on("/app/immutable/nodes/4.uPX12-NQ.js", HTTP_GET, serveAppImmutableNodes_4UPx12NqJs);
    server->on("/app/immutable/nodes/5.BlLK1aHY.js", HTTP_GET, serveAppImmutableNodes_5BlLk1aHyJs);
    server->on("/app/immutable/nodes/6.DW0qAQBn.js", HTTP_GET, serveAppImmutableNodes_6Dw0qAqBnJs);
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
