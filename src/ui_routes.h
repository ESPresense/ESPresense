/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,326 bytes
 * ui_app_immutable_entry_js: 63,687 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 93,935 bytes
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
    server->on("/app/immutable/entry/app.DzDNS1e_.js", HTTP_GET, serveAppImmutableEntryAppDzDns1eJs);
    server->on("/app/immutable/entry/start.CCyn0Seg.js", HTTP_GET, serveAppImmutableEntryStartCCyn0SegJs);
    server->on("/app/immutable/nodes/0.CTVpbJFQ.js", HTTP_GET, serveAppImmutableNodes_0CtVpbJfqJs);
    server->on("/app/immutable/nodes/1.1LBC87r_.js", HTTP_GET, serveAppImmutableNodes_1_1Lbc87rJs);
    server->on("/app/immutable/nodes/2.BtQuYNiW.js", HTTP_GET, serveAppImmutableNodes_2BtQuYNiWJs);
    server->on("/app/immutable/nodes/3.BEpX5IOA.js", HTTP_GET, serveAppImmutableNodes_3BEpX5IoaJs);
    server->on("/app/immutable/nodes/4.DEikH38Q.js", HTTP_GET, serveAppImmutableNodes_4DEikH38QJs);
    server->on("/app/immutable/nodes/5.D2xLtHpW.js", HTTP_GET, serveAppImmutableNodes_5D2xLtHpWJs);
    server->on("/app/immutable/nodes/6.DBuc2U5J.js", HTTP_GET, serveAppImmutableNodes_6DBuc2U5JJs);
    server->on("/app/immutable/nodes/7.C9fz0CRq.js", HTTP_GET, serveAppImmutableNodes_7C9fz0CRqJs);
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
