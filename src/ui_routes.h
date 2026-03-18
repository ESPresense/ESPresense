/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,246 bytes
 * ui_html: 4,332 bytes
 * ui_app_immutable_entry_js: 64,645 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,269 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DxBQeS86.css", HTTP_GET, serveAppImmutableAssetsInternalDxBQeS86Css);
    server->on("/app/immutable/assets/start.DxBQeS86.css", HTTP_GET, serveAppImmutableAssetsStartDxBQeS86Css);
    server->on("/app/immutable/entry/app.mG7Amr7k.js", HTTP_GET, serveAppImmutableEntryAppMG7Amr7kJs);
    server->on("/app/immutable/entry/start.DPlaT82z.js", HTTP_GET, serveAppImmutableEntryStartDPlaT82zJs);
    server->on("/app/immutable/nodes/0.2ckRCKpM.js", HTTP_GET, serveAppImmutableNodes_0_2ckRcKpMJs);
    server->on("/app/immutable/nodes/1.BmWCB2Ua.js", HTTP_GET, serveAppImmutableNodes_1BmWcb2UaJs);
    server->on("/app/immutable/nodes/2.D0DXhjii.js", HTTP_GET, serveAppImmutableNodes_2D0DXhjiiJs);
    server->on("/app/immutable/nodes/3.BaPYIF1u.js", HTTP_GET, serveAppImmutableNodes_3BaPyif1uJs);
    server->on("/app/immutable/nodes/4.BlCjrFkZ.js", HTTP_GET, serveAppImmutableNodes_4BlCjrFkZJs);
    server->on("/app/immutable/nodes/5.CSQZIFIK.js", HTTP_GET, serveAppImmutableNodes_5CsqzifikJs);
    server->on("/app/immutable/nodes/6.iwCKFiiv.js", HTTP_GET, serveAppImmutableNodes_6IwCkFiivJs);
    server->on("/app/immutable/nodes/7.Da2y2KQD.js", HTTP_GET, serveAppImmutableNodes_7Da2y2KqdJs);
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
