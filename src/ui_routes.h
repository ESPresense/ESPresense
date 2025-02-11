/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,872 bytes
 * ui_html: 3,360 bytes
 * ui_app_immutable_chunks_js: 44,332 bytes
 * ui_app_immutable_entry_js: 224 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 57,728 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_chunks_js.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/index.BvV_u1cj.css", HTTP_GET, serveAppImmutableAssetsIndexBvVU1cjCss);
    server->on("/app/immutable/assets/index.C9yXJMb3.css", HTTP_GET, serveAppImmutableAssetsIndexC9yXjMb3Css);
    server->on("/app/immutable/chunks/BzEfM-yx.js", HTTP_GET, serveAppImmutableChunksBzEfMYxJs);
    server->on("/app/immutable/entry/app.BJOsbrDb.js", HTTP_GET, serveAppImmutableEntryAppBjOsbrDbJs);
    server->on("/app/immutable/entry/start.BRWhmY9K.js", HTTP_GET, serveAppImmutableEntryStartBrWhmY9KJs);
    server->on("/app/immutable/nodes/0.f-UXSzsA.js", HTTP_GET, serveAppImmutableNodes_0FUxSzsAJs);
    server->on("/app/immutable/nodes/1.Qt5HEqQj.js", HTTP_GET, serveAppImmutableNodes_1Qt5HEqQjJs);
    server->on("/app/immutable/nodes/2.CAPvpTL7.js", HTTP_GET, serveAppImmutableNodes_2CaPvpTl7Js);
    server->on("/app/immutable/nodes/3.BRF1IttE.js", HTTP_GET, serveAppImmutableNodes_3Brf1IttEJs);
    server->on("/app/immutable/nodes/4.hR3DFtNr.js", HTTP_GET, serveAppImmutableNodes_4HR3DFtNrJs);
    server->on("/app/immutable/nodes/5.B-_jSokT.js", HTTP_GET, serveAppImmutableNodes_5BJSokTJs);
    server->on("/app/immutable/nodes/6.pYVBaJC-.js", HTTP_GET, serveAppImmutableNodes_6PYvBaJcJs);
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
