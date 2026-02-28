/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,122 bytes
 * ui_html: 4,320 bytes
 * ui_app_immutable_entry_js: 64,054 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,542 bytes
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
    server->on("/app/immutable/entry/app.1t1voLMG.js", HTTP_GET, serveAppImmutableEntryApp_1t1voLmgJs);
    server->on("/app/immutable/entry/start.BAdbczA8.js", HTTP_GET, serveAppImmutableEntryStartBAdbczA8Js);
    server->on("/app/immutable/nodes/0.BOc3p101.js", HTTP_GET, serveAppImmutableNodes_0BOc3p101Js);
    server->on("/app/immutable/nodes/1.BWhf5zar.js", HTTP_GET, serveAppImmutableNodes_1BWhf5zarJs);
    server->on("/app/immutable/nodes/2.BeN_W-xc.js", HTTP_GET, serveAppImmutableNodes_2BeNWXcJs);
    server->on("/app/immutable/nodes/3.C9hMYsLU.js", HTTP_GET, serveAppImmutableNodes_3C9hMYsLuJs);
    server->on("/app/immutable/nodes/4.Ds4bwzXX.js", HTTP_GET, serveAppImmutableNodes_4Ds4bwzXxJs);
    server->on("/app/immutable/nodes/5.BfoG2pXY.js", HTTP_GET, serveAppImmutableNodes_5BfoG2pXyJs);
    server->on("/app/immutable/nodes/6.Piqwb1dJ.js", HTTP_GET, serveAppImmutableNodes_6Piqwb1dJJs);
    server->on("/app/immutable/nodes/7.mxkgu2oI.js", HTTP_GET, serveAppImmutableNodes_7Mxkgu2oIJs);
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
