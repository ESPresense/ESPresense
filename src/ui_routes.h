/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,571 bytes
 * ui_html: 3,240 bytes
 * ui_app_immutable_entry_js: 66,236 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 81,022 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.wrGhOo2C.css", HTTP_GET, serveAppImmutableAssetsInternalWrGhOo2CCss);
    server->on("/app/immutable/assets/start.DOTUP31V.css", HTTP_GET, serveAppImmutableAssetsStartDotup31VCss);
    server->on("/app/immutable/entry/app.Cv0PdRwH.js", HTTP_GET, serveAppImmutableEntryAppCv0PdRwHJs);
    server->on("/app/immutable/entry/start.D-db4ohs.js", HTTP_GET, serveAppImmutableEntryStartDDb4ohsJs);
    server->on("/app/immutable/nodes/0.CfXITrF0.js", HTTP_GET, serveAppImmutableNodes_0CfXiTrF0Js);
    server->on("/app/immutable/nodes/1.Peyw7y36.js", HTTP_GET, serveAppImmutableNodes_1Peyw7y36Js);
    server->on("/app/immutable/nodes/2.BBitr1yy.js", HTTP_GET, serveAppImmutableNodes_2BBitr1yyJs);
    server->on("/app/immutable/nodes/3.Dd9yujqp.js", HTTP_GET, serveAppImmutableNodes_3Dd9yujqpJs);
    server->on("/app/immutable/nodes/4.CEioOMSe.js", HTTP_GET, serveAppImmutableNodes_4CEioOmSeJs);
    server->on("/app/immutable/nodes/5.D169VmJx.js", HTTP_GET, serveAppImmutableNodes_5D169VmJxJs);
    server->on("/app/immutable/nodes/6.Cno7Y3a5.js", HTTP_GET, serveAppImmutableNodes_6Cno7Y3a5Js);
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
