/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 14 520 bytes
 * ui_app_immutable_chunks_js: 66 071 bytes
 * ui_app_immutable_entry_js: 269 bytes
 * ui_app_immutable_nodes_js: 542 bytes
 * ui_html: 4 638 bytes
 * ui_svg: 456 bytes
 * Total: 86 496 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_app_immutable_chunks_js.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_html.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/index.DKQns9on.css", HTTP_GET, serveAppImmutableAssetsIndexDkQns9onCss);
    server->on("/app/immutable/chunks/DpGg5Vcm.js", HTTP_GET, serveAppImmutableChunksDpGg5VcmJs);
    server->on("/app/immutable/entry/app.DF5XWutZ.js", HTTP_GET, serveAppImmutableEntryAppDf5XWutZJs);
    server->on("/app/immutable/entry/start.CavoUrY9.js", HTTP_GET, serveAppImmutableEntryStartCavoUrY9Js);
    server->on("/app/immutable/nodes/0.D23sfxeJ.js", HTTP_GET, serveAppImmutableNodes_0D23sfxeJJs);
    server->on("/app/immutable/nodes/1.D_Nig1YW.js", HTTP_GET, serveAppImmutableNodes_1DNig1YwJs);
    server->on("/app/immutable/nodes/2.BTqRTRhx.js", HTTP_GET, serveAppImmutableNodes_2BTqRtRhxJs);
    server->on("/app/immutable/nodes/3.DuJZRSI7.js", HTTP_GET, serveAppImmutableNodes_3DuJzrsi7Js);
    server->on("/app/immutable/nodes/4.DIqH_ddC.js", HTTP_GET, serveAppImmutableNodes_4DIqHDdCJs);
    server->on("/app/immutable/nodes/5.VhCZ75ev.js", HTTP_GET, serveAppImmutableNodes_5VhCz75evJs);
    server->on("/app/immutable/nodes/6.Wuy1Uo5y.js", HTTP_GET, serveAppImmutableNodes_6Wuy1Uo5yJs);
    server->on("/app/immutable/nodes/7.BzktTkE4.js", HTTP_GET, serveAppImmutableNodes_7BzktTkE4Js);
    server->on("/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes
    server->on("/", HTTP_GET, serveIndexHtml);
    server->on("/devices", HTTP_GET, serveDevicesHtml);
    server->on("/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/hardware", HTTP_GET, serveHardwareHtml);
    server->on("/hardware.html", HTTP_GET, serveHardwareHtml);
    server->on("/network", HTTP_GET, serveNetworkHtml);
    server->on("/network.html", HTTP_GET, serveNetworkHtml);
    server->on("/settings", HTTP_GET, serveSettingsHtml);
    server->on("/settings.html", HTTP_GET, serveSettingsHtml);
}
