/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,202 bytes
 * ui_html: 3,882 bytes
 * ui_app_immutable_entry_js: 62,437 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 91,567 bytes
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
    server->on("/app/immutable/entry/app.DlGLujiy.js", HTTP_GET, serveAppImmutableEntryAppDlGLujiyJs);
    server->on("/app/immutable/entry/start.BLjitrgr.js", HTTP_GET, serveAppImmutableEntryStartBLjitrgrJs);
    server->on("/app/immutable/nodes/0.ChnuMcaS.js", HTTP_GET, serveAppImmutableNodes_0ChnuMcaSJs);
    server->on("/app/immutable/nodes/1.D77uR3Ei.js", HTTP_GET, serveAppImmutableNodes_1D77uR3EiJs);
    server->on("/app/immutable/nodes/2.DF4G9apd.js", HTTP_GET, serveAppImmutableNodes_2Df4G9apdJs);
    server->on("/app/immutable/nodes/3.DA0FIiZA.js", HTTP_GET, serveAppImmutableNodes_3Da0FIiZaJs);
    server->on("/app/immutable/nodes/4.DRdvaKaa.js", HTTP_GET, serveAppImmutableNodes_4DRdvaKaaJs);
    server->on("/app/immutable/nodes/5.BN7TEVcJ.js", HTTP_GET, serveAppImmutableNodes_5Bn7TeVcJJs);
    server->on("/app/immutable/nodes/6.hDE5IsVi.js", HTTP_GET, serveAppImmutableNodes_6HDe5IsViJs);
    server->on("/app/immutable/nodes/7.Cj0jTOiC.js", HTTP_GET, serveAppImmutableNodes_7Cj0jTOiCJs);
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
