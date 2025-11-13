/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,462 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 66,267 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 80,964 bytes
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
    server->on("/app/immutable/entry/app.C0BaKm7I.js", HTTP_GET, serveAppImmutableEntryAppC0BaKm7IJs);
    server->on("/app/immutable/entry/start.DHg8SoV6.js", HTTP_GET, serveAppImmutableEntryStartDHg8SoV6Js);
    server->on("/app/immutable/nodes/0.6MVXJjH4.js", HTTP_GET, serveAppImmutableNodes_0_6MvxJjH4Js);
    server->on("/app/immutable/nodes/1.BSBDzMzv.js", HTTP_GET, serveAppImmutableNodes_1BsbDzMzvJs);
    server->on("/app/immutable/nodes/2.BjYMPYFO.js", HTTP_GET, serveAppImmutableNodes_2BjYmpyfoJs);
    server->on("/app/immutable/nodes/3.qRBwh8cn.js", HTTP_GET, serveAppImmutableNodes_3QRBwh8cnJs);
    server->on("/app/immutable/nodes/4.CGI9cM-n.js", HTTP_GET, serveAppImmutableNodes_4Cgi9cMNJs);
    server->on("/app/immutable/nodes/5.CfMvSWy9.js", HTTP_GET, serveAppImmutableNodes_5CfMvSWy9Js);
    server->on("/app/immutable/nodes/6.DIz7HrJd.js", HTTP_GET, serveAppImmutableNodes_6DIz7HrJdJs);
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
