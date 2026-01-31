/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,202 bytes
 * ui_html: 3,900 bytes
 * ui_app_immutable_entry_js: 62,387 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 91,535 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.BRhgnC6i.css", HTTP_GET, serveAppImmutableAssetsInternalBRhgnC6iCss);
    server->on("/app/immutable/assets/start.BRhgnC6i.css", HTTP_GET, serveAppImmutableAssetsStartBRhgnC6iCss);
    server->on("/app/immutable/entry/app.DvdlG2i0.js", HTTP_GET, serveAppImmutableEntryAppDvdlG2i0Js);
    server->on("/app/immutable/entry/start.Cs_8MIEd.js", HTTP_GET, serveAppImmutableEntryStartCs_8MiEdJs);
    server->on("/app/immutable/nodes/0.D01odWY5.js", HTTP_GET, serveAppImmutableNodes_0D01odWy5Js);
    server->on("/app/immutable/nodes/1.yMd4ghDe.js", HTTP_GET, serveAppImmutableNodes_1YMd4ghDeJs);
    server->on("/app/immutable/nodes/2.V-F1wnVM.js", HTTP_GET, serveAppImmutableNodes_2VF1wnVmJs);
    server->on("/app/immutable/nodes/3.DP29vhtA.js", HTTP_GET, serveAppImmutableNodes_3Dp29vhtAJs);
    server->on("/app/immutable/nodes/4.DTYLY2Gc.js", HTTP_GET, serveAppImmutableNodes_4Dtyly2GcJs);
    server->on("/app/immutable/nodes/5.DJQkfNAH.js", HTTP_GET, serveAppImmutableNodes_5DjQkfNahJs);
    server->on("/app/immutable/nodes/6.D0ugTSBU.js", HTTP_GET, serveAppImmutableNodes_6D0ugTsbuJs);
    server->on("/app/immutable/nodes/7.Kb4bk4Jt.js", HTTP_GET, serveAppImmutableNodes_7Kb4bk4JtJs);
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
