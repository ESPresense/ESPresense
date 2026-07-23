/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,138 bytes
 * ui_html: 4,513 bytes
 * ui_app_immutable_entry_js: 67,454 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 98,151 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DPaphot8.css", HTTP_GET, serveAppImmutableAssetsInternalDPaphot8Css);
    server->on("/app/immutable/assets/start.DPaphot8.css", HTTP_GET, serveAppImmutableAssetsStartDPaphot8Css);
    server->on("/app/immutable/entry/app.rltFq7zP.js", HTTP_GET, serveAppImmutableEntryAppRltFq7zPJs);
    server->on("/app/immutable/entry/start.BgUA5sta.js", HTTP_GET, serveAppImmutableEntryStartBgUa5staJs);
    server->on("/app/immutable/nodes/0.C2M0EjeJ.js", HTTP_GET, serveAppImmutableNodes_0C2M0EjeJJs);
    server->on("/app/immutable/nodes/1.BRK-nBSs.js", HTTP_GET, serveAppImmutableNodes_1BrkNBSsJs);
    server->on("/app/immutable/nodes/2.Et6Glw2_.js", HTTP_GET, serveAppImmutableNodes_2Et6Glw2Js);
    server->on("/app/immutable/nodes/3.BKtCbjml.js", HTTP_GET, serveAppImmutableNodes_3BKtCbjmlJs);
    server->on("/app/immutable/nodes/4.7UqO-NAa.js", HTTP_GET, serveAppImmutableNodes_4_7UqONAaJs);
    server->on("/app/immutable/nodes/5.CYBikrFw.js", HTTP_GET, serveAppImmutableNodes_5CyBikrFwJs);
    server->on("/app/immutable/nodes/6.CxyNYfEt.js", HTTP_GET, serveAppImmutableNodes_6CxyNYfEtJs);
    server->on("/app/immutable/nodes/7.7XSBx9Eq.js", HTTP_GET, serveAppImmutableNodes_7_7XsBx9EqJs);
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
