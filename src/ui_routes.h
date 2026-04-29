/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,144 bytes
 * ui_html: 4,332 bytes
 * ui_app_immutable_entry_js: 66,581 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 97,103 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.BieM-44V.css", HTTP_GET, serveAppImmutableAssetsInternalBieM_44VCss);
    server->on("/app/immutable/assets/start.BieM-44V.css", HTTP_GET, serveAppImmutableAssetsStartBieM_44VCss);
    server->on("/app/immutable/entry/app.BADTFL9B.js", HTTP_GET, serveAppImmutableEntryAppBadtfl9BJs);
    server->on("/app/immutable/entry/start.DBGOgp0j.js", HTTP_GET, serveAppImmutableEntryStartDbgOgp0jJs);
    server->on("/app/immutable/nodes/0.Buu7_HQp.js", HTTP_GET, serveAppImmutableNodes_0Buu7HQpJs);
    server->on("/app/immutable/nodes/1.tbaWs5ZA.js", HTTP_GET, serveAppImmutableNodes_1TbaWs5ZaJs);
    server->on("/app/immutable/nodes/2.s2rP0TFX.js", HTTP_GET, serveAppImmutableNodes_2S2rP0TfxJs);
    server->on("/app/immutable/nodes/3.CCZFVpf_.js", HTTP_GET, serveAppImmutableNodes_3CczfVpfJs);
    server->on("/app/immutable/nodes/4.BanZAzpV.js", HTTP_GET, serveAppImmutableNodes_4BanZAzpVJs);
    server->on("/app/immutable/nodes/5.Er20dwNq.js", HTTP_GET, serveAppImmutableNodes_5Er20dwNqJs);
    server->on("/app/immutable/nodes/6.BHlOj2Iy.js", HTTP_GET, serveAppImmutableNodes_6BHlOj2IyJs);
    server->on("/app/immutable/nodes/7.BHRsKnvD.js", HTTP_GET, serveAppImmutableNodes_7BhRsKnvDJs);
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
