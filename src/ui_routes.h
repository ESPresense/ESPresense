/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,210 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 47,510 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 61,950 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.R2nWuKlp.css", HTTP_GET, serveAppImmutableAssetsInternalR2nWuKlpCss);
    server->on("/app/immutable/assets/start.CquYjHqP.css", HTTP_GET, serveAppImmutableAssetsStartCquYjHqPCss);
    server->on("/app/immutable/entry/app.BT64Ukmk.js", HTTP_GET, serveAppImmutableEntryAppBt64UkmkJs);
    server->on("/app/immutable/entry/start.B9A4mwEQ.js", HTTP_GET, serveAppImmutableEntryStartB9A4mwEqJs);
    server->on("/app/immutable/nodes/0.Bcihvq33.js", HTTP_GET, serveAppImmutableNodes_0Bcihvq33Js);
    server->on("/app/immutable/nodes/1.EhjMLBpS.js", HTTP_GET, serveAppImmutableNodes_1EhjMlBpSJs);
    server->on("/app/immutable/nodes/2.CTe_3LF6.js", HTTP_GET, serveAppImmutableNodes_2CTe_3Lf6Js);
    server->on("/app/immutable/nodes/3.CmRKYfa9.js", HTTP_GET, serveAppImmutableNodes_3CmRkYfa9Js);
    server->on("/app/immutable/nodes/4.BvizKJlz.js", HTTP_GET, serveAppImmutableNodes_4BvizKJlzJs);
    server->on("/app/immutable/nodes/5.qICHanin.js", HTTP_GET, serveAppImmutableNodes_5QIcHaninJs);
    server->on("/app/immutable/nodes/6.B5-qgbCe.js", HTTP_GET, serveAppImmutableNodes_6B5QgbCeJs);
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
