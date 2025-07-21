/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,210 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 47,350 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 61,780 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.R2nWuKlp.css", HTTP_GET, serveAppImmutableAssetsInternalR2nWuKlpCss);
    server->on("/app/immutable/assets/start.CquYjHqP.css", HTTP_GET, serveAppImmutableAssetsStartCquYjHqPCss);
    server->on("/app/immutable/entry/app.BiavsjQz.js", HTTP_GET, serveAppImmutableEntryAppBiavsjQzJs);
    server->on("/app/immutable/entry/start.RTvM1e7q.js", HTTP_GET, serveAppImmutableEntryStartRTvM1e7qJs);
    server->on("/app/immutable/nodes/0.CwpDaxuo.js", HTTP_GET, serveAppImmutableNodes_0CwpDaxuoJs);
    server->on("/app/immutable/nodes/1.Bp94P4G6.js", HTTP_GET, serveAppImmutableNodes_1Bp94P4G6Js);
    server->on("/app/immutable/nodes/2.D-CrPzRb.js", HTTP_GET, serveAppImmutableNodes_2DCrPzRbJs);
    server->on("/app/immutable/nodes/3.4PAQXAbo.js", HTTP_GET, serveAppImmutableNodes_3_4PaqxAboJs);
    server->on("/app/immutable/nodes/4.B8GNKU8W.js", HTTP_GET, serveAppImmutableNodes_4B8Gnku8WJs);
    server->on("/app/immutable/nodes/5.CZ5CFJdr.js", HTTP_GET, serveAppImmutableNodes_5Cz5CfJdrJs);
    server->on("/app/immutable/nodes/6.GJ6t8JZn.js", HTTP_GET, serveAppImmutableNodes_6Gj6t8JZnJs);
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
