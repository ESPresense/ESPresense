/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,571 bytes
 * ui_html: 3,900 bytes
 * ui_app_immutable_entry_js: 66,612 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 82,129 bytes
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
    server->on("/app/immutable/entry/app.DdBSHxuR.js", HTTP_GET, serveAppImmutableEntryAppDdBsHxuRJs);
    server->on("/app/immutable/entry/start.Hxk_S471.js", HTTP_GET, serveAppImmutableEntryStartHxkS471Js);
    server->on("/app/immutable/nodes/0.EQde_CmK.js", HTTP_GET, serveAppImmutableNodes_0EQdeCmKJs);
    server->on("/app/immutable/nodes/1.BpXbLNdz.js", HTTP_GET, serveAppImmutableNodes_1BpXbLNdzJs);
    server->on("/app/immutable/nodes/2.DKCnQvGj.js", HTTP_GET, serveAppImmutableNodes_2DkCnQvGjJs);
    server->on("/app/immutable/nodes/3.C3Zcf2P9.js", HTTP_GET, serveAppImmutableNodes_3C3Zcf2P9Js);
    server->on("/app/immutable/nodes/4.DXNfXbGT.js", HTTP_GET, serveAppImmutableNodes_4DxNfXbGtJs);
    server->on("/app/immutable/nodes/5.DB14TRl9.js", HTTP_GET, serveAppImmutableNodes_5Db14TRl9Js);
    server->on("/app/immutable/nodes/6.DRxt8Lzl.js", HTTP_GET, serveAppImmutableNodes_6DRxt8LzlJs);
    server->on("/app/immutable/nodes/7.ByGxYuZV.js", HTTP_GET, serveAppImmutableNodes_7ByGxYuZvJs);
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
