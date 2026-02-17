/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,344 bytes
 * ui_app_immutable_entry_js: 63,542 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 93,808 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsInternalDAt95k88Css);
    server->on("/app/immutable/assets/start.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsStartDAt95k88Css);
    server->on("/app/immutable/entry/app.3GOWPyqj.js", HTTP_GET, serveAppImmutableEntryApp_3GowPyqjJs);
    server->on("/app/immutable/entry/start.CFkQaC6D.js", HTTP_GET, serveAppImmutableEntryStartCFkQaC6DJs);
    server->on("/app/immutable/nodes/0.mYHjaIW0.js", HTTP_GET, serveAppImmutableNodes_0MYHjaIw0Js);
    server->on("/app/immutable/nodes/1.Cj58JkU8.js", HTTP_GET, serveAppImmutableNodes_1Cj58JkU8Js);
    server->on("/app/immutable/nodes/2.BdHIScUe.js", HTTP_GET, serveAppImmutableNodes_2BdHiScUeJs);
    server->on("/app/immutable/nodes/3.BHXuyOFW.js", HTTP_GET, serveAppImmutableNodes_3BhXuyOfwJs);
    server->on("/app/immutable/nodes/4.Du-2j6LO.js", HTTP_GET, serveAppImmutableNodes_4Du_2j6LoJs);
    server->on("/app/immutable/nodes/5.K68pgbce.js", HTTP_GET, serveAppImmutableNodes_5K68pgbceJs);
    server->on("/app/immutable/nodes/6.P_XXdM4_.js", HTTP_GET, serveAppImmutableNodes_6PXXdM4Js);
    server->on("/app/immutable/nodes/7.CkDPjsGg.js", HTTP_GET, serveAppImmutableNodes_7CkDPjsGgJs);
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
