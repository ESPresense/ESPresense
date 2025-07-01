/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 45,368 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 58,471 bytes
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
    server->on("/app/immutable/entry/app.CWvuWE73.js", HTTP_GET, serveAppImmutableEntryAppCWvuWe73Js);
    server->on("/app/immutable/entry/start.DhQygNtS.js", HTTP_GET, serveAppImmutableEntryStartDhQygNtSJs);
    server->on("/app/immutable/nodes/0.DtUYX8Pn.js", HTTP_GET, serveAppImmutableNodes_0DtUyx8PnJs);
    server->on("/app/immutable/nodes/1.B1cDgW7l.js", HTTP_GET, serveAppImmutableNodes_1B1cDgW7lJs);
    server->on("/app/immutable/nodes/2.DE7N87bb.js", HTTP_GET, serveAppImmutableNodes_2De7N87bbJs);
    server->on("/app/immutable/nodes/3.CIvMaRrK.js", HTTP_GET, serveAppImmutableNodes_3CIvMaRrKJs);
    server->on("/app/immutable/nodes/4.B7w4brBD.js", HTTP_GET, serveAppImmutableNodes_4B7w4brBdJs);
    server->on("/app/immutable/nodes/5.BeC6Fz8N.js", HTTP_GET, serveAppImmutableNodes_5BeC6Fz8NJs);
    server->on("/app/immutable/nodes/6.DcK8PHTs.js", HTTP_GET, serveAppImmutableNodes_6DcK8PhTsJs);
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
