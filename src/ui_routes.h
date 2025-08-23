/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,243 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 45,725 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 60,188 bytes
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
    server->on("/app/immutable/entry/app.D24sEDnG.js", HTTP_GET, serveAppImmutableEntryAppD24sEDnGJs);
    server->on("/app/immutable/entry/start.Bcaz09Ax.js", HTTP_GET, serveAppImmutableEntryStartBcaz09AxJs);
    server->on("/app/immutable/nodes/0.DvA2NPkd.js", HTTP_GET, serveAppImmutableNodes_0DvA2NPkdJs);
    server->on("/app/immutable/nodes/1.CJHUEfpT.js", HTTP_GET, serveAppImmutableNodes_1CjhuEfpTJs);
    server->on("/app/immutable/nodes/2.uaKKqGyd.js", HTTP_GET, serveAppImmutableNodes_2UaKKqGydJs);
    server->on("/app/immutable/nodes/3.CQGdSEJ1.js", HTTP_GET, serveAppImmutableNodes_3CqGdSej1Js);
    server->on("/app/immutable/nodes/4.TV2TZiJr.js", HTTP_GET, serveAppImmutableNodes_4Tv2TZiJrJs);
    server->on("/app/immutable/nodes/5.QWEnc0Yh.js", HTTP_GET, serveAppImmutableNodes_5QwEnc0YhJs);
    server->on("/app/immutable/nodes/6.CHG_MmA5.js", HTTP_GET, serveAppImmutableNodes_6ChgMmA5Js);
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
