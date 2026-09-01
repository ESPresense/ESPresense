/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 14,520 bytes
 * ui_app_immutable_chunks_js: 66,367 bytes
 * ui_app_immutable_entry_js: 268 bytes
 * ui_app_immutable_nodes_js: 542 bytes
 * ui_html: 4,642 bytes
 * ui_svg: 456 bytes
 * Total: 86,795 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_app_immutable_chunks_js.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_html.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/index.DKQns9on.css", HTTP_GET, serveAppImmutableAssetsIndexDkQns9onCss);
    server->on("/app/immutable/chunks/DMngkxwn.js", HTTP_GET, serveAppImmutableChunksDMngkxwnJs);
    server->on("/app/immutable/entry/app.BGo4gh21.js", HTTP_GET, serveAppImmutableEntryAppBGo4gh21Js);
    server->on("/app/immutable/entry/start.BjmyzvU5.js", HTTP_GET, serveAppImmutableEntryStartBjmyzvU5Js);
    server->on("/app/immutable/nodes/0.5Pyn4wEx.js", HTTP_GET, serveAppImmutableNodes_0_5Pyn4wExJs);
    server->on("/app/immutable/nodes/1.CDamB5mx.js", HTTP_GET, serveAppImmutableNodes_1CDamB5mxJs);
    server->on("/app/immutable/nodes/2.Dqq4iJSc.js", HTTP_GET, serveAppImmutableNodes_2Dqq4iJScJs);
    server->on("/app/immutable/nodes/3.CNTzsAj4.js", HTTP_GET, serveAppImmutableNodes_3CnTzsAj4Js);
    server->on("/app/immutable/nodes/4.BfcZzNfr.js", HTTP_GET, serveAppImmutableNodes_4BfcZzNfrJs);
    server->on("/app/immutable/nodes/5.DfrgYewy.js", HTTP_GET, serveAppImmutableNodes_5DfrgYewyJs);
    server->on("/app/immutable/nodes/6.CbTkKIC0.js", HTTP_GET, serveAppImmutableNodes_6CbTkKic0Js);
    server->on("/app/immutable/nodes/7.DrbGeEtQ.js", HTTP_GET, serveAppImmutableNodes_7DrbGeEtQJs);
    server->on("/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes
    server->on("/", HTTP_GET, serveIndexHtml);
    server->on("/devices", HTTP_GET, serveDevicesHtml);
    server->on("/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/hardware", HTTP_GET, serveHardwareHtml);
    server->on("/hardware.html", HTTP_GET, serveHardwareHtml);
    server->on("/network", HTTP_GET, serveNetworkHtml);
    server->on("/network.html", HTTP_GET, serveNetworkHtml);
    server->on("/settings", HTTP_GET, serveSettingsHtml);
    server->on("/settings.html", HTTP_GET, serveSettingsHtml);
}
