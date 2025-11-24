/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,571 bytes
 * ui_html: 3,240 bytes
 * ui_app_immutable_entry_js: 66,313 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 81,099 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.wrGhOo2C.css", HTTP_GET, serveAppImmutableAssetsInternalWrGhOo2CCss);
    server->on("/app/immutable/assets/start.DOTUP31V.css", HTTP_GET, serveAppImmutableAssetsStartDotup31VCss);
    server->on("/app/immutable/entry/app.D6nWcthr.js", HTTP_GET, serveAppImmutableEntryAppD6nWcthrJs);
    server->on("/app/immutable/entry/start.CLciQiCr.js", HTTP_GET, serveAppImmutableEntryStartCLciQiCrJs);
    server->on("/app/immutable/nodes/0.DPvPkNWB.js", HTTP_GET, serveAppImmutableNodes_0DPvPkNwbJs);
    server->on("/app/immutable/nodes/1.idGTN2lr.js", HTTP_GET, serveAppImmutableNodes_1IdGtn2lrJs);
    server->on("/app/immutable/nodes/2.Ob2U5Cmc.js", HTTP_GET, serveAppImmutableNodes_2Ob2U5CmcJs);
    server->on("/app/immutable/nodes/3.CagR02ql.js", HTTP_GET, serveAppImmutableNodes_3CagR02qlJs);
    server->on("/app/immutable/nodes/4.D4iGkyIR.js", HTTP_GET, serveAppImmutableNodes_4D4iGkyIrJs);
    server->on("/app/immutable/nodes/5.BAiyHu55.js", HTTP_GET, serveAppImmutableNodes_5BAiyHu55Js);
    server->on("/app/immutable/nodes/6.Dq6tHEIx.js", HTTP_GET, serveAppImmutableNodes_6Dq6tHeIxJs);
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
