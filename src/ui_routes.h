/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,210 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 45,338 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 59,778 bytes
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
    server->on("/app/immutable/entry/app.C16DZ_rO.js", HTTP_GET, serveAppImmutableEntryAppC16DzROJs);
    server->on("/app/immutable/entry/start.BOE2TOoH.js", HTTP_GET, serveAppImmutableEntryStartBoe2TOoHJs);
    server->on("/app/immutable/nodes/0.CT1YSc8v.js", HTTP_GET, serveAppImmutableNodes_0Ct1YSc8vJs);
    server->on("/app/immutable/nodes/1.CwbMXG69.js", HTTP_GET, serveAppImmutableNodes_1CwbMxg69Js);
    server->on("/app/immutable/nodes/2.fj19HQKN.js", HTTP_GET, serveAppImmutableNodes_2Fj19HqknJs);
    server->on("/app/immutable/nodes/3.CTFaJmQO.js", HTTP_GET, serveAppImmutableNodes_3CtFaJmQoJs);
    server->on("/app/immutable/nodes/4.D9emeeX1.js", HTTP_GET, serveAppImmutableNodes_4D9emeeX1Js);
    server->on("/app/immutable/nodes/5.BoSRxFG2.js", HTTP_GET, serveAppImmutableNodes_5BoSRxFg2Js);
    server->on("/app/immutable/nodes/6.BonZqjUM.js", HTTP_GET, serveAppImmutableNodes_6BonZqjUmJs);
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
