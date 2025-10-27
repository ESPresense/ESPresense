/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,367 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 47,973 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 62,570 bytes
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
    server->on("/app/immutable/entry/app.t2SM6NME.js", HTTP_GET, serveAppImmutableEntryAppT2Sm6NmeJs);
    server->on("/app/immutable/entry/start.CxDWIqB9.js", HTTP_GET, serveAppImmutableEntryStartCxDwIqB9Js);
    server->on("/app/immutable/nodes/0.DsyCG7by.js", HTTP_GET, serveAppImmutableNodes_0DsyCg7byJs);
    server->on("/app/immutable/nodes/1.C1R_wa-8.js", HTTP_GET, serveAppImmutableNodes_1C1RWa_8Js);
    server->on("/app/immutable/nodes/2.ByL-JAC2.js", HTTP_GET, serveAppImmutableNodes_2ByLJac2Js);
    server->on("/app/immutable/nodes/3.CdT2OEl6.js", HTTP_GET, serveAppImmutableNodes_3CdT2OEl6Js);
    server->on("/app/immutable/nodes/4.B5UsSk-s.js", HTTP_GET, serveAppImmutableNodes_4B5UsSkSJs);
    server->on("/app/immutable/nodes/5.Dph8xoAF.js", HTTP_GET, serveAppImmutableNodes_5Dph8xoAfJs);
    server->on("/app/immutable/nodes/6.BfP-_KIj.js", HTTP_GET, serveAppImmutableNodes_6BfPKIjJs);
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
