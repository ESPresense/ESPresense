/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,238 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 45,085 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 59,543 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.BmRJnoOK.css", HTTP_GET, serveAppImmutableAssetsInternalBmRJnoOkCss);
    server->on("/app/immutable/assets/start.78t_RGpI.css", HTTP_GET, serveAppImmutableAssetsStart_78tRGpICss);
    server->on("/app/immutable/entry/app.C2-rM-r9.js", HTTP_GET, serveAppImmutableEntryAppC2RMR9Js);
    server->on("/app/immutable/entry/start.DRG0jkgd.js", HTTP_GET, serveAppImmutableEntryStartDrg0jkgdJs);
    server->on("/app/immutable/nodes/0.xfH5iXDw.js", HTTP_GET, serveAppImmutableNodes_0XfH5iXDwJs);
    server->on("/app/immutable/nodes/1.jRjEfLFM.js", HTTP_GET, serveAppImmutableNodes_1JRjEfLfmJs);
    server->on("/app/immutable/nodes/2.CEIETh5E.js", HTTP_GET, serveAppImmutableNodes_2CeieTh5EJs);
    server->on("/app/immutable/nodes/3.xgDJqtR6.js", HTTP_GET, serveAppImmutableNodes_3XgDJqtR6Js);
    server->on("/app/immutable/nodes/4.C7Wqkq3H.js", HTTP_GET, serveAppImmutableNodes_4C7Wqkq3HJs);
    server->on("/app/immutable/nodes/5.CbERAbpy.js", HTTP_GET, serveAppImmutableNodes_5CbErAbpyJs);
    server->on("/app/immutable/nodes/6.C0xdOPyq.js", HTTP_GET, serveAppImmutableNodes_6C0xdOPyqJs);
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
