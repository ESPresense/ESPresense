/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,960 bytes
 * ui_html: 4,326 bytes
 * ui_app_immutable_entry_js: 63,821 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,153 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CBvFVl_F.css", HTTP_GET, serveAppImmutableAssetsInternalCBvFVlFCss);
    server->on("/app/immutable/assets/start.CBvFVl_F.css", HTTP_GET, serveAppImmutableAssetsStartCBvFVlFCss);
    server->on("/app/immutable/entry/app.DS980rdP.js", HTTP_GET, serveAppImmutableEntryAppDs980rdPJs);
    server->on("/app/immutable/entry/start.Bv49SYtc.js", HTTP_GET, serveAppImmutableEntryStartBv49SYtcJs);
    server->on("/app/immutable/nodes/0.j9VCyCoZ.js", HTTP_GET, serveAppImmutableNodes_0J9VCyCoZJs);
    server->on("/app/immutable/nodes/1.CdGO_bhs.js", HTTP_GET, serveAppImmutableNodes_1CdGoBhsJs);
    server->on("/app/immutable/nodes/2.Bx4DIHUj.js", HTTP_GET, serveAppImmutableNodes_2Bx4DihUjJs);
    server->on("/app/immutable/nodes/3.BIxdjYum.js", HTTP_GET, serveAppImmutableNodes_3BIxdjYumJs);
    server->on("/app/immutable/nodes/4.C9qNIIlk.js", HTTP_GET, serveAppImmutableNodes_4C9qNiIlkJs);
    server->on("/app/immutable/nodes/5.74sYQ-4M.js", HTTP_GET, serveAppImmutableNodes_5_74sYq_4MJs);
    server->on("/app/immutable/nodes/6.BO3lIpQu.js", HTTP_GET, serveAppImmutableNodes_6Bo3lIpQuJs);
    server->on("/app/immutable/nodes/7.DC-TgIrw.js", HTTP_GET, serveAppImmutableNodes_7DcTgIrwJs);
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
