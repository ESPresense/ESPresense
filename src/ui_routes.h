/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,361 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 48,769 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 63,350 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.B89G9FIo.css", HTTP_GET, serveAppImmutableAssetsInternalB89G9FIoCss);
    server->on("/app/immutable/assets/start.D-jtWh4c.css", HTTP_GET, serveAppImmutableAssetsStartDJtWh4cCss);
    server->on("/app/immutable/entry/app.CidPB3iv.js", HTTP_GET, serveAppImmutableEntryAppCidPb3ivJs);
    server->on("/app/immutable/entry/start.7ijBQCQR.js", HTTP_GET, serveAppImmutableEntryStart_7ijBqcqrJs);
    server->on("/app/immutable/nodes/0.CDhdCXyv.js", HTTP_GET, serveAppImmutableNodes_0CDhdCXyvJs);
    server->on("/app/immutable/nodes/1.Wk_M4jqH.js", HTTP_GET, serveAppImmutableNodes_1WkM4jqHJs);
    server->on("/app/immutable/nodes/2.2Z3T6Hmg.js", HTTP_GET, serveAppImmutableNodes_2_2Z3T6HmgJs);
    server->on("/app/immutable/nodes/3.C3dDEmRi.js", HTTP_GET, serveAppImmutableNodes_3C3dDEmRiJs);
    server->on("/app/immutable/nodes/4.CHVof8kx.js", HTTP_GET, serveAppImmutableNodes_4ChVof8kxJs);
    server->on("/app/immutable/nodes/5.D-7Hx3ri.js", HTTP_GET, serveAppImmutableNodes_5D_7Hx3riJs);
    server->on("/app/immutable/nodes/6.CLnlwwY7.js", HTTP_GET, serveAppImmutableNodes_6CLnlwwY7Js);
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
