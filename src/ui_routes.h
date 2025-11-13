/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,462 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 66,268 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 80,960 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CcwqoBYG.css", HTTP_GET, serveAppImmutableAssetsInternalCcwqoBygCss);
    server->on("/app/immutable/assets/start.XzWBJjFS.css", HTTP_GET, serveAppImmutableAssetsStartXzWbJjFsCss);
    server->on("/app/immutable/entry/app.CJTzo0o4.js", HTTP_GET, serveAppImmutableEntryAppCjTzo0o4Js);
    server->on("/app/immutable/entry/start.DOjlQyxh.js", HTTP_GET, serveAppImmutableEntryStartDOjlQyxhJs);
    server->on("/app/immutable/nodes/0.COQPKchZ.js", HTTP_GET, serveAppImmutableNodes_0CoqpKchZJs);
    server->on("/app/immutable/nodes/1.BjUK79RC.js", HTTP_GET, serveAppImmutableNodes_1BjUk79RcJs);
    server->on("/app/immutable/nodes/2.B2LlOMyi.js", HTTP_GET, serveAppImmutableNodes_2B2LlOMyiJs);
    server->on("/app/immutable/nodes/3.XkqjmsKw.js", HTTP_GET, serveAppImmutableNodes_3XkqjmsKwJs);
    server->on("/app/immutable/nodes/4.BuhUGWT7.js", HTTP_GET, serveAppImmutableNodes_4BuhUgwt7Js);
    server->on("/app/immutable/nodes/5.DXEXEG9X.js", HTTP_GET, serveAppImmutableNodes_5Dxexeg9XJs);
    server->on("/app/immutable/nodes/6.CSAjjZE2.js", HTTP_GET, serveAppImmutableNodes_6CsAjjZe2Js);
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
