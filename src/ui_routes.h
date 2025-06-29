/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 9,145 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 45,012 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 58,372 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.Cdi-LVYQ.css", HTTP_GET, serveAppImmutableAssetsInternalCdiLvyqCss);
    server->on("/app/immutable/assets/start.CJnjhWGH.css", HTTP_GET, serveAppImmutableAssetsStartCJnjhWghCss);
    server->on("/app/immutable/entry/app.BqWR24Wl.js", HTTP_GET, serveAppImmutableEntryAppBqWr24WlJs);
    server->on("/app/immutable/entry/start.CdT-Q7FA.js", HTTP_GET, serveAppImmutableEntryStartCdTQ7FaJs);
    server->on("/app/immutable/nodes/0.DdZ1NaoW.js", HTTP_GET, serveAppImmutableNodes_0DdZ1NaoWJs);
    server->on("/app/immutable/nodes/1.DvjF4vlF.js", HTTP_GET, serveAppImmutableNodes_1DvjF4vlFJs);
    server->on("/app/immutable/nodes/2.DQIjqvCj.js", HTTP_GET, serveAppImmutableNodes_2DqIjqvCjJs);
    server->on("/app/immutable/nodes/3.3xRZYOvF.js", HTTP_GET, serveAppImmutableNodes_3_3xRzyOvFJs);
    server->on("/app/immutable/nodes/4.DtEH9lr4.js", HTTP_GET, serveAppImmutableNodes_4DtEh9lr4Js);
    server->on("/app/immutable/nodes/5.IAGVT67U.js", HTTP_GET, serveAppImmutableNodes_5Iagvt67UJs);
    server->on("/app/immutable/nodes/6.EY3FCza3.js", HTTP_GET, serveAppImmutableNodes_6Ey3FCza3Js);
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
