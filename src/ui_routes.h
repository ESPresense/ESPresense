/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,365 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 47,971 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 62,556 bytes
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
    server->on("/app/immutable/entry/app.CBgyclO5.js", HTTP_GET, serveAppImmutableEntryAppCBgyclO5Js);
    server->on("/app/immutable/entry/start.DTJsyign.js", HTTP_GET, serveAppImmutableEntryStartDtJsyignJs);
    server->on("/app/immutable/nodes/0.D-HwNwD6.js", HTTP_GET, serveAppImmutableNodes_0DHwNwD6Js);
    server->on("/app/immutable/nodes/1.1VkRPaU4.js", HTTP_GET, serveAppImmutableNodes_1_1VkRPaU4Js);
    server->on("/app/immutable/nodes/2.GA-V4_ue.js", HTTP_GET, serveAppImmutableNodes_2GaV4UeJs);
    server->on("/app/immutable/nodes/3.Bp1mYC9h.js", HTTP_GET, serveAppImmutableNodes_3Bp1mYc9hJs);
    server->on("/app/immutable/nodes/4.Ct3Q6B3r.js", HTTP_GET, serveAppImmutableNodes_4Ct3Q6B3rJs);
    server->on("/app/immutable/nodes/5.BMK-vAAC.js", HTTP_GET, serveAppImmutableNodes_5BmkVAacJs);
    server->on("/app/immutable/nodes/6.Cunm0JQZ.js", HTTP_GET, serveAppImmutableNodes_6Cunm0JqzJs);
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
