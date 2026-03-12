/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,122 bytes
 * ui_html: 4,326 bytes
 * ui_app_immutable_entry_js: 64,411 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,905 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CKNao7CM.css", HTTP_GET, serveAppImmutableAssetsInternalCkNao7CmCss);
    server->on("/app/immutable/assets/start.CKNao7CM.css", HTTP_GET, serveAppImmutableAssetsStartCkNao7CmCss);
    server->on("/app/immutable/entry/app.DsRZEuFi.js", HTTP_GET, serveAppImmutableEntryAppDsRzEuFiJs);
    server->on("/app/immutable/entry/start.b3WWatuJ.js", HTTP_GET, serveAppImmutableEntryStartB3WWatuJJs);
    server->on("/app/immutable/nodes/0.ChPEF1et.js", HTTP_GET, serveAppImmutableNodes_0ChPef1etJs);
    server->on("/app/immutable/nodes/1.C6wzEVcQ.js", HTTP_GET, serveAppImmutableNodes_1C6wzEVcQJs);
    server->on("/app/immutable/nodes/2.WtRb6MnV.js", HTTP_GET, serveAppImmutableNodes_2WtRb6MnVJs);
    server->on("/app/immutable/nodes/3.fm569Rme.js", HTTP_GET, serveAppImmutableNodes_3Fm569RmeJs);
    server->on("/app/immutable/nodes/4.DU9u8b04.js", HTTP_GET, serveAppImmutableNodes_4Du9u8b04Js);
    server->on("/app/immutable/nodes/5.C38cdKVj.js", HTTP_GET, serveAppImmutableNodes_5C38cdKVjJs);
    server->on("/app/immutable/nodes/6.CsTgxABh.js", HTTP_GET, serveAppImmutableNodes_6CsTgxABhJs);
    server->on("/app/immutable/nodes/7.BEbUn6CE.js", HTTP_GET, serveAppImmutableNodes_7BEbUn6CeJs);
    server->on("/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes
    server->on("/devices", HTTP_GET, serveDevicesHtml);
    server->on("/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/hardware", HTTP_GET, serveHardwareHtml);
    server->on("/hardware.html", HTTP_GET, serveHardwareHtml);
    server->on("/", HTTP_GET, serveIndexHtml);
    server->on("/network", HTTP_GET, serveNetworkHtml);
    server->on("/network.html", HTTP_GET, serveNetworkHtml);
    server->on("/settings", HTTP_GET, serveSettingsHtml);
    server->on("/settings.html", HTTP_GET, serveSettingsHtml);
}
