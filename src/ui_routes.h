/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,144 bytes
 * ui_html: 4,326 bytes
 * ui_app_immutable_entry_js: 64,547 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,063 bytes
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
    server->on("/app/immutable/entry/app.DppGZ7vk.js", HTTP_GET, serveAppImmutableEntryAppDppGz7vkJs);
    server->on("/app/immutable/entry/start.DlCyMuX5.js", HTTP_GET, serveAppImmutableEntryStartDlCyMuX5Js);
    server->on("/app/immutable/nodes/0.BAFW64t_.js", HTTP_GET, serveAppImmutableNodes_0Bafw64tJs);
    server->on("/app/immutable/nodes/1.DGyVvo0K.js", HTTP_GET, serveAppImmutableNodes_1DGyVvo0KJs);
    server->on("/app/immutable/nodes/2.DtI3p2Rc.js", HTTP_GET, serveAppImmutableNodes_2DtI3p2RcJs);
    server->on("/app/immutable/nodes/3.xGNRUCU6.js", HTTP_GET, serveAppImmutableNodes_3XGnrucu6Js);
    server->on("/app/immutable/nodes/4.mQ6M4Zpd.js", HTTP_GET, serveAppImmutableNodes_4MQ6M4ZpdJs);
    server->on("/app/immutable/nodes/5.DUMfqTBO.js", HTTP_GET, serveAppImmutableNodes_5DuMfqTboJs);
    server->on("/app/immutable/nodes/6.BAFg6Jaa.js", HTTP_GET, serveAppImmutableNodes_6BaFg6JaaJs);
    server->on("/app/immutable/nodes/7.DGWJA4RC.js", HTTP_GET, serveAppImmutableNodes_7Dgwja4RcJs);
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
