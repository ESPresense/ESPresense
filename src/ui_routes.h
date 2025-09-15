/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,361 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 48,506 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 63,092 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CNjsDNpd.css", HTTP_GET, serveAppImmutableAssetsInternalCNjsDNpdCss);
    server->on("/app/immutable/assets/start.BSuysxAz.css", HTTP_GET, serveAppImmutableAssetsStartBSuysxAzCss);
    server->on("/app/immutable/entry/app.J7f2ioQ-.js", HTTP_GET, serveAppImmutableEntryAppJ7f2ioQJs);
    server->on("/app/immutable/entry/start.C4yTI5DQ.js", HTTP_GET, serveAppImmutableEntryStartC4yTi5DqJs);
    server->on("/app/immutable/nodes/0.CSXwBH0Q.js", HTTP_GET, serveAppImmutableNodes_0CsXwBh0QJs);
    server->on("/app/immutable/nodes/1.C23AN_ub.js", HTTP_GET, serveAppImmutableNodes_1C23AnUbJs);
    server->on("/app/immutable/nodes/2.B2OF8Onf.js", HTTP_GET, serveAppImmutableNodes_2B2Of8OnfJs);
    server->on("/app/immutable/nodes/3.COTyUCfC.js", HTTP_GET, serveAppImmutableNodes_3CoTyUCfCJs);
    server->on("/app/immutable/nodes/4.CXKL9AoW.js", HTTP_GET, serveAppImmutableNodes_4Cxkl9AoWJs);
    server->on("/app/immutable/nodes/5.DwuaEONl.js", HTTP_GET, serveAppImmutableNodes_5DwuaEoNlJs);
    server->on("/app/immutable/nodes/6.BLA1R06X.js", HTTP_GET, serveAppImmutableNodes_6Bla1R06XJs);
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
