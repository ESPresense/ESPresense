/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,122 bytes
 * ui_html: 4,326 bytes
 * ui_app_immutable_entry_js: 64,132 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,626 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CKNao7CM.css", HTTP_GET, serveAppImmutableAssetsInternalCkNao7CmCss);
    server->on("/app/immutable/assets/start.CKNao7CM.css", HTTP_GET, serveAppImmutableAssetsStartCkNao7CmCss);
    server->on("/app/immutable/entry/app.h7BOaZqB.js", HTTP_GET, serveAppImmutableEntryAppH7BOaZqBJs);
    server->on("/app/immutable/entry/start.CBvSAHqj.js", HTTP_GET, serveAppImmutableEntryStartCBvSaHqjJs);
    server->on("/app/immutable/nodes/0.CpkDiUu1.js", HTTP_GET, serveAppImmutableNodes_0CpkDiUu1Js);
    server->on("/app/immutable/nodes/1.5h0LHarv.js", HTTP_GET, serveAppImmutableNodes_1_5h0LHarvJs);
    server->on("/app/immutable/nodes/2.aQ8iDOGd.js", HTTP_GET, serveAppImmutableNodes_2AQ8iDoGdJs);
    server->on("/app/immutable/nodes/3.BEIiu3Lx.js", HTTP_GET, serveAppImmutableNodes_3BeIiu3LxJs);
    server->on("/app/immutable/nodes/4.CFfLJXHe.js", HTTP_GET, serveAppImmutableNodes_4CFfLjxHeJs);
    server->on("/app/immutable/nodes/5.D4zf2Vg5.js", HTTP_GET, serveAppImmutableNodes_5D4zf2Vg5Js);
    server->on("/app/immutable/nodes/6.C_7B-t2R.js", HTTP_GET, serveAppImmutableNodes_6C_7BT2RJs);
    server->on("/app/immutable/nodes/7.Col_Zr4a.js", HTTP_GET, serveAppImmutableNodes_7ColZr4aJs);
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
