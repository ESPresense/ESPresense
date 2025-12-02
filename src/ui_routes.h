/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,410 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 66,407 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 81,047 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.VjTAGEOH.css", HTTP_GET, serveAppImmutableAssetsInternalVjTageohCss);
    server->on("/app/immutable/assets/start.Bt_nQDmA.css", HTTP_GET, serveAppImmutableAssetsStartBtNQDmACss);
    server->on("/app/immutable/entry/app.5fkh0CE8.js", HTTP_GET, serveAppImmutableEntryApp_5fkh0Ce8Js);
    server->on("/app/immutable/entry/start.y8AIAWBS.js", HTTP_GET, serveAppImmutableEntryStartY8AiawbsJs);
    server->on("/app/immutable/nodes/0.TeZxRP06.js", HTTP_GET, serveAppImmutableNodes_0TeZxRp06Js);
    server->on("/app/immutable/nodes/1.BAs2Am4S.js", HTTP_GET, serveAppImmutableNodes_1BAs2Am4SJs);
    server->on("/app/immutable/nodes/2.B_w5NsiI.js", HTTP_GET, serveAppImmutableNodes_2BW5NsiIJs);
    server->on("/app/immutable/nodes/3.CuyX5m5y.js", HTTP_GET, serveAppImmutableNodes_3CuyX5m5yJs);
    server->on("/app/immutable/nodes/4.DnCmD_fM.js", HTTP_GET, serveAppImmutableNodes_4DnCmDFMJs);
    server->on("/app/immutable/nodes/5.BmtmYmEV.js", HTTP_GET, serveAppImmutableNodes_5BmtmYmEvJs);
    server->on("/app/immutable/nodes/6.0KgsP2DW.js", HTTP_GET, serveAppImmutableNodes_6_0KgsP2DwJs);
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
