/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,265 bytes
 * ui_app_immutable_entry_js: 45,045 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 58,153 bytes
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
    server->on("/app/immutable/entry/app.CfyJZIZY.js", HTTP_GET, serveAppImmutableEntryAppCfyJzizyJs);
    server->on("/app/immutable/entry/start.XyUY7WBW.js", HTTP_GET, serveAppImmutableEntryStartXyUy7WbwJs);
    server->on("/app/immutable/nodes/0.D3_ADpUQ.js", HTTP_GET, serveAppImmutableNodes_0D3ADpUqJs);
    server->on("/app/immutable/nodes/1.Bb-SZsvF.js", HTTP_GET, serveAppImmutableNodes_1BbSZsvFJs);
    server->on("/app/immutable/nodes/2.D0GOPS6z.js", HTTP_GET, serveAppImmutableNodes_2D0Gops6zJs);
    server->on("/app/immutable/nodes/3.Cqf9Kge0.js", HTTP_GET, serveAppImmutableNodes_3Cqf9Kge0Js);
    server->on("/app/immutable/nodes/4.CC-WHKww.js", HTTP_GET, serveAppImmutableNodes_4CcWhKwwJs);
    server->on("/app/immutable/nodes/5.DpeQG-Tu.js", HTTP_GET, serveAppImmutableNodes_5DpeQgTuJs);
    server->on("/app/immutable/nodes/6.B7e9l27-.js", HTTP_GET, serveAppImmutableNodes_6B7e9l27Js);
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
