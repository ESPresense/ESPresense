/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,523 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 66,294 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 81,042 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.fg3uWggT.css", HTTP_GET, serveAppImmutableAssetsInternalFg3uWggTCss);
    server->on("/app/immutable/assets/start._Bn8CRe_.css", HTTP_GET, serveAppImmutableAssetsStartBn8CReCss);
    server->on("/app/immutable/entry/app.CbAGiU-1.js", HTTP_GET, serveAppImmutableEntryAppCbAGiU_1Js);
    server->on("/app/immutable/entry/start.CpDgyTok.js", HTTP_GET, serveAppImmutableEntryStartCpDgyTokJs);
    server->on("/app/immutable/nodes/0.C-jDM9zR.js", HTTP_GET, serveAppImmutableNodes_0CJDm9zRJs);
    server->on("/app/immutable/nodes/1.DpExpndf.js", HTTP_GET, serveAppImmutableNodes_1DpExpndfJs);
    server->on("/app/immutable/nodes/2.1eeTnQ4f.js", HTTP_GET, serveAppImmutableNodes_2_1eeTnQ4fJs);
    server->on("/app/immutable/nodes/3.D8uX4Y-X.js", HTTP_GET, serveAppImmutableNodes_3D8uX4YXJs);
    server->on("/app/immutable/nodes/4.D9HO0RU9.js", HTTP_GET, serveAppImmutableNodes_4D9Ho0Ru9Js);
    server->on("/app/immutable/nodes/5.PstoIk4m.js", HTTP_GET, serveAppImmutableNodes_5PstoIk4mJs);
    server->on("/app/immutable/nodes/6.C3e0oqDk.js", HTTP_GET, serveAppImmutableNodes_6C3e0oqDkJs);
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
