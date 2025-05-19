/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 44,981 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 58,084 bytes
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
    server->on("/app/immutable/entry/app.fG7Qwjmf.js", HTTP_GET, serveAppImmutableEntryAppFG7QwjmfJs);
    server->on("/app/immutable/entry/start.CMANNI2E.js", HTTP_GET, serveAppImmutableEntryStartCmanni2EJs);
    server->on("/app/immutable/nodes/0.DXm_UU1m.js", HTTP_GET, serveAppImmutableNodes_0DXmUu1mJs);
    server->on("/app/immutable/nodes/1.BYIexzV9.js", HTTP_GET, serveAppImmutableNodes_1ByIexzV9Js);
    server->on("/app/immutable/nodes/2.BepRIJ-X.js", HTTP_GET, serveAppImmutableNodes_2BepRijXJs);
    server->on("/app/immutable/nodes/3.CmfMa3YF.js", HTTP_GET, serveAppImmutableNodes_3CmfMa3YfJs);
    server->on("/app/immutable/nodes/4.Dj7lSvlj.js", HTTP_GET, serveAppImmutableNodes_4Dj7lSvljJs);
    server->on("/app/immutable/nodes/5.BuLzZ4qg.js", HTTP_GET, serveAppImmutableNodes_5BuLzZ4qgJs);
    server->on("/app/immutable/nodes/6.CtUKAQxF.js", HTTP_GET, serveAppImmutableNodes_6CtUkaQxFJs);
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
