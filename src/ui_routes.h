/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,299 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 48,540 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 63,054 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.dcg9vOzM.css", HTTP_GET, serveAppImmutableAssetsInternalDcg9vOzMCss);
    server->on("/app/immutable/assets/start.6opmQcx_.css", HTTP_GET, serveAppImmutableAssetsStart_6opmQcxCss);
    server->on("/app/immutable/entry/app.B_NRxB1l.js", HTTP_GET, serveAppImmutableEntryAppBNRxB1lJs);
    server->on("/app/immutable/entry/start.sHsAe3iK.js", HTTP_GET, serveAppImmutableEntryStartSHsAe3iKJs);
    server->on("/app/immutable/nodes/0.BgwFR5Vl.js", HTTP_GET, serveAppImmutableNodes_0BgwFr5VlJs);
    server->on("/app/immutable/nodes/1.CboITKGw.js", HTTP_GET, serveAppImmutableNodes_1CboItkGwJs);
    server->on("/app/immutable/nodes/2.Bc3YO8Y1.js", HTTP_GET, serveAppImmutableNodes_2Bc3Yo8Y1Js);
    server->on("/app/immutable/nodes/3.BVbASigV.js", HTTP_GET, serveAppImmutableNodes_3BVbASigVJs);
    server->on("/app/immutable/nodes/4.DQZY3W0P.js", HTTP_GET, serveAppImmutableNodes_4Dqzy3W0PJs);
    server->on("/app/immutable/nodes/5.AQ3LgDrG.js", HTTP_GET, serveAppImmutableNodes_5Aq3LgDrGJs);
    server->on("/app/immutable/nodes/6.DOcaOe_R.js", HTTP_GET, serveAppImmutableNodes_6DOcaOeRJs);
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
