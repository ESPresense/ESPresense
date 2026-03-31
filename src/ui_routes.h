/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,144 bytes
 * ui_html: 4,332 bytes
 * ui_app_immutable_entry_js: 64,935 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,457 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.BieM-44V.css", HTTP_GET, serveAppImmutableAssetsInternalBieM_44VCss);
    server->on("/app/immutable/assets/start.BieM-44V.css", HTTP_GET, serveAppImmutableAssetsStartBieM_44VCss);
    server->on("/app/immutable/entry/app.VxkZ2C6s.js", HTTP_GET, serveAppImmutableEntryAppVxkZ2C6sJs);
    server->on("/app/immutable/entry/start.U2IvtoJV.js", HTTP_GET, serveAppImmutableEntryStartU2IvtoJvJs);
    server->on("/app/immutable/nodes/0.CDP5uoeV.js", HTTP_GET, serveAppImmutableNodes_0Cdp5uoeVJs);
    server->on("/app/immutable/nodes/1.B-XXnnPx.js", HTTP_GET, serveAppImmutableNodes_1BXXnnPxJs);
    server->on("/app/immutable/nodes/2.BEZ3ZILc.js", HTTP_GET, serveAppImmutableNodes_2Bez3ZiLcJs);
    server->on("/app/immutable/nodes/3.-sBj3UsF.js", HTTP_GET, serveAppImmutableNodes_3SBj3UsFJs);
    server->on("/app/immutable/nodes/4.sQ_WmkHu.js", HTTP_GET, serveAppImmutableNodes_4SQWmkHuJs);
    server->on("/app/immutable/nodes/5.BwJ2sAvV.js", HTTP_GET, serveAppImmutableNodes_5BwJ2sAvVJs);
    server->on("/app/immutable/nodes/6.CswxJDO8.js", HTTP_GET, serveAppImmutableNodes_6CswxJdo8Js);
    server->on("/app/immutable/nodes/7.3hHeWylg.js", HTTP_GET, serveAppImmutableNodes_7_3hHeWylgJs);
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
