/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 14,520 bytes
 * ui_app_immutable_chunks_js: 65,971 bytes
 * ui_app_immutable_entry_js: 272 bytes
 * ui_app_immutable_nodes_js: 542 bytes
 * ui_html: 4,652 bytes
 * ui_svg: 456 bytes
 * Total: 86,413 bytes
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
    server->on("/app/immutable/chunks/FGARPxUH.js", HTTP_GET, serveAppImmutableChunksFgarPxUhJs);
    server->on("/app/immutable/entry/app.sPNs-FEu.js", HTTP_GET, serveAppImmutableEntryAppSPNsFEuJs);
    server->on("/app/immutable/entry/start.CIhOv1o8.js", HTTP_GET, serveAppImmutableEntryStartCIhOv1o8Js);
    server->on("/app/immutable/nodes/0.B2PPOOu1.js", HTTP_GET, serveAppImmutableNodes_0B2PpoOu1Js);
    server->on("/app/immutable/nodes/1.9pFJDU0m.js", HTTP_GET, serveAppImmutableNodes_1_9pFjdu0mJs);
    server->on("/app/immutable/nodes/2.CO0tjcMS.js", HTTP_GET, serveAppImmutableNodes_2Co0tjcMsJs);
    server->on("/app/immutable/nodes/3.DZVM8X1W.js", HTTP_GET, serveAppImmutableNodes_3Dzvm8X1WJs);
    server->on("/app/immutable/nodes/4.DZ4Fhj9u.js", HTTP_GET, serveAppImmutableNodes_4Dz4Fhj9uJs);
    server->on("/app/immutable/nodes/5.C9S0i5vb.js", HTTP_GET, serveAppImmutableNodes_5C9S0i5vbJs);
    server->on("/app/immutable/nodes/6.69EE_lgA.js", HTTP_GET, serveAppImmutableNodes_6_69EeLgAJs);
    server->on("/app/immutable/nodes/7.CER2hIlG.js", HTTP_GET, serveAppImmutableNodes_7Cer2hIlGJs);
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
