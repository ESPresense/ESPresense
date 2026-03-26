/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,246 bytes
 * ui_html: 4,350 bytes
 * ui_app_immutable_entry_js: 64,738 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,380 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DxBQeS86.css", HTTP_GET, serveAppImmutableAssetsInternalDxBQeS86Css);
    server->on("/app/immutable/assets/start.DxBQeS86.css", HTTP_GET, serveAppImmutableAssetsStartDxBQeS86Css);
    server->on("/app/immutable/entry/app.zJA62GDg.js", HTTP_GET, serveAppImmutableEntryAppZJa62GDgJs);
    server->on("/app/immutable/entry/start.CWGAPbhA.js", HTTP_GET, serveAppImmutableEntryStartCwgaPbhAJs);
    server->on("/app/immutable/nodes/0.DoBBqsBH.js", HTTP_GET, serveAppImmutableNodes_0DoBBqsBhJs);
    server->on("/app/immutable/nodes/1.B60MFcZ3.js", HTTP_GET, serveAppImmutableNodes_1B60MFcZ3Js);
    server->on("/app/immutable/nodes/2.Dt0XvGTR.js", HTTP_GET, serveAppImmutableNodes_2Dt0XvGtrJs);
    server->on("/app/immutable/nodes/3.BoA4o-di.js", HTTP_GET, serveAppImmutableNodes_3BoA4oDiJs);
    server->on("/app/immutable/nodes/4.BPETVldV.js", HTTP_GET, serveAppImmutableNodes_4BpetVldVJs);
    server->on("/app/immutable/nodes/5.hMp2QCMZ.js", HTTP_GET, serveAppImmutableNodes_5HMp2QcmzJs);
    server->on("/app/immutable/nodes/6.Cp_OSbod.js", HTTP_GET, serveAppImmutableNodes_6CpOSbodJs);
    server->on("/app/immutable/nodes/7.DPjBlyJY.js", HTTP_GET, serveAppImmutableNodes_7DPjBlyJyJs);
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
