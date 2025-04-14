/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 44,908 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 58,011 bytes
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
    server->on("/app/immutable/entry/app.DSjX9ojX.js", HTTP_GET, serveAppImmutableEntryAppDSjX9ojXJs);
    server->on("/app/immutable/entry/start.8NnSTKDP.js", HTTP_GET, serveAppImmutableEntryStart_8NnStkdpJs);
    server->on("/app/immutable/nodes/0.B6VBSSce.js", HTTP_GET, serveAppImmutableNodes_0B6VbsSceJs);
    server->on("/app/immutable/nodes/1.CNqMoub2.js", HTTP_GET, serveAppImmutableNodes_1CNqMoub2Js);
    server->on("/app/immutable/nodes/2.COwkEn-E.js", HTTP_GET, serveAppImmutableNodes_2COwkEnEJs);
    server->on("/app/immutable/nodes/3.CEsMwbCx.js", HTTP_GET, serveAppImmutableNodes_3CEsMwbCxJs);
    server->on("/app/immutable/nodes/4.bhvraEeF.js", HTTP_GET, serveAppImmutableNodes_4BhvraEeFJs);
    server->on("/app/immutable/nodes/5.CAOqgQnM.js", HTTP_GET, serveAppImmutableNodes_5CaOqgQnMJs);
    server->on("/app/immutable/nodes/6.IhlGoz3-.js", HTTP_GET, serveAppImmutableNodes_6IhlGoz3Js);
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
