/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,299 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 48,031 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 62,550 bytes
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
    server->on("/app/immutable/entry/app.n2glBZUg.js", HTTP_GET, serveAppImmutableEntryAppN2glBzUgJs);
    server->on("/app/immutable/entry/start.C5OukEmk.js", HTTP_GET, serveAppImmutableEntryStartC5OukEmkJs);
    server->on("/app/immutable/nodes/0.XfzrHdQc.js", HTTP_GET, serveAppImmutableNodes_0XfzrHdQcJs);
    server->on("/app/immutable/nodes/1.B0NhE34F.js", HTTP_GET, serveAppImmutableNodes_1B0NhE34FJs);
    server->on("/app/immutable/nodes/2.BrvL4jGO.js", HTTP_GET, serveAppImmutableNodes_2BrvL4jGoJs);
    server->on("/app/immutable/nodes/3.C9Ershbe.js", HTTP_GET, serveAppImmutableNodes_3C9ErshbeJs);
    server->on("/app/immutable/nodes/4.BkqBWcfu.js", HTTP_GET, serveAppImmutableNodes_4BkqBWcfuJs);
    server->on("/app/immutable/nodes/5.ri9Toy0U.js", HTTP_GET, serveAppImmutableNodes_5Ri9Toy0UJs);
    server->on("/app/immutable/nodes/6.AyvrGVNK.js", HTTP_GET, serveAppImmutableNodes_6AyvrGvnkJs);
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
