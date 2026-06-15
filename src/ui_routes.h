/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,138 bytes
 * ui_html: 4,531 bytes
 * ui_app_immutable_entry_js: 67,498 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 98,213 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DPaphot8.css", HTTP_GET, serveAppImmutableAssetsInternalDPaphot8Css);
    server->on("/app/immutable/assets/start.DPaphot8.css", HTTP_GET, serveAppImmutableAssetsStartDPaphot8Css);
    server->on("/app/immutable/entry/app.DkIAwQyu.js", HTTP_GET, serveAppImmutableEntryAppDkIAwQyuJs);
    server->on("/app/immutable/entry/start.Cuf7MWBX.js", HTTP_GET, serveAppImmutableEntryStartCuf7MwbxJs);
    server->on("/app/immutable/nodes/0.CIUeXJqt.js", HTTP_GET, serveAppImmutableNodes_0CiUeXJqtJs);
    server->on("/app/immutable/nodes/1.Dq-K_8c4.js", HTTP_GET, serveAppImmutableNodes_1DqK_8c4Js);
    server->on("/app/immutable/nodes/2.Bddb6YRT.js", HTTP_GET, serveAppImmutableNodes_2Bddb6YrtJs);
    server->on("/app/immutable/nodes/3.CuizYUI3.js", HTTP_GET, serveAppImmutableNodes_3CuizYui3Js);
    server->on("/app/immutable/nodes/4.BBRjLdeI.js", HTTP_GET, serveAppImmutableNodes_4BbRjLdeIJs);
    server->on("/app/immutable/nodes/5.Clwqr6zw.js", HTTP_GET, serveAppImmutableNodes_5Clwqr6zwJs);
    server->on("/app/immutable/nodes/6.CfD8bLB0.js", HTTP_GET, serveAppImmutableNodes_6CfD8bLb0Js);
    server->on("/app/immutable/nodes/7.CXxe4ttH.js", HTTP_GET, serveAppImmutableNodes_7CXxe4ttHJs);
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
