/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,326 bytes
 * ui_app_immutable_entry_js: 63,875 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,123 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsInternalDAt95k88Css);
    server->on("/app/immutable/assets/start.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsStartDAt95k88Css);
    server->on("/app/immutable/entry/app.cHCjwK_5.js", HTTP_GET, serveAppImmutableEntryAppCHCjwK_5Js);
    server->on("/app/immutable/entry/start.CsMMwjq5.js", HTTP_GET, serveAppImmutableEntryStartCsMMwjq5Js);
    server->on("/app/immutable/nodes/0.BQWCvNtg.js", HTTP_GET, serveAppImmutableNodes_0BqwCvNtgJs);
    server->on("/app/immutable/nodes/1.4EMLORCc.js", HTTP_GET, serveAppImmutableNodes_1_4EmlorCcJs);
    server->on("/app/immutable/nodes/2.BjsxZQHQ.js", HTTP_GET, serveAppImmutableNodes_2BjsxZqhqJs);
    server->on("/app/immutable/nodes/3.DqsnXz3F.js", HTTP_GET, serveAppImmutableNodes_3DqsnXz3FJs);
    server->on("/app/immutable/nodes/4.COzJHW1g.js", HTTP_GET, serveAppImmutableNodes_4COzJhw1gJs);
    server->on("/app/immutable/nodes/5.CPZA26LI.js", HTTP_GET, serveAppImmutableNodes_5Cpza26LiJs);
    server->on("/app/immutable/nodes/6.Q_w5VyO5.js", HTTP_GET, serveAppImmutableNodes_6QW5VyO5Js);
    server->on("/app/immutable/nodes/7.DS5Nfilx.js", HTTP_GET, serveAppImmutableNodes_7Ds5NfilxJs);
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
