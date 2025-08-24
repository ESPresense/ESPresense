/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 5,261 bytes
 * ui_app_immutable_entry_js: 49,192 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_html: 3,250 bytes
 * ui_svg: 456 bytes
 * Total: 58,673 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_html.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/start.DdNmlMvT.css", HTTP_GET, serveAppImmutableAssetsStartDdNmlMvTCss);
    server->on("/app/immutable/entry/start.q2sxJERG.js", HTTP_GET, serveAppImmutableEntryStartQ2sxJergJs);
    server->on("/app/immutable/entry/app.CZpw9BAs.js", HTTP_GET, serveAppImmutableEntryAppCZpw9BAsJs);
    server->on("/app/immutable/nodes/0.BNb73Gh7.js", HTTP_GET, serveAppImmutableNodes_0BNb73Gh7Js);
    server->on("/app/immutable/nodes/1.GIugSdo5.js", HTTP_GET, serveAppImmutableNodes_1GIugSdo5Js);
    server->on("/app/immutable/nodes/2.Dwx8LLT9.js", HTTP_GET, serveAppImmutableNodes_2Dwx8Llt9Js);
    server->on("/app/immutable/nodes/3.B5MXV-f2.js", HTTP_GET, serveAppImmutableNodes_3B5MxvF2Js);
    server->on("/app/immutable/nodes/4.ldHTy56_.js", HTTP_GET, serveAppImmutableNodes_4LdHTy56Js);
    server->on("/app/immutable/nodes/5.DqaaYJzS.js", HTTP_GET, serveAppImmutableNodes_5DqaaYJzSJs);
    server->on("/app/immutable/nodes/6.Bk0xpFne.js", HTTP_GET, serveAppImmutableNodes_6Bk0xpFneJs);
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
