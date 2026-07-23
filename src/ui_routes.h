/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,138 bytes
 * ui_html: 4,524 bytes
 * ui_app_immutable_entry_js: 67,513 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 98,221 bytes
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
    server->on("/app/immutable/entry/app.DXOe-d1M.js", HTTP_GET, serveAppImmutableEntryAppDxOeD1MJs);
    server->on("/app/immutable/entry/start.Bdx8C08T.js", HTTP_GET, serveAppImmutableEntryStartBdx8C08TJs);
    server->on("/app/immutable/nodes/0.CJ9TgrwB.js", HTTP_GET, serveAppImmutableNodes_0Cj9TgrwBJs);
    server->on("/app/immutable/nodes/1.i0YlaLV2.js", HTTP_GET, serveAppImmutableNodes_1I0YlaLv2Js);
    server->on("/app/immutable/nodes/2.DrUPvmrV.js", HTTP_GET, serveAppImmutableNodes_2DrUPvmrVJs);
    server->on("/app/immutable/nodes/3.C9GNblyF.js", HTTP_GET, serveAppImmutableNodes_3C9GNblyFJs);
    server->on("/app/immutable/nodes/4.JYh3NAYi.js", HTTP_GET, serveAppImmutableNodes_4JYh3NaYiJs);
    server->on("/app/immutable/nodes/5.BPVwG2j2.js", HTTP_GET, serveAppImmutableNodes_5BpVwG2j2Js);
    server->on("/app/immutable/nodes/6.Cc4IUOFz.js", HTTP_GET, serveAppImmutableNodes_6Cc4IuoFzJs);
    server->on("/app/immutable/nodes/7.DWg9QgrZ.js", HTTP_GET, serveAppImmutableNodes_7DWg9QgrZJs);
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
