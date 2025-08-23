/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,361 bytes
 * ui_html: 3,240 bytes
 * ui_app_immutable_entry_js: 48,625 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 63,196 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.B89G9FIo.css", HTTP_GET, serveAppImmutableAssetsInternalB89G9FIoCss);
    server->on("/app/immutable/assets/start.D-jtWh4c.css", HTTP_GET, serveAppImmutableAssetsStartDJtWh4cCss);
    server->on("/app/immutable/entry/app.Bqofl6Zj.js", HTTP_GET, serveAppImmutableEntryAppBqofl6ZjJs);
    server->on("/app/immutable/entry/start.C5IhBpGh.js", HTTP_GET, serveAppImmutableEntryStartC5IhBpGhJs);
    server->on("/app/immutable/nodes/0.C6VCPYFb.js", HTTP_GET, serveAppImmutableNodes_0C6VcpyFbJs);
    server->on("/app/immutable/nodes/1.Db7nTP07.js", HTTP_GET, serveAppImmutableNodes_1Db7nTp07Js);
    server->on("/app/immutable/nodes/2.lO6dHw5i.js", HTTP_GET, serveAppImmutableNodes_2LO6dHw5iJs);
    server->on("/app/immutable/nodes/3.DBzVf0Nr.js", HTTP_GET, serveAppImmutableNodes_3DBzVf0NrJs);
    server->on("/app/immutable/nodes/4.Dbgxx8p3.js", HTTP_GET, serveAppImmutableNodes_4Dbgxx8p3Js);
    server->on("/app/immutable/nodes/5.OCHRxmiB.js", HTTP_GET, serveAppImmutableNodes_5OchRxmiBJs);
    server->on("/app/immutable/nodes/6.B6LNSUj3.js", HTTP_GET, serveAppImmutableNodes_6B6LnsUj3Js);
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
