/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,410 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 66,320 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 80,955 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.VjTAGEOH.css", HTTP_GET, serveAppImmutableAssetsInternalVjTageohCss);
    server->on("/app/immutable/assets/start.Bt_nQDmA.css", HTTP_GET, serveAppImmutableAssetsStartBtNQDmACss);
    server->on("/app/immutable/entry/app.B1DmRMB1.js", HTTP_GET, serveAppImmutableEntryAppB1DmRmb1Js);
    server->on("/app/immutable/entry/start.DT1qlW-J.js", HTTP_GET, serveAppImmutableEntryStartDt1qlWJJs);
    server->on("/app/immutable/nodes/0.CI63is04.js", HTTP_GET, serveAppImmutableNodes_0Ci63is04Js);
    server->on("/app/immutable/nodes/1.DtJQzXcw.js", HTTP_GET, serveAppImmutableNodes_1DtJQzXcwJs);
    server->on("/app/immutable/nodes/2.DBVo6EU1.js", HTTP_GET, serveAppImmutableNodes_2DbVo6Eu1Js);
    server->on("/app/immutable/nodes/3.BAdqHkqI.js", HTTP_GET, serveAppImmutableNodes_3BAdqHkqIJs);
    server->on("/app/immutable/nodes/4.BbXW6PLr.js", HTTP_GET, serveAppImmutableNodes_4BbXw6PLrJs);
    server->on("/app/immutable/nodes/5.DOgwPyBI.js", HTTP_GET, serveAppImmutableNodes_5DOgwPyBiJs);
    server->on("/app/immutable/nodes/6.BEU0f8m3.js", HTTP_GET, serveAppImmutableNodes_6Beu0f8m3Js);
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
