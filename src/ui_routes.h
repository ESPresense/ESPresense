/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,202 bytes
 * ui_html: 3,900 bytes
 * ui_app_immutable_entry_js: 62,552 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 91,700 bytes
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
    server->on("/app/immutable/entry/app.DVnTm6b2.js", HTTP_GET, serveAppImmutableEntryAppDVnTm6b2Js);
    server->on("/app/immutable/entry/start.D2qNiMpH.js", HTTP_GET, serveAppImmutableEntryStartD2qNiMpHJs);
    server->on("/app/immutable/nodes/0.Dyw0mts0.js", HTTP_GET, serveAppImmutableNodes_0Dyw0mts0Js);
    server->on("/app/immutable/nodes/1.CB4cgYLI.js", HTTP_GET, serveAppImmutableNodes_1Cb4cgYliJs);
    server->on("/app/immutable/nodes/2.2BNimZgn.js", HTTP_GET, serveAppImmutableNodes_2_2BNimZgnJs);
    server->on("/app/immutable/nodes/3.CCRvCUZ1.js", HTTP_GET, serveAppImmutableNodes_3CcRvCuz1Js);
    server->on("/app/immutable/nodes/4.D-Bw4tov.js", HTTP_GET, serveAppImmutableNodes_4DBw4tovJs);
    server->on("/app/immutable/nodes/5.CencCPtE.js", HTTP_GET, serveAppImmutableNodes_5CencCPtEJs);
    server->on("/app/immutable/nodes/6.DOgg4QSv.js", HTTP_GET, serveAppImmutableNodes_6DOgg4QSvJs);
    server->on("/app/immutable/nodes/7.BACVLnXS.js", HTTP_GET, serveAppImmutableNodes_7BacvLnXsJs);
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
