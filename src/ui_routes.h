/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,148 bytes
 * ui_html: 4,527 bytes
 * ui_app_immutable_entry_js: 67,445 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 98,166 bytes
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
    server->on("/app/immutable/entry/app._1k8zXNd.js", HTTP_GET, serveAppImmutableEntryApp_1k8zXNdJs);
    server->on("/app/immutable/entry/start.B_57LmH0.js", HTTP_GET, serveAppImmutableEntryStartB_57LmH0Js);
    server->on("/app/immutable/nodes/0.C8UYRJCx.js", HTTP_GET, serveAppImmutableNodes_0C8UyrjCxJs);
    server->on("/app/immutable/nodes/1.Dc2ZmBPt.js", HTTP_GET, serveAppImmutableNodes_1Dc2ZmBPtJs);
    server->on("/app/immutable/nodes/2.B1k0b2_k.js", HTTP_GET, serveAppImmutableNodes_2B1k0b2KJs);
    server->on("/app/immutable/nodes/3.CathxLbE.js", HTTP_GET, serveAppImmutableNodes_3CathxLbEJs);
    server->on("/app/immutable/nodes/4.BwXHmmi8.js", HTTP_GET, serveAppImmutableNodes_4BwXHmmi8Js);
    server->on("/app/immutable/nodes/5.Dh7L3JBe.js", HTTP_GET, serveAppImmutableNodes_5Dh7L3JBeJs);
    server->on("/app/immutable/nodes/6.ZofWQp5f.js", HTTP_GET, serveAppImmutableNodes_6ZofWQp5fJs);
    server->on("/app/immutable/nodes/7.C6jDtr5y.js", HTTP_GET, serveAppImmutableNodes_7C6jDtr5yJs);
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
