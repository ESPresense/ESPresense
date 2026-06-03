/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,148 bytes
 * ui_html: 4,540 bytes
 * ui_app_immutable_entry_js: 67,306 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 98,040 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DJlzcx4I.css", HTTP_GET, serveAppImmutableAssetsInternalDJlzcx4ICss);
    server->on("/app/immutable/assets/start.DJlzcx4I.css", HTTP_GET, serveAppImmutableAssetsStartDJlzcx4ICss);
    server->on("/app/immutable/entry/app.BQyTEVaT.js", HTTP_GET, serveAppImmutableEntryAppBQyTeVaTJs);
    server->on("/app/immutable/entry/start.TzP3uYHj.js", HTTP_GET, serveAppImmutableEntryStartTzP3uYHjJs);
    server->on("/app/immutable/nodes/0.Bc_X0RS7.js", HTTP_GET, serveAppImmutableNodes_0BcX0Rs7Js);
    server->on("/app/immutable/nodes/1.Be24Lfbp.js", HTTP_GET, serveAppImmutableNodes_1Be24LfbpJs);
    server->on("/app/immutable/nodes/2.DhZ5kT0i.js", HTTP_GET, serveAppImmutableNodes_2DhZ5kT0iJs);
    server->on("/app/immutable/nodes/3.CUoXEI78.js", HTTP_GET, serveAppImmutableNodes_3CUoXei78Js);
    server->on("/app/immutable/nodes/4.UqDGPAi3.js", HTTP_GET, serveAppImmutableNodes_4UqDgpAi3Js);
    server->on("/app/immutable/nodes/5.BFN7Oarq.js", HTTP_GET, serveAppImmutableNodes_5Bfn7OarqJs);
    server->on("/app/immutable/nodes/6.B-NxU3xx.js", HTTP_GET, serveAppImmutableNodes_6BNxU3xxJs);
    server->on("/app/immutable/nodes/7.tOPlylz1.js", HTTP_GET, serveAppImmutableNodes_7TOPlylz1Js);
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
