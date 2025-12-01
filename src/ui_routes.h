/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,571 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 66,319 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 81,115 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.wrGhOo2C.css", HTTP_GET, serveAppImmutableAssetsInternalWrGhOo2CCss);
    server->on("/app/immutable/assets/start.DOTUP31V.css", HTTP_GET, serveAppImmutableAssetsStartDotup31VCss);
    server->on("/app/immutable/entry/app.Dlfwnw8f.js", HTTP_GET, serveAppImmutableEntryAppDlfwnw8fJs);
    server->on("/app/immutable/entry/start.Bdja-Pt-.js", HTTP_GET, serveAppImmutableEntryStartBdjaPtJs);
    server->on("/app/immutable/nodes/0.CtnE88BT.js", HTTP_GET, serveAppImmutableNodes_0CtnE88BtJs);
    server->on("/app/immutable/nodes/1.BOuy1jBU.js", HTTP_GET, serveAppImmutableNodes_1BOuy1jBuJs);
    server->on("/app/immutable/nodes/2.DcgSiZyJ.js", HTTP_GET, serveAppImmutableNodes_2DcgSiZyJJs);
    server->on("/app/immutable/nodes/3.gHyMJtZi.js", HTTP_GET, serveAppImmutableNodes_3GHyMJtZiJs);
    server->on("/app/immutable/nodes/4.DrvKl8R_.js", HTTP_GET, serveAppImmutableNodes_4DrvKl8RJs);
    server->on("/app/immutable/nodes/5.Csf8V7mo.js", HTTP_GET, serveAppImmutableNodes_5Csf8V7moJs);
    server->on("/app/immutable/nodes/6.Bh2eTW7z.js", HTTP_GET, serveAppImmutableNodes_6Bh2eTw7zJs);
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
