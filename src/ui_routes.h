/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,962 bytes
 * ui_html: 4,338 bytes
 * ui_app_immutable_entry_js: 63,291 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 93,637 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CrYon3pw.css", HTTP_GET, serveAppImmutableAssetsInternalCrYon3pwCss);
    server->on("/app/immutable/assets/start.CrYon3pw.css", HTTP_GET, serveAppImmutableAssetsStartCrYon3pwCss);
    server->on("/app/immutable/entry/app.BLz96HYu.js", HTTP_GET, serveAppImmutableEntryAppBLz96HYuJs);
    server->on("/app/immutable/entry/start.AWO8Wg8q.js", HTTP_GET, serveAppImmutableEntryStartAwo8Wg8qJs);
    server->on("/app/immutable/nodes/0.BHMHrh9G.js", HTTP_GET, serveAppImmutableNodes_0BhmHrh9GJs);
    server->on("/app/immutable/nodes/1.DGdRKW9S.js", HTTP_GET, serveAppImmutableNodes_1DGdRkw9SJs);
    server->on("/app/immutable/nodes/2.BaQbROtT.js", HTTP_GET, serveAppImmutableNodes_2BaQbROtTJs);
    server->on("/app/immutable/nodes/3.CuPuCvxI.js", HTTP_GET, serveAppImmutableNodes_3CuPuCvxIJs);
    server->on("/app/immutable/nodes/4.B95yRHMy.js", HTTP_GET, serveAppImmutableNodes_4B95yRhMyJs);
    server->on("/app/immutable/nodes/5.B-HGNg1G.js", HTTP_GET, serveAppImmutableNodes_5BHgNg1GJs);
    server->on("/app/immutable/nodes/6.ZQuxyqwc.js", HTTP_GET, serveAppImmutableNodes_6ZQuxyqwcJs);
    server->on("/app/immutable/nodes/7.BnDw1J5G.js", HTTP_GET, serveAppImmutableNodes_7BnDw1J5GJs);
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
