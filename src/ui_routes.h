/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 45,093 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 58,186 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DN2mWWgh.css", HTTP_GET, serveAppImmutableAssetsInternalDn2mWWghCss);
    server->on("/app/immutable/assets/start.BfkQNPT2.css", HTTP_GET, serveAppImmutableAssetsStartBfkQnpt2Css);
    server->on("/app/immutable/entry/app.5FDgHXmH.js", HTTP_GET, serveAppImmutableEntryApp_5FDgHXmHJs);
    server->on("/app/immutable/entry/start.RdRfikS8.js", HTTP_GET, serveAppImmutableEntryStartRdRfikS8Js);
    server->on("/app/immutable/nodes/0.BW676RgV.js", HTTP_GET, serveAppImmutableNodes_0Bw676RgVJs);
    server->on("/app/immutable/nodes/1.Jc5Dv5tZ.js", HTTP_GET, serveAppImmutableNodes_1Jc5Dv5tZJs);
    server->on("/app/immutable/nodes/2.ZBW-GaKX.js", HTTP_GET, serveAppImmutableNodes_2ZbwGaKxJs);
    server->on("/app/immutable/nodes/3.-Y1aS8zm.js", HTTP_GET, serveAppImmutableNodes_3Y1aS8zmJs);
    server->on("/app/immutable/nodes/4.BKyXPTr4.js", HTTP_GET, serveAppImmutableNodes_4BKyXpTr4Js);
    server->on("/app/immutable/nodes/5.Bg6jeZyE.js", HTTP_GET, serveAppImmutableNodes_5Bg6jeZyEJs);
    server->on("/app/immutable/nodes/6.qaDtm2HA.js", HTTP_GET, serveAppImmutableNodes_6QaDtm2HaJs);
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
