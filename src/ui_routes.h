/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,144 bytes
 * ui_html: 4,320 bytes
 * ui_app_immutable_entry_js: 64,497 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,007 bytes
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
    server->on("/app/immutable/entry/app.BbYeIhum.js", HTTP_GET, serveAppImmutableEntryAppBbYeIhumJs);
    server->on("/app/immutable/entry/start.BOW3sI7j.js", HTTP_GET, serveAppImmutableEntryStartBow3sI7jJs);
    server->on("/app/immutable/nodes/0.qVDD8kVE.js", HTTP_GET, serveAppImmutableNodes_0QVdd8kVeJs);
    server->on("/app/immutable/nodes/1.DQ0RxIeW.js", HTTP_GET, serveAppImmutableNodes_1Dq0RxIeWJs);
    server->on("/app/immutable/nodes/2.D50uEfvG.js", HTTP_GET, serveAppImmutableNodes_2D50uEfvGJs);
    server->on("/app/immutable/nodes/3.C8rvP_-w.js", HTTP_GET, serveAppImmutableNodes_3C8rvPWJs);
    server->on("/app/immutable/nodes/4.BFnn4qWi.js", HTTP_GET, serveAppImmutableNodes_4BFnn4qWiJs);
    server->on("/app/immutable/nodes/5._spFDcJm.js", HTTP_GET, serveAppImmutableNodes_5SpFDcJmJs);
    server->on("/app/immutable/nodes/6.Ds13FRXN.js", HTTP_GET, serveAppImmutableNodes_6Ds13FrxnJs);
    server->on("/app/immutable/nodes/7.eOfj84mk.js", HTTP_GET, serveAppImmutableNodes_7EOfj84mkJs);
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
