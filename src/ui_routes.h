/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,196 bytes
 * ui_html: 4,332 bytes
 * ui_app_immutable_entry_js: 64,416 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,990 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CUVH-Rpt.css", HTTP_GET, serveAppImmutableAssetsInternalCuvhRptCss);
    server->on("/app/immutable/assets/start.CUVH-Rpt.css", HTTP_GET, serveAppImmutableAssetsStartCuvhRptCss);
    server->on("/app/immutable/entry/app.CVS93TRw.js", HTTP_GET, serveAppImmutableEntryAppCvs93TRwJs);
    server->on("/app/immutable/entry/start.WRtsvdMM.js", HTTP_GET, serveAppImmutableEntryStartWRtsvdMmJs);
    server->on("/app/immutable/nodes/0.7Wt1Kenb.js", HTTP_GET, serveAppImmutableNodes_0_7Wt1KenbJs);
    server->on("/app/immutable/nodes/1.CgmaaNgC.js", HTTP_GET, serveAppImmutableNodes_1CgmaaNgCJs);
    server->on("/app/immutable/nodes/2.fWjgVCi5.js", HTTP_GET, serveAppImmutableNodes_2FWjgVCi5Js);
    server->on("/app/immutable/nodes/3.WgCCbsLG.js", HTTP_GET, serveAppImmutableNodes_3WgCCbsLgJs);
    server->on("/app/immutable/nodes/4.COTBxkXp.js", HTTP_GET, serveAppImmutableNodes_4CotBxkXpJs);
    server->on("/app/immutable/nodes/5.Cvao1wIm.js", HTTP_GET, serveAppImmutableNodes_5Cvao1wImJs);
    server->on("/app/immutable/nodes/6.CsD9y1jA.js", HTTP_GET, serveAppImmutableNodes_6CsD9y1jAJs);
    server->on("/app/immutable/nodes/7.Ux_0sfsD.js", HTTP_GET, serveAppImmutableNodes_7Ux_0sfsDJs);
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
