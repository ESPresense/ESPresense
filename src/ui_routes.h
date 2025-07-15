/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,210 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 47,226 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 61,661 bytes
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
    server->on("/app/immutable/entry/app.ChAPcWlL.js", HTTP_GET, serveAppImmutableEntryAppChAPcWlLJs);
    server->on("/app/immutable/entry/start.BpkUvH1S.js", HTTP_GET, serveAppImmutableEntryStartBpkUvH1SJs);
    server->on("/app/immutable/nodes/0.GLZL4jcx.js", HTTP_GET, serveAppImmutableNodes_0Glzl4jcxJs);
    server->on("/app/immutable/nodes/1.DBI2cXcY.js", HTTP_GET, serveAppImmutableNodes_1Dbi2cXcYJs);
    server->on("/app/immutable/nodes/2.CxuI7NgL.js", HTTP_GET, serveAppImmutableNodes_2CxuI7NgLJs);
    server->on("/app/immutable/nodes/3.DZfqs80M.js", HTTP_GET, serveAppImmutableNodes_3DZfqs80MJs);
    server->on("/app/immutable/nodes/4.Dq-HaBKk.js", HTTP_GET, serveAppImmutableNodes_4DqHaBKkJs);
    server->on("/app/immutable/nodes/5.Df7CeloS.js", HTTP_GET, serveAppImmutableNodes_5Df7CeloSJs);
    server->on("/app/immutable/nodes/6.D32cLYT1.js", HTTP_GET, serveAppImmutableNodes_6D32cLyt1Js);
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
