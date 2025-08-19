/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,210 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 45,552 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 59,992 bytes
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
    server->on("/app/immutable/entry/app.CP-MJz57.js", HTTP_GET, serveAppImmutableEntryAppCpMJz57Js);
    server->on("/app/immutable/entry/start.D-RhHQhj.js", HTTP_GET, serveAppImmutableEntryStartDRhHQhjJs);
    server->on("/app/immutable/nodes/0.D4-7bY27.js", HTTP_GET, serveAppImmutableNodes_0D4_7bY27Js);
    server->on("/app/immutable/nodes/1.e7oL5gY4.js", HTTP_GET, serveAppImmutableNodes_1E7oL5gY4Js);
    server->on("/app/immutable/nodes/2.DhI7Nb3w.js", HTTP_GET, serveAppImmutableNodes_2DhI7Nb3wJs);
    server->on("/app/immutable/nodes/3.BONhmk7O.js", HTTP_GET, serveAppImmutableNodes_3BoNhmk7OJs);
    server->on("/app/immutable/nodes/4.FvkKluc6.js", HTTP_GET, serveAppImmutableNodes_4FvkKluc6Js);
    server->on("/app/immutable/nodes/5.nRPx496U.js", HTTP_GET, serveAppImmutableNodes_5NRPx496UJs);
    server->on("/app/immutable/nodes/6.B-QWiuSg.js", HTTP_GET, serveAppImmutableNodes_6BQWiuSgJs);
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
