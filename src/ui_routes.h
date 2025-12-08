/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,410 bytes
 * ui_html: 3,240 bytes
 * ui_app_immutable_entry_js: 66,374 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 80,999 bytes
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
    server->on("/app/immutable/entry/app.D4kleYbY.js", HTTP_GET, serveAppImmutableEntryAppD4kleYbYJs);
    server->on("/app/immutable/entry/start.CmpB2fiJ.js", HTTP_GET, serveAppImmutableEntryStartCmpB2fiJJs);
    server->on("/app/immutable/nodes/0.g1QCP0lk.js", HTTP_GET, serveAppImmutableNodes_0G1Qcp0lkJs);
    server->on("/app/immutable/nodes/1.BXVcmbTQ.js", HTTP_GET, serveAppImmutableNodes_1BxVcmbTqJs);
    server->on("/app/immutable/nodes/2.SR90M7uz.js", HTTP_GET, serveAppImmutableNodes_2Sr90M7uzJs);
    server->on("/app/immutable/nodes/3.CTSBX1Jt.js", HTTP_GET, serveAppImmutableNodes_3Ctsbx1JtJs);
    server->on("/app/immutable/nodes/4.CpetZr9o.js", HTTP_GET, serveAppImmutableNodes_4CpetZr9oJs);
    server->on("/app/immutable/nodes/5.DztGD_xe.js", HTTP_GET, serveAppImmutableNodes_5DztGdXeJs);
    server->on("/app/immutable/nodes/6.Dtv6ipe7.js", HTTP_GET, serveAppImmutableNodes_6Dtv6ipe7Js);
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
