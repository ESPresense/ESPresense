/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,984 bytes
 * ui_html: 4,320 bytes
 * ui_app_immutable_entry_js: 63,806 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,156 bytes
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
    server->on("/app/immutable/entry/app.B9hDIDIG.js", HTTP_GET, serveAppImmutableEntryAppB9hDidigJs);
    server->on("/app/immutable/entry/start.C3rcrNch.js", HTTP_GET, serveAppImmutableEntryStartC3rcrNchJs);
    server->on("/app/immutable/nodes/0.BUOBSshb.js", HTTP_GET, serveAppImmutableNodes_0BuobSshbJs);
    server->on("/app/immutable/nodes/1.C_x45UDa.js", HTTP_GET, serveAppImmutableNodes_1CX45UDaJs);
    server->on("/app/immutable/nodes/2.DioGSrdn.js", HTTP_GET, serveAppImmutableNodes_2DioGSrdnJs);
    server->on("/app/immutable/nodes/3.Cz3Jjg36.js", HTTP_GET, serveAppImmutableNodes_3Cz3Jjg36Js);
    server->on("/app/immutable/nodes/4.Ds7-FXTJ.js", HTTP_GET, serveAppImmutableNodes_4Ds7FxtjJs);
    server->on("/app/immutable/nodes/5.g86Qsdj6.js", HTTP_GET, serveAppImmutableNodes_5G86Qsdj6Js);
    server->on("/app/immutable/nodes/6.cboKPwnu.js", HTTP_GET, serveAppImmutableNodes_6CboKPwnuJs);
    server->on("/app/immutable/nodes/7.ANDkkUlw.js", HTTP_GET, serveAppImmutableNodes_7AnDkkUlwJs);
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
