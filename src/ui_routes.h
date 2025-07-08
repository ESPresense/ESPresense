/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,210 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 45,372 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 59,802 bytes
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
    server->on("/app/immutable/entry/app.D1YNTwwe.js", HTTP_GET, serveAppImmutableEntryAppD1YnTwweJs);
    server->on("/app/immutable/entry/start.CHkgvFBK.js", HTTP_GET, serveAppImmutableEntryStartCHkgvFbkJs);
    server->on("/app/immutable/nodes/0.DTc3cPLL.js", HTTP_GET, serveAppImmutableNodes_0DTc3cPllJs);
    server->on("/app/immutable/nodes/1.Bqw1OoRo.js", HTTP_GET, serveAppImmutableNodes_1Bqw1OoRoJs);
    server->on("/app/immutable/nodes/2.DMX_-bpJ.js", HTTP_GET, serveAppImmutableNodes_2DmxBpJJs);
    server->on("/app/immutable/nodes/3.DryhDQao.js", HTTP_GET, serveAppImmutableNodes_3DryhDQaoJs);
    server->on("/app/immutable/nodes/4.2wMg4gPz.js", HTTP_GET, serveAppImmutableNodes_4_2wMg4gPzJs);
    server->on("/app/immutable/nodes/5.KOwKQeGZ.js", HTTP_GET, serveAppImmutableNodes_5KOwKQeGzJs);
    server->on("/app/immutable/nodes/6.cf32PkXc.js", HTTP_GET, serveAppImmutableNodes_6Cf32PkXcJs);
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
