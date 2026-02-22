/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,332 bytes
 * ui_app_immutable_entry_js: 63,840 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,094 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsInternalDAt95k88Css);
    server->on("/app/immutable/assets/start.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsStartDAt95k88Css);
    server->on("/app/immutable/entry/app.WoJo2rs8.js", HTTP_GET, serveAppImmutableEntryAppWoJo2rs8Js);
    server->on("/app/immutable/entry/start.C4OO_E_3.js", HTTP_GET, serveAppImmutableEntryStartC4OoE_3Js);
    server->on("/app/immutable/nodes/0.CLAHeaqO.js", HTTP_GET, serveAppImmutableNodes_0ClaHeaqOJs);
    server->on("/app/immutable/nodes/1.rXSnN7f-.js", HTTP_GET, serveAppImmutableNodes_1RXSnN7fJs);
    server->on("/app/immutable/nodes/2.BYJeDPz5.js", HTTP_GET, serveAppImmutableNodes_2ByJeDPz5Js);
    server->on("/app/immutable/nodes/3.DSSdUEcw.js", HTTP_GET, serveAppImmutableNodes_3DsSdUEcwJs);
    server->on("/app/immutable/nodes/4.DVqwjcjo.js", HTTP_GET, serveAppImmutableNodes_4DVqwjcjoJs);
    server->on("/app/immutable/nodes/5.CnEIOJpr.js", HTTP_GET, serveAppImmutableNodes_5CnEioJprJs);
    server->on("/app/immutable/nodes/6.E90XCcFZ.js", HTTP_GET, serveAppImmutableNodes_6E90XCcFzJs);
    server->on("/app/immutable/nodes/7.Fe8Av7vJ.js", HTTP_GET, serveAppImmutableNodes_7Fe8Av7vJJs);
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
