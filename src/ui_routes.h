/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,210 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 47,426 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 61,856 bytes
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
    server->on("/app/immutable/entry/app.Cddl1RV_.js", HTTP_GET, serveAppImmutableEntryAppCddl1RvJs);
    server->on("/app/immutable/entry/start.CIVXh7Jm.js", HTTP_GET, serveAppImmutableEntryStartCivXh7JmJs);
    server->on("/app/immutable/nodes/0.DQ30Sgnp.js", HTTP_GET, serveAppImmutableNodes_0Dq30SgnpJs);
    server->on("/app/immutable/nodes/1.GvbtRix9.js", HTTP_GET, serveAppImmutableNodes_1GvbtRix9Js);
    server->on("/app/immutable/nodes/2.BEnNBxPJ.js", HTTP_GET, serveAppImmutableNodes_2BEnNBxPjJs);
    server->on("/app/immutable/nodes/3.BscV4i9e.js", HTTP_GET, serveAppImmutableNodes_3BscV4i9eJs);
    server->on("/app/immutable/nodes/4.CtRLmezX.js", HTTP_GET, serveAppImmutableNodes_4CtRLmezXJs);
    server->on("/app/immutable/nodes/5.DNJp9dxK.js", HTTP_GET, serveAppImmutableNodes_5DnJp9dxKJs);
    server->on("/app/immutable/nodes/6.B1880F3S.js", HTTP_GET, serveAppImmutableNodes_6B1880F3SJs);
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
