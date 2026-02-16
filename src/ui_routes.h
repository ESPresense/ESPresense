/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,332 bytes
 * ui_app_immutable_entry_js: 63,236 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 93,490 bytes
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
    server->on("/app/immutable/entry/app.Da7HtmDF.js", HTTP_GET, serveAppImmutableEntryAppDa7HtmDfJs);
    server->on("/app/immutable/entry/start.CyO60RQr.js", HTTP_GET, serveAppImmutableEntryStartCyO60RQrJs);
    server->on("/app/immutable/nodes/0.Bo_bXaMb.js", HTTP_GET, serveAppImmutableNodes_0BoBXaMbJs);
    server->on("/app/immutable/nodes/1.CpHtOn9Z.js", HTTP_GET, serveAppImmutableNodes_1CpHtOn9ZJs);
    server->on("/app/immutable/nodes/2.BcjJil0T.js", HTTP_GET, serveAppImmutableNodes_2BcjJil0TJs);
    server->on("/app/immutable/nodes/3.z0PGKxxW.js", HTTP_GET, serveAppImmutableNodes_3Z0PgKxxWJs);
    server->on("/app/immutable/nodes/4.CmQkTG4f.js", HTTP_GET, serveAppImmutableNodes_4CmQkTg4fJs);
    server->on("/app/immutable/nodes/5.Djq46XJF.js", HTTP_GET, serveAppImmutableNodes_5Djq46XjfJs);
    server->on("/app/immutable/nodes/6.BqsuDo1e.js", HTTP_GET, serveAppImmutableNodes_6BqsuDo1eJs);
    server->on("/app/immutable/nodes/7.B9yklRYg.js", HTTP_GET, serveAppImmutableNodes_7B9yklRYgJs);
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
