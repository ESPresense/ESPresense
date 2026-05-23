/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,144 bytes
 * ui_html: 4,529 bytes
 * ui_app_immutable_entry_js: 67,280 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 97,999 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.BieM-44V.css", HTTP_GET, serveAppImmutableAssetsInternalBieM_44VCss);
    server->on("/app/immutable/assets/start.BieM-44V.css", HTTP_GET, serveAppImmutableAssetsStartBieM_44VCss);
    server->on("/app/immutable/entry/app.DBUsJ8ez.js", HTTP_GET, serveAppImmutableEntryAppDbUsJ8ezJs);
    server->on("/app/immutable/entry/start.8MI4WlxY.js", HTTP_GET, serveAppImmutableEntryStart_8Mi4WlxYJs);
    server->on("/app/immutable/nodes/0.t2P7ABvz.js", HTTP_GET, serveAppImmutableNodes_0T2P7ABvzJs);
    server->on("/app/immutable/nodes/1.BLxuVvTN.js", HTTP_GET, serveAppImmutableNodes_1BLxuVvTnJs);
    server->on("/app/immutable/nodes/2.B13k9kfv.js", HTTP_GET, serveAppImmutableNodes_2B13k9kfvJs);
    server->on("/app/immutable/nodes/3.BF6JuUaG.js", HTTP_GET, serveAppImmutableNodes_3Bf6JuUaGJs);
    server->on("/app/immutable/nodes/4.BLCNNVSl.js", HTTP_GET, serveAppImmutableNodes_4BlcnnvSlJs);
    server->on("/app/immutable/nodes/5.CcV5S5nH.js", HTTP_GET, serveAppImmutableNodes_5CcV5S5nHJs);
    server->on("/app/immutable/nodes/6.JZV61_fB.js", HTTP_GET, serveAppImmutableNodes_6Jzv61FBJs);
    server->on("/app/immutable/nodes/7.V0joXU8r.js", HTTP_GET, serveAppImmutableNodes_7V0joXu8rJs);
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
