/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,522 bytes
 * ui_html: 3,888 bytes
 * ui_app_immutable_entry_js: 62,555 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 92,011 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.Bb3R8x5k.css", HTTP_GET, serveAppImmutableAssetsInternalBb3R8x5kCss);
    server->on("/app/immutable/assets/start.Bb3R8x5k.css", HTTP_GET, serveAppImmutableAssetsStartBb3R8x5kCss);
    server->on("/app/immutable/entry/app.BboDC2p-.js", HTTP_GET, serveAppImmutableEntryAppBboDc2pJs);
    server->on("/app/immutable/entry/start.iau1sbY5.js", HTTP_GET, serveAppImmutableEntryStartIau1sbY5Js);
    server->on("/app/immutable/nodes/0.BUuVBfej.js", HTTP_GET, serveAppImmutableNodes_0BUuVBfejJs);
    server->on("/app/immutable/nodes/1.QLydoQyV.js", HTTP_GET, serveAppImmutableNodes_1QLydoQyVJs);
    server->on("/app/immutable/nodes/2.BRFmpmII.js", HTTP_GET, serveAppImmutableNodes_2BrFmpmIiJs);
    server->on("/app/immutable/nodes/3.B7663bGa.js", HTTP_GET, serveAppImmutableNodes_3B7663bGaJs);
    server->on("/app/immutable/nodes/4.HPnLErl4.js", HTTP_GET, serveAppImmutableNodes_4HPnLErl4Js);
    server->on("/app/immutable/nodes/5.4_ydPcjU.js", HTTP_GET, serveAppImmutableNodes_5_4YdPcjUJs);
    server->on("/app/immutable/nodes/6.DjytGVB8.js", HTTP_GET, serveAppImmutableNodes_6DjytGvb8Js);
    server->on("/app/immutable/nodes/7.GhF_t3aA.js", HTTP_GET, serveAppImmutableNodes_7GhFT3aAJs);
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
