/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,326 bytes
 * ui_app_immutable_entry_js: 63,939 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,187 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsInternalDAt95k88Css);
    server->on("/app/immutable/assets/start.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsStartDAt95k88Css);
    server->on("/app/immutable/entry/app.DzW8c57n.js", HTTP_GET, serveAppImmutableEntryAppDzW8c57nJs);
    server->on("/app/immutable/entry/start.CA4bD1Ao.js", HTTP_GET, serveAppImmutableEntryStartCa4bD1AoJs);
    server->on("/app/immutable/nodes/0.DOq0QJax.js", HTTP_GET, serveAppImmutableNodes_0DOq0QJaxJs);
    server->on("/app/immutable/nodes/1.DXVaFe0s.js", HTTP_GET, serveAppImmutableNodes_1DxVaFe0sJs);
    server->on("/app/immutable/nodes/2.-8CSNDTN.js", HTTP_GET, serveAppImmutableNodes_2_8CsndtnJs);
    server->on("/app/immutable/nodes/3.CHQ9Xs9S.js", HTTP_GET, serveAppImmutableNodes_3Chq9Xs9SJs);
    server->on("/app/immutable/nodes/4.gVjCkZUl.js", HTTP_GET, serveAppImmutableNodes_4GVjCkZUlJs);
    server->on("/app/immutable/nodes/5.Cq8o_rqs.js", HTTP_GET, serveAppImmutableNodes_5Cq8oRqsJs);
    server->on("/app/immutable/nodes/6.mCDv-UUH.js", HTTP_GET, serveAppImmutableNodes_6MCDvUuhJs);
    server->on("/app/immutable/nodes/7.B7KIQ673.js", HTTP_GET, serveAppImmutableNodes_7B7Kiq673Js);
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
