/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,571 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 66,404 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 81,210 bytes
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
    server->on("/app/immutable/entry/app.5X8VD7p-.js", HTTP_GET, serveAppImmutableEntryApp_5X8Vd7pJs);
    server->on("/app/immutable/entry/start.BhEmd4p2.js", HTTP_GET, serveAppImmutableEntryStartBhEmd4p2Js);
    server->on("/app/immutable/nodes/0.BNc_eF9A.js", HTTP_GET, serveAppImmutableNodes_0BNcEF9AJs);
    server->on("/app/immutable/nodes/1.BamT2GUg.js", HTTP_GET, serveAppImmutableNodes_1BamT2GUgJs);
    server->on("/app/immutable/nodes/2.CJFSWBbs.js", HTTP_GET, serveAppImmutableNodes_2CjfswBbsJs);
    server->on("/app/immutable/nodes/3.D3id52Si.js", HTTP_GET, serveAppImmutableNodes_3D3id52SiJs);
    server->on("/app/immutable/nodes/4.PyYzcBVj.js", HTTP_GET, serveAppImmutableNodes_4PyYzcBVjJs);
    server->on("/app/immutable/nodes/5.BA2ZYEsf.js", HTTP_GET, serveAppImmutableNodes_5Ba2ZyEsfJs);
    server->on("/app/immutable/nodes/6.vIZyAZE6.js", HTTP_GET, serveAppImmutableNodes_6VIZyAze6Js);
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
