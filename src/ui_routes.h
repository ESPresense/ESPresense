/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,571 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 66,264 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 81,065 bytes
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
    server->on("/app/immutable/entry/app.DOYJ0J8U.js", HTTP_GET, serveAppImmutableEntryAppDoyj0J8UJs);
    server->on("/app/immutable/entry/start.BtwkTc4w.js", HTTP_GET, serveAppImmutableEntryStartBtwkTc4wJs);
    server->on("/app/immutable/nodes/0.D5b7VYb9.js", HTTP_GET, serveAppImmutableNodes_0D5b7VYb9Js);
    server->on("/app/immutable/nodes/1.gRmJA8Pn.js", HTTP_GET, serveAppImmutableNodes_1GRmJa8PnJs);
    server->on("/app/immutable/nodes/2.BeDZpEIH.js", HTTP_GET, serveAppImmutableNodes_2BeDZpEihJs);
    server->on("/app/immutable/nodes/3.CVCmLnwN.js", HTTP_GET, serveAppImmutableNodes_3CvCmLnwNJs);
    server->on("/app/immutable/nodes/4.9UUJoDeq.js", HTTP_GET, serveAppImmutableNodes_4_9UuJoDeqJs);
    server->on("/app/immutable/nodes/5.Cfd_l-N-.js", HTTP_GET, serveAppImmutableNodes_5CfdLNJs);
    server->on("/app/immutable/nodes/6.DInJabSr.js", HTTP_GET, serveAppImmutableNodes_6DInJabSrJs);
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
