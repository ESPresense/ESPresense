/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,210 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 47,442 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 61,877 bytes
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
    server->on("/app/immutable/entry/app.BZHBzLZr.js", HTTP_GET, serveAppImmutableEntryAppBzhBzLZrJs);
    server->on("/app/immutable/entry/start.DzmQ2Vvc.js", HTTP_GET, serveAppImmutableEntryStartDzmQ2VvcJs);
    server->on("/app/immutable/nodes/0.D6uKe12h.js", HTTP_GET, serveAppImmutableNodes_0D6uKe12hJs);
    server->on("/app/immutable/nodes/1.fO26Ivm9.js", HTTP_GET, serveAppImmutableNodes_1FO26Ivm9Js);
    server->on("/app/immutable/nodes/2.D3in0JTd.js", HTTP_GET, serveAppImmutableNodes_2D3in0JTdJs);
    server->on("/app/immutable/nodes/3.Bm3vnqsC.js", HTTP_GET, serveAppImmutableNodes_3Bm3vnqsCJs);
    server->on("/app/immutable/nodes/4.CUbZbUi2.js", HTTP_GET, serveAppImmutableNodes_4CUbZbUi2Js);
    server->on("/app/immutable/nodes/5.CDoT03cQ.js", HTTP_GET, serveAppImmutableNodes_5CDoT03cQJs);
    server->on("/app/immutable/nodes/6.XmMrLPHs.js", HTTP_GET, serveAppImmutableNodes_6XmMrLpHsJs);
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
