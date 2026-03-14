/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,246 bytes
 * ui_html: 4,338 bytes
 * ui_app_immutable_entry_js: 64,551 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,181 bytes
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
    server->on("/app/immutable/entry/app.DkLmTR71.js", HTTP_GET, serveAppImmutableEntryAppDkLmTr71Js);
    server->on("/app/immutable/entry/start.sXUDYGug.js", HTTP_GET, serveAppImmutableEntryStartSXudyGugJs);
    server->on("/app/immutable/nodes/0.CZ2uVc-a.js", HTTP_GET, serveAppImmutableNodes_0Cz2uVcAJs);
    server->on("/app/immutable/nodes/1.BJuE4NFU.js", HTTP_GET, serveAppImmutableNodes_1BJuE4NfuJs);
    server->on("/app/immutable/nodes/2.Bira9Cs5.js", HTTP_GET, serveAppImmutableNodes_2Bira9Cs5Js);
    server->on("/app/immutable/nodes/3.BMf-STvP.js", HTTP_GET, serveAppImmutableNodes_3BMfSTvPJs);
    server->on("/app/immutable/nodes/4.CswRlM1x.js", HTTP_GET, serveAppImmutableNodes_4CswRlM1xJs);
    server->on("/app/immutable/nodes/5.nnlxzSL3.js", HTTP_GET, serveAppImmutableNodes_5NnlxzSl3Js);
    server->on("/app/immutable/nodes/6.BJ1ZE702.js", HTTP_GET, serveAppImmutableNodes_6Bj1Ze702Js);
    server->on("/app/immutable/nodes/7.BcuSfHb7.js", HTTP_GET, serveAppImmutableNodes_7BcuSfHb7Js);
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
