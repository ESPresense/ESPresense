/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,365 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 48,495 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 63,075 bytes
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
    server->on("/app/immutable/entry/app.B00DVNe2.js", HTTP_GET, serveAppImmutableEntryAppB00DvNe2Js);
    server->on("/app/immutable/entry/start.BHZu2Wku.js", HTTP_GET, serveAppImmutableEntryStartBhZu2WkuJs);
    server->on("/app/immutable/nodes/0.CLkNxYdz.js", HTTP_GET, serveAppImmutableNodes_0CLkNxYdzJs);
    server->on("/app/immutable/nodes/1.SYHIOpa3.js", HTTP_GET, serveAppImmutableNodes_1SyhiOpa3Js);
    server->on("/app/immutable/nodes/2.DQlchPSc.js", HTTP_GET, serveAppImmutableNodes_2DQlchPScJs);
    server->on("/app/immutable/nodes/3.BFwR2RVq.js", HTTP_GET, serveAppImmutableNodes_3BFwR2RVqJs);
    server->on("/app/immutable/nodes/4.swD-kCaj.js", HTTP_GET, serveAppImmutableNodes_4SwDKCajJs);
    server->on("/app/immutable/nodes/5.Xcaj0brH.js", HTTP_GET, serveAppImmutableNodes_5Xcaj0brHJs);
    server->on("/app/immutable/nodes/6.CQ0NAF4L.js", HTTP_GET, serveAppImmutableNodes_6Cq0Naf4LJs);
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
