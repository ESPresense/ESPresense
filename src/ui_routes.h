/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,365 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 50,231 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 64,811 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.T25hC3XV.css", HTTP_GET, serveAppImmutableAssetsInternalT25hC3XvCss);
    server->on("/app/immutable/assets/start.BbZCJWr7.css", HTTP_GET, serveAppImmutableAssetsStartBbZcjWr7Css);
    server->on("/app/immutable/entry/app.o1fdHe0X.js", HTTP_GET, serveAppImmutableEntryAppO1fdHe0XJs);
    server->on("/app/immutable/entry/start.B0oALr6_.js", HTTP_GET, serveAppImmutableEntryStartB0oALr6Js);
    server->on("/app/immutable/nodes/0.D6LqETst.js", HTTP_GET, serveAppImmutableNodes_0D6LqETstJs);
    server->on("/app/immutable/nodes/1.C7QlD7_V.js", HTTP_GET, serveAppImmutableNodes_1C7QlD7VJs);
    server->on("/app/immutable/nodes/2.5-gRUTcJ.js", HTTP_GET, serveAppImmutableNodes_2_5GRuTcJJs);
    server->on("/app/immutable/nodes/3.CXP7lf-R.js", HTTP_GET, serveAppImmutableNodes_3Cxp7lfRJs);
    server->on("/app/immutable/nodes/4.B_Tu9R9g.js", HTTP_GET, serveAppImmutableNodes_4BTu9R9gJs);
    server->on("/app/immutable/nodes/5.DnRX46SD.js", HTTP_GET, serveAppImmutableNodes_5DnRx46SdJs);
    server->on("/app/immutable/nodes/6.ER6P2UjV.js", HTTP_GET, serveAppImmutableNodes_6Er6P2UjVJs);
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
