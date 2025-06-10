/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 11,107 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 45,219 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 60,546 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CQIPqu7U.css", HTTP_GET, serveAppImmutableAssetsInternalCqiPqu7UCss);
    server->on("/app/immutable/assets/start.BGksjXtY.css", HTTP_GET, serveAppImmutableAssetsStartBGksjXtYCss);
    server->on("/app/immutable/entry/app.D7CUJsL0.js", HTTP_GET, serveAppImmutableEntryAppD7CuJsL0Js);
    server->on("/app/immutable/entry/start.CrfDHbtS.js", HTTP_GET, serveAppImmutableEntryStartCrfDHbtSJs);
    server->on("/app/immutable/nodes/0.CyaxyLuw.js", HTTP_GET, serveAppImmutableNodes_0CyaxyLuwJs);
    server->on("/app/immutable/nodes/1.DP_lnY3l.js", HTTP_GET, serveAppImmutableNodes_1DpLnY3lJs);
    server->on("/app/immutable/nodes/2.DboRoVuo.js", HTTP_GET, serveAppImmutableNodes_2DboRoVuoJs);
    server->on("/app/immutable/nodes/3.C-cOTdq3.js", HTTP_GET, serveAppImmutableNodes_3CCOTdq3Js);
    server->on("/app/immutable/nodes/4.DQatR8NO.js", HTTP_GET, serveAppImmutableNodes_4DQatR8NoJs);
    server->on("/app/immutable/nodes/5.4_PYVEHH.js", HTTP_GET, serveAppImmutableNodes_5_4PyvehhJs);
    server->on("/app/immutable/nodes/6.sBIObEB1.js", HTTP_GET, serveAppImmutableNodes_6SBiObEb1Js);
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
