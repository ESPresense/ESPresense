/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,138 bytes
 * ui_html: 4,526 bytes
 * ui_app_immutable_entry_js: 67,333 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 98,043 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DPaphot8.css", HTTP_GET, serveAppImmutableAssetsInternalDPaphot8Css);
    server->on("/app/immutable/assets/start.DPaphot8.css", HTTP_GET, serveAppImmutableAssetsStartDPaphot8Css);
    server->on("/app/immutable/entry/app.BTa2MlEn.js", HTTP_GET, serveAppImmutableEntryAppBTa2MlEnJs);
    server->on("/app/immutable/entry/start.CPQ0EE94.js", HTTP_GET, serveAppImmutableEntryStartCpq0Ee94Js);
    server->on("/app/immutable/nodes/0.LfKfYl16.js", HTTP_GET, serveAppImmutableNodes_0LfKfYl16Js);
    server->on("/app/immutable/nodes/1.BYkjBpev.js", HTTP_GET, serveAppImmutableNodes_1BYkjBpevJs);
    server->on("/app/immutable/nodes/2.DrBmI3Tq.js", HTTP_GET, serveAppImmutableNodes_2DrBmI3TqJs);
    server->on("/app/immutable/nodes/3.CmD2rwYi.js", HTTP_GET, serveAppImmutableNodes_3CmD2rwYiJs);
    server->on("/app/immutable/nodes/4.COY1lM9G.js", HTTP_GET, serveAppImmutableNodes_4Coy1lM9GJs);
    server->on("/app/immutable/nodes/5.ByiJ9a3u.js", HTTP_GET, serveAppImmutableNodes_5ByiJ9a3uJs);
    server->on("/app/immutable/nodes/6.D6EjG0Re.js", HTTP_GET, serveAppImmutableNodes_6D6EjG0ReJs);
    server->on("/app/immutable/nodes/7.MZzcyrrf.js", HTTP_GET, serveAppImmutableNodes_7MZzcyrrfJs);
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
