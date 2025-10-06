/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,361 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 48,748 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 63,324 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CNjsDNpd.css", HTTP_GET, serveAppImmutableAssetsInternalCNjsDNpdCss);
    server->on("/app/immutable/assets/start.BSuysxAz.css", HTTP_GET, serveAppImmutableAssetsStartBSuysxAzCss);
    server->on("/app/immutable/entry/app.B_gD7ymx.js", HTTP_GET, serveAppImmutableEntryAppBGD7ymxJs);
    server->on("/app/immutable/entry/start.CnWEce3H.js", HTTP_GET, serveAppImmutableEntryStartCnWEce3HJs);
    server->on("/app/immutable/nodes/0.BRh-CIuO.js", HTTP_GET, serveAppImmutableNodes_0BRhCIuOJs);
    server->on("/app/immutable/nodes/1.DOYY28_r.js", HTTP_GET, serveAppImmutableNodes_1Doyy28RJs);
    server->on("/app/immutable/nodes/2.BvpHb8kk.js", HTTP_GET, serveAppImmutableNodes_2BvpHb8kkJs);
    server->on("/app/immutable/nodes/3.TJM_6xm_.js", HTTP_GET, serveAppImmutableNodes_3Tjm_6xmJs);
    server->on("/app/immutable/nodes/4.jjz4Ky08.js", HTTP_GET, serveAppImmutableNodes_4Jjz4Ky08Js);
    server->on("/app/immutable/nodes/5.DalZMt7g.js", HTTP_GET, serveAppImmutableNodes_5DalZMt7gJs);
    server->on("/app/immutable/nodes/6.Ctd1xkJ-.js", HTTP_GET, serveAppImmutableNodes_6Ctd1xkJJs);
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
