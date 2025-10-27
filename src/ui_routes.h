/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,367 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 50,234 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 64,821 bytes
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
    server->on("/app/immutable/entry/app.BLIrtG1d.js", HTTP_GET, serveAppImmutableEntryAppBlIrtG1dJs);
    server->on("/app/immutable/entry/start.BPbJFRLE.js", HTTP_GET, serveAppImmutableEntryStartBPbJfrleJs);
    server->on("/app/immutable/nodes/0.C-yvoqTO.js", HTTP_GET, serveAppImmutableNodes_0CYvoqToJs);
    server->on("/app/immutable/nodes/1.CCiwyQ5Y.js", HTTP_GET, serveAppImmutableNodes_1CCiwyQ5YJs);
    server->on("/app/immutable/nodes/2.BZSAyZsC.js", HTTP_GET, serveAppImmutableNodes_2BzsAyZsCJs);
    server->on("/app/immutable/nodes/3.DJFz-GHN.js", HTTP_GET, serveAppImmutableNodes_3DjFzGhnJs);
    server->on("/app/immutable/nodes/4.DVpTK3rH.js", HTTP_GET, serveAppImmutableNodes_4DVpTk3rHJs);
    server->on("/app/immutable/nodes/5.C72bTxag.js", HTTP_GET, serveAppImmutableNodes_5C72bTxagJs);
    server->on("/app/immutable/nodes/6.Z6o3DROX.js", HTTP_GET, serveAppImmutableNodes_6Z6o3DroxJs);
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
