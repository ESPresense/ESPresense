/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,361 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 48,519 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 63,095 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.B89G9FIo.css", HTTP_GET, serveAppImmutableAssetsInternalB89G9FIoCss);
    server->on("/app/immutable/assets/start.D-jtWh4c.css", HTTP_GET, serveAppImmutableAssetsStartDJtWh4cCss);
    server->on("/app/immutable/entry/app.DfzdUd_l.js", HTTP_GET, serveAppImmutableEntryAppDfzdUdLJs);
    server->on("/app/immutable/entry/start.DP-MRUXV.js", HTTP_GET, serveAppImmutableEntryStartDpMruxvJs);
    server->on("/app/immutable/nodes/0.BgT348M3.js", HTTP_GET, serveAppImmutableNodes_0BgT348M3Js);
    server->on("/app/immutable/nodes/1.B7q3n-6y.js", HTTP_GET, serveAppImmutableNodes_1B7q3n_6yJs);
    server->on("/app/immutable/nodes/2.Db5mCJLs.js", HTTP_GET, serveAppImmutableNodes_2Db5mCjLsJs);
    server->on("/app/immutable/nodes/3.DPcCRilD.js", HTTP_GET, serveAppImmutableNodes_3DPcCRilDJs);
    server->on("/app/immutable/nodes/4.CSozkVrc.js", HTTP_GET, serveAppImmutableNodes_4CSozkVrcJs);
    server->on("/app/immutable/nodes/5.BaGRmr-v.js", HTTP_GET, serveAppImmutableNodes_5BaGRmrVJs);
    server->on("/app/immutable/nodes/6.FKr_Bq37.js", HTTP_GET, serveAppImmutableNodes_6FKrBq37Js);
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
