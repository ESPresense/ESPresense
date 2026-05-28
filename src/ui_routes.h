/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,144 bytes
 * ui_html: 4,544 bytes
 * ui_app_immutable_entry_js: 67,353 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 98,087 bytes
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
    server->on("/app/immutable/entry/app.C4IG4ZIr.js", HTTP_GET, serveAppImmutableEntryAppC4Ig4ZIrJs);
    server->on("/app/immutable/entry/start.6FqDePHO.js", HTTP_GET, serveAppImmutableEntryStart_6FqDePhoJs);
    server->on("/app/immutable/nodes/0.Bx7SCmJz.js", HTTP_GET, serveAppImmutableNodes_0Bx7SCmJzJs);
    server->on("/app/immutable/nodes/1.2VGljNVI.js", HTTP_GET, serveAppImmutableNodes_1_2VGljNviJs);
    server->on("/app/immutable/nodes/2.BgdhqmER.js", HTTP_GET, serveAppImmutableNodes_2BgdhqmErJs);
    server->on("/app/immutable/nodes/3.C07HEsWv.js", HTTP_GET, serveAppImmutableNodes_3C07HEsWvJs);
    server->on("/app/immutable/nodes/4.Cvxf8jaP.js", HTTP_GET, serveAppImmutableNodes_4Cvxf8jaPJs);
    server->on("/app/immutable/nodes/5.DJjn0P2i.js", HTTP_GET, serveAppImmutableNodes_5DJjn0P2iJs);
    server->on("/app/immutable/nodes/6.B4wBCfvt.js", HTTP_GET, serveAppImmutableNodes_6B4wBCfvtJs);
    server->on("/app/immutable/nodes/7.C-1ZJTl2.js", HTTP_GET, serveAppImmutableNodes_7C_1ZjTl2Js);
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
