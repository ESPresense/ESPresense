/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,984 bytes
 * ui_html: 4,320 bytes
 * ui_app_immutable_entry_js: 63,839 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,189 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CMfVrT3e.css", HTTP_GET, serveAppImmutableAssetsInternalCMfVrT3eCss);
    server->on("/app/immutable/assets/start.CMfVrT3e.css", HTTP_GET, serveAppImmutableAssetsStartCMfVrT3eCss);
    server->on("/app/immutable/entry/app.-2lbXYZN.js", HTTP_GET, serveAppImmutableEntryApp_2lbXyznJs);
    server->on("/app/immutable/entry/start.CCsxT0CG.js", HTTP_GET, serveAppImmutableEntryStartCCsxT0CgJs);
    server->on("/app/immutable/nodes/0.BC0qgwCf.js", HTTP_GET, serveAppImmutableNodes_0Bc0qgwCfJs);
    server->on("/app/immutable/nodes/1.CzSKWpB-.js", HTTP_GET, serveAppImmutableNodes_1CzSkWpBJs);
    server->on("/app/immutable/nodes/2.D7_vKpyD.js", HTTP_GET, serveAppImmutableNodes_2D7VKpyDJs);
    server->on("/app/immutable/nodes/3.CilCFgT8.js", HTTP_GET, serveAppImmutableNodes_3CilCFgT8Js);
    server->on("/app/immutable/nodes/4.CZlijB0S.js", HTTP_GET, serveAppImmutableNodes_4CZlijB0SJs);
    server->on("/app/immutable/nodes/5.C9P73YyR.js", HTTP_GET, serveAppImmutableNodes_5C9P73YyRJs);
    server->on("/app/immutable/nodes/6.BoEKpy5f.js", HTTP_GET, serveAppImmutableNodes_6BoEKpy5fJs);
    server->on("/app/immutable/nodes/7.ChNLlZ9-.js", HTTP_GET, serveAppImmutableNodes_7ChNLlZ9Js);
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
