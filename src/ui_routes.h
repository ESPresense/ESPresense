/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,367 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 48,467 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 63,064 bytes
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
    server->on("/app/immutable/entry/app.C43U6-U3.js", HTTP_GET, serveAppImmutableEntryAppC43U6U3Js);
    server->on("/app/immutable/entry/start.BpBao9UX.js", HTTP_GET, serveAppImmutableEntryStartBpBao9UxJs);
    server->on("/app/immutable/nodes/0.DDP1V3Vx.js", HTTP_GET, serveAppImmutableNodes_0Ddp1V3VxJs);
    server->on("/app/immutable/nodes/1.CVGo9MrA.js", HTTP_GET, serveAppImmutableNodes_1CvGo9MrAJs);
    server->on("/app/immutable/nodes/2.CrjxAOLB.js", HTTP_GET, serveAppImmutableNodes_2CrjxAolbJs);
    server->on("/app/immutable/nodes/3.BJGtD8iB.js", HTTP_GET, serveAppImmutableNodes_3BjGtD8iBJs);
    server->on("/app/immutable/nodes/4.CPh_8pQc.js", HTTP_GET, serveAppImmutableNodes_4CPh_8pQcJs);
    server->on("/app/immutable/nodes/5.BIS4ni0e.js", HTTP_GET, serveAppImmutableNodes_5Bis4ni0eJs);
    server->on("/app/immutable/nodes/6.GgAR56_C.js", HTTP_GET, serveAppImmutableNodes_6GgAr56CJs);
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
