/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 14,520 bytes
 * ui_app_immutable_chunks_js: 66,544 bytes
 * ui_app_immutable_entry_js: 270 bytes
 * ui_app_immutable_nodes_js: 542 bytes
 * ui_html: 4,641 bytes
 * ui_svg: 456 bytes
 * Total: 86,973 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_app_immutable_chunks_js.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_html.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/index.DKQns9on.css", HTTP_GET, serveAppImmutableAssetsIndexDkQns9onCss);
    server->on("/app/immutable/chunks/B6KabNcX.js", HTTP_GET, serveAppImmutableChunksB6KabNcXJs);
    server->on("/app/immutable/entry/app.DSkqpys8.js", HTTP_GET, serveAppImmutableEntryAppDSkqpys8Js);
    server->on("/app/immutable/entry/start.DCs0dtJJ.js", HTTP_GET, serveAppImmutableEntryStartDCs0dtJjJs);
    server->on("/app/immutable/nodes/0.HPu-180P.js", HTTP_GET, serveAppImmutableNodes_0HPu_180PJs);
    server->on("/app/immutable/nodes/1.BGPmWH22.js", HTTP_GET, serveAppImmutableNodes_1BgPmWh22Js);
    server->on("/app/immutable/nodes/2.CYcavR9P.js", HTTP_GET, serveAppImmutableNodes_2CYcavR9PJs);
    server->on("/app/immutable/nodes/3.CHFzFyP3.js", HTTP_GET, serveAppImmutableNodes_3ChFzFyP3Js);
    server->on("/app/immutable/nodes/4.BPceVuzQ.js", HTTP_GET, serveAppImmutableNodes_4BPceVuzQJs);
    server->on("/app/immutable/nodes/5.BwsQFkQz.js", HTTP_GET, serveAppImmutableNodes_5BwsQFkQzJs);
    server->on("/app/immutable/nodes/6.IA991muk.js", HTTP_GET, serveAppImmutableNodes_6Ia991mukJs);
    server->on("/app/immutable/nodes/7.Am8DfozT.js", HTTP_GET, serveAppImmutableNodes_7Am8DfozTJs);
    server->on("/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes
    server->on("/", HTTP_GET, serveIndexHtml);
    server->on("/devices", HTTP_GET, serveDevicesHtml);
    server->on("/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/hardware", HTTP_GET, serveHardwareHtml);
    server->on("/hardware.html", HTTP_GET, serveHardwareHtml);
    server->on("/network", HTTP_GET, serveNetworkHtml);
    server->on("/network.html", HTTP_GET, serveNetworkHtml);
    server->on("/settings", HTTP_GET, serveSettingsHtml);
    server->on("/settings.html", HTTP_GET, serveSettingsHtml);
}
