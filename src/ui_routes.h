/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,243 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 48,400 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 62,863 bytes
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
    server->on("/app/immutable/entry/app.CJyWIuCe.js", HTTP_GET, serveAppImmutableEntryAppCJyWIuCeJs);
    server->on("/app/immutable/entry/start.DJ3NDvO7.js", HTTP_GET, serveAppImmutableEntryStartDj3NDvO7Js);
    server->on("/app/immutable/nodes/0.B-ekYxPU.js", HTTP_GET, serveAppImmutableNodes_0BEkYxPuJs);
    server->on("/app/immutable/nodes/1.cFwer1Wt.js", HTTP_GET, serveAppImmutableNodes_1CFwer1WtJs);
    server->on("/app/immutable/nodes/2.HP8k-g3p.js", HTTP_GET, serveAppImmutableNodes_2Hp8kG3pJs);
    server->on("/app/immutable/nodes/3.eqQ3yh6F.js", HTTP_GET, serveAppImmutableNodes_3EqQ3yh6FJs);
    server->on("/app/immutable/nodes/4.B6gtNdJH.js", HTTP_GET, serveAppImmutableNodes_4B6gtNdJhJs);
    server->on("/app/immutable/nodes/5.CEdzvwLN.js", HTTP_GET, serveAppImmutableNodes_5CEdzvwLnJs);
    server->on("/app/immutable/nodes/6.9Q3bKJRn.js", HTTP_GET, serveAppImmutableNodes_6_9Q3bKjRnJs);
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
