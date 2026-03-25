/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,246 bytes
 * ui_html: 4,338 bytes
 * ui_app_immutable_entry_js: 64,604 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,234 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DxBQeS86.css", HTTP_GET, serveAppImmutableAssetsInternalDxBQeS86Css);
    server->on("/app/immutable/assets/start.DxBQeS86.css", HTTP_GET, serveAppImmutableAssetsStartDxBQeS86Css);
    server->on("/app/immutable/entry/app.SRAKFLrg.js", HTTP_GET, serveAppImmutableEntryAppSrakfLrgJs);
    server->on("/app/immutable/entry/start.zwZDxwKO.js", HTTP_GET, serveAppImmutableEntryStartZwZDxwKoJs);
    server->on("/app/immutable/nodes/0.bG1hQUjl.js", HTTP_GET, serveAppImmutableNodes_0BG1hQUjlJs);
    server->on("/app/immutable/nodes/1.C8yX-yXp.js", HTTP_GET, serveAppImmutableNodes_1C8yXYXpJs);
    server->on("/app/immutable/nodes/2.4uK0EPHM.js", HTTP_GET, serveAppImmutableNodes_2_4uK0EphmJs);
    server->on("/app/immutable/nodes/3.BHjEtzG9.js", HTTP_GET, serveAppImmutableNodes_3BHjEtzG9Js);
    server->on("/app/immutable/nodes/4.T-2B0Sb-.js", HTTP_GET, serveAppImmutableNodes_4T_2B0SbJs);
    server->on("/app/immutable/nodes/5.ObA1QWel.js", HTTP_GET, serveAppImmutableNodes_5ObA1QWelJs);
    server->on("/app/immutable/nodes/6.BTSXpU8r.js", HTTP_GET, serveAppImmutableNodes_6BtsXpU8rJs);
    server->on("/app/immutable/nodes/7.CazgxBAO.js", HTTP_GET, serveAppImmutableNodes_7CazgxBaoJs);
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
