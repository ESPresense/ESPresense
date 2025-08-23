/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,243 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 45,689 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 60,152 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CiKEhKNm.css", HTTP_GET, serveAppImmutableAssetsInternalCiKEhKNmCss);
    server->on("/app/immutable/assets/start.DpkgWjYd.css", HTTP_GET, serveAppImmutableAssetsStartDpkgWjYdCss);
    server->on("/app/immutable/entry/app.Bl6zvGFx.js", HTTP_GET, serveAppImmutableEntryAppBl6zvGFxJs);
    server->on("/app/immutable/entry/start.CtcZJ8kN.js", HTTP_GET, serveAppImmutableEntryStartCtcZj8kNJs);
    server->on("/app/immutable/nodes/0.BaBDna1s.js", HTTP_GET, serveAppImmutableNodes_0BaBDna1sJs);
    server->on("/app/immutable/nodes/1.Bwd8XVqE.js", HTTP_GET, serveAppImmutableNodes_1Bwd8XVqEJs);
    server->on("/app/immutable/nodes/2.CHeCr9lM.js", HTTP_GET, serveAppImmutableNodes_2CHeCr9lMJs);
    server->on("/app/immutable/nodes/3.CRDlublG.js", HTTP_GET, serveAppImmutableNodes_3CrDlublGJs);
    server->on("/app/immutable/nodes/4.CoJkuoKc.js", HTTP_GET, serveAppImmutableNodes_4CoJkuoKcJs);
    server->on("/app/immutable/nodes/5.BoLoyTPk.js", HTTP_GET, serveAppImmutableNodes_5BoLoyTPkJs);
    server->on("/app/immutable/nodes/6.BxcxfWHh.js", HTTP_GET, serveAppImmutableNodes_6BxcxfWHhJs);
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
