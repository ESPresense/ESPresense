/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 45,014 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 58,102 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DN2mWWgh.css", HTTP_GET, serveAppImmutableAssetsInternalDn2mWWghCss);
    server->on("/app/immutable/assets/start.BfkQNPT2.css", HTTP_GET, serveAppImmutableAssetsStartBfkQnpt2Css);
    server->on("/app/immutable/entry/app.BMkXJhaw.js", HTTP_GET, serveAppImmutableEntryAppBMkXJhawJs);
    server->on("/app/immutable/entry/start.DVtKKGXh.js", HTTP_GET, serveAppImmutableEntryStartDVtKkgXhJs);
    server->on("/app/immutable/nodes/0.B1ngUrhi.js", HTTP_GET, serveAppImmutableNodes_0B1ngUrhiJs);
    server->on("/app/immutable/nodes/1.BopZtz-u.js", HTTP_GET, serveAppImmutableNodes_1BopZtzUJs);
    server->on("/app/immutable/nodes/2.C1-dNONc.js", HTTP_GET, serveAppImmutableNodes_2C1DNoNcJs);
    server->on("/app/immutable/nodes/3.Czefjy3p.js", HTTP_GET, serveAppImmutableNodes_3Czefjy3pJs);
    server->on("/app/immutable/nodes/4.B1ZllCZs.js", HTTP_GET, serveAppImmutableNodes_4B1ZllCZsJs);
    server->on("/app/immutable/nodes/5.CYJJL2Jx.js", HTTP_GET, serveAppImmutableNodes_5Cyjjl2JxJs);
    server->on("/app/immutable/nodes/6.BeETBAXS.js", HTTP_GET, serveAppImmutableNodes_6BeEtbaxsJs);
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
