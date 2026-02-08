/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,936 bytes
 * ui_html: 4,338 bytes
 * ui_app_immutable_entry_js: 63,227 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 93,547 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.Bl0IsqIY.css", HTTP_GET, serveAppImmutableAssetsInternalBl0IsqIyCss);
    server->on("/app/immutable/assets/start.Bl0IsqIY.css", HTTP_GET, serveAppImmutableAssetsStartBl0IsqIyCss);
    server->on("/app/immutable/entry/app.dT2MJzA7.js", HTTP_GET, serveAppImmutableEntryAppDT2MJzA7Js);
    server->on("/app/immutable/entry/start.DVOr3Nsk.js", HTTP_GET, serveAppImmutableEntryStartDvOr3NskJs);
    server->on("/app/immutable/nodes/0.CkwmsESC.js", HTTP_GET, serveAppImmutableNodes_0CkwmsEscJs);
    server->on("/app/immutable/nodes/1.DaoL-JiI.js", HTTP_GET, serveAppImmutableNodes_1DaoLJiIJs);
    server->on("/app/immutable/nodes/2.D3k06VyL.js", HTTP_GET, serveAppImmutableNodes_2D3k06VyLJs);
    server->on("/app/immutable/nodes/3.baAfHlh2.js", HTTP_GET, serveAppImmutableNodes_3BaAfHlh2Js);
    server->on("/app/immutable/nodes/4.BwCWDo44.js", HTTP_GET, serveAppImmutableNodes_4BwCwDo44Js);
    server->on("/app/immutable/nodes/5.C-8MZtBP.js", HTTP_GET, serveAppImmutableNodes_5C_8MZtBpJs);
    server->on("/app/immutable/nodes/6.Dk2C6Kcw.js", HTTP_GET, serveAppImmutableNodes_6Dk2C6KcwJs);
    server->on("/app/immutable/nodes/7.Ba5Bsm16.js", HTTP_GET, serveAppImmutableNodes_7Ba5Bsm16Js);
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
