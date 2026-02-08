/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,554 bytes
 * ui_html: 4,350 bytes
 * ui_app_immutable_entry_js: 62,867 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 92,817 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DHP8CLvW.css", HTTP_GET, serveAppImmutableAssetsInternalDhp8CLvWCss);
    server->on("/app/immutable/assets/start.DHP8CLvW.css", HTTP_GET, serveAppImmutableAssetsStartDhp8CLvWCss);
    server->on("/app/immutable/entry/app.BETESICn.js", HTTP_GET, serveAppImmutableEntryAppBetesiCnJs);
    server->on("/app/immutable/entry/start.R4uTt0V2.js", HTTP_GET, serveAppImmutableEntryStartR4uTt0V2Js);
    server->on("/app/immutable/nodes/0.BHiMg9xE.js", HTTP_GET, serveAppImmutableNodes_0BHiMg9xEJs);
    server->on("/app/immutable/nodes/1.DyZnojKu.js", HTTP_GET, serveAppImmutableNodes_1DyZnojKuJs);
    server->on("/app/immutable/nodes/2.CWyyummq.js", HTTP_GET, serveAppImmutableNodes_2CWyyummqJs);
    server->on("/app/immutable/nodes/3.BwhJXqn4.js", HTTP_GET, serveAppImmutableNodes_3BwhJXqn4Js);
    server->on("/app/immutable/nodes/4.WUOHsStR.js", HTTP_GET, serveAppImmutableNodes_4WuoHsStRJs);
    server->on("/app/immutable/nodes/5.Cmy8BkvZ.js", HTTP_GET, serveAppImmutableNodes_5Cmy8BkvZJs);
    server->on("/app/immutable/nodes/6.Ccm2cVip.js", HTTP_GET, serveAppImmutableNodes_6Ccm2cVipJs);
    server->on("/app/immutable/nodes/7.BBrhRcyD.js", HTTP_GET, serveAppImmutableNodes_7BBrhRcyDJs);
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
