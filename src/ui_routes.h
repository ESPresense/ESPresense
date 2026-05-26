/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,144 bytes
 * ui_html: 4,526 bytes
 * ui_app_immutable_entry_js: 67,298 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 98,014 bytes
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
    server->on("/app/immutable/entry/app.CkDXhhPp.js", HTTP_GET, serveAppImmutableEntryAppCkDXhhPpJs);
    server->on("/app/immutable/entry/start.DB_m9gxQ.js", HTTP_GET, serveAppImmutableEntryStartDbM9gxQJs);
    server->on("/app/immutable/nodes/0.DtPEeDXn.js", HTTP_GET, serveAppImmutableNodes_0DtPEeDXnJs);
    server->on("/app/immutable/nodes/1.CV-PPYTA.js", HTTP_GET, serveAppImmutableNodes_1CvPpytaJs);
    server->on("/app/immutable/nodes/2.CnibuxqG.js", HTTP_GET, serveAppImmutableNodes_2CnibuxqGJs);
    server->on("/app/immutable/nodes/3.YD_6eVaG.js", HTTP_GET, serveAppImmutableNodes_3Yd_6eVaGJs);
    server->on("/app/immutable/nodes/4.CD2YApeG.js", HTTP_GET, serveAppImmutableNodes_4Cd2YApeGJs);
    server->on("/app/immutable/nodes/5.Cn5xuR4S.js", HTTP_GET, serveAppImmutableNodes_5Cn5xuR4SJs);
    server->on("/app/immutable/nodes/6.CQVLr2Em.js", HTTP_GET, serveAppImmutableNodes_6CqvLr2EmJs);
    server->on("/app/immutable/nodes/7.xheYLM4P.js", HTTP_GET, serveAppImmutableNodes_7XheYlm4PJs);
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
