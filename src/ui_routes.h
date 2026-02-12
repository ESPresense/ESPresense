/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,332 bytes
 * ui_app_immutable_entry_js: 63,769 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,023 bytes
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
    server->on("/app/immutable/entry/app.DIn-AK8O.js", HTTP_GET, serveAppImmutableEntryAppDInAk8OJs);
    server->on("/app/immutable/entry/start.nbWAUCiX.js", HTTP_GET, serveAppImmutableEntryStartNbWauCiXJs);
    server->on("/app/immutable/nodes/0.Cy3ycnaS.js", HTTP_GET, serveAppImmutableNodes_0Cy3ycnaSJs);
    server->on("/app/immutable/nodes/1.BhBLt3oW.js", HTTP_GET, serveAppImmutableNodes_1BhBLt3oWJs);
    server->on("/app/immutable/nodes/2.CtyFQShG.js", HTTP_GET, serveAppImmutableNodes_2CtyFqShGJs);
    server->on("/app/immutable/nodes/3.DjtKg3Cn.js", HTTP_GET, serveAppImmutableNodes_3DjtKg3CnJs);
    server->on("/app/immutable/nodes/4.BZxqWeXP.js", HTTP_GET, serveAppImmutableNodes_4BZxqWeXpJs);
    server->on("/app/immutable/nodes/5.nM-AaRml.js", HTTP_GET, serveAppImmutableNodes_5NMAaRmlJs);
    server->on("/app/immutable/nodes/6.Cb6T7b_r.js", HTTP_GET, serveAppImmutableNodes_6Cb6T7bRJs);
    server->on("/app/immutable/nodes/7.Dp07-2hh.js", HTTP_GET, serveAppImmutableNodes_7Dp07_2hhJs);
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
