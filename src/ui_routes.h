/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 44,848 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 57,946 bytes
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
    server->on("/app/immutable/entry/app.CZKrhnDE.js", HTTP_GET, serveAppImmutableEntryAppCzKrhnDeJs);
    server->on("/app/immutable/entry/start.Ds2Y_jgr.js", HTTP_GET, serveAppImmutableEntryStartDs2YJgrJs);
    server->on("/app/immutable/nodes/0.CdJZ1r-k.js", HTTP_GET, serveAppImmutableNodes_0CdJz1rKJs);
    server->on("/app/immutable/nodes/1.xzJlon7c.js", HTTP_GET, serveAppImmutableNodes_1XzJlon7cJs);
    server->on("/app/immutable/nodes/2.DjPfH6qc.js", HTTP_GET, serveAppImmutableNodes_2DjPfH6qcJs);
    server->on("/app/immutable/nodes/3.2OekyMbm.js", HTTP_GET, serveAppImmutableNodes_3_2OekyMbmJs);
    server->on("/app/immutable/nodes/4.CXBKZ-6m.js", HTTP_GET, serveAppImmutableNodes_4Cxbkz_6mJs);
    server->on("/app/immutable/nodes/5.Br4HbnsS.js", HTTP_GET, serveAppImmutableNodes_5Br4HbnsSJs);
    server->on("/app/immutable/nodes/6.C3S1nymW.js", HTTP_GET, serveAppImmutableNodes_6C3S1nymWJs);
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
