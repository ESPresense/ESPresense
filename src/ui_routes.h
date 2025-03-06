/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,365 bytes
 * ui_app_immutable_chunks_js: 44,183 bytes
 * ui_app_immutable_entry_js: 225 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 57,586 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_chunks_js.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/index.DN2mWWgh.css", HTTP_GET, serveAppImmutableAssetsIndexDn2mWWghCss);
    server->on("/app/immutable/assets/index.BfkQNPT2.css", HTTP_GET, serveAppImmutableAssetsIndexBfkQnpt2Css);
    server->on("/app/immutable/chunks/DOcGRBt9.js", HTTP_GET, serveAppImmutableChunksDOcGrBt9Js);
    server->on("/app/immutable/entry/app.Bd7_t-6m.js", HTTP_GET, serveAppImmutableEntryAppBd7T_6mJs);
    server->on("/app/immutable/entry/start.s24c_uPO.js", HTTP_GET, serveAppImmutableEntryStartS24cUPoJs);
    server->on("/app/immutable/nodes/0.CNRjrXho.js", HTTP_GET, serveAppImmutableNodes_0CnRjrXhoJs);
    server->on("/app/immutable/nodes/1.CZIPrTcs.js", HTTP_GET, serveAppImmutableNodes_1CziPrTcsJs);
    server->on("/app/immutable/nodes/2.gUKTxrBx.js", HTTP_GET, serveAppImmutableNodes_2GUkTxrBxJs);
    server->on("/app/immutable/nodes/3.BlL00Tp1.js", HTTP_GET, serveAppImmutableNodes_3BlL00Tp1Js);
    server->on("/app/immutable/nodes/4.VE5mXAro.js", HTTP_GET, serveAppImmutableNodes_4Ve5mXAroJs);
    server->on("/app/immutable/nodes/5.0RrJSt8e.js", HTTP_GET, serveAppImmutableNodes_5_0RrJSt8eJs);
    server->on("/app/immutable/nodes/6.D-lcQvF4.js", HTTP_GET, serveAppImmutableNodes_6DLcQvF4Js);
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
