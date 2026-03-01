/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,984 bytes
 * ui_html: 4,314 bytes
 * ui_app_immutable_entry_js: 63,955 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,299 bytes
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
    server->on("/app/immutable/entry/app.H7xZMDuu.js", HTTP_GET, serveAppImmutableEntryAppH7xZmDuuJs);
    server->on("/app/immutable/entry/start.alqElARc.js", HTTP_GET, serveAppImmutableEntryStartAlqElARcJs);
    server->on("/app/immutable/nodes/0.D7shetyL.js", HTTP_GET, serveAppImmutableNodes_0D7shetyLJs);
    server->on("/app/immutable/nodes/1.B7ZNCjkr.js", HTTP_GET, serveAppImmutableNodes_1B7ZnCjkrJs);
    server->on("/app/immutable/nodes/2.TbCNiUqF.js", HTTP_GET, serveAppImmutableNodes_2TbCNiUqFJs);
    server->on("/app/immutable/nodes/3.BEK7nLBe.js", HTTP_GET, serveAppImmutableNodes_3Bek7nLBeJs);
    server->on("/app/immutable/nodes/4.DehJGVhG.js", HTTP_GET, serveAppImmutableNodes_4DehJgVhGJs);
    server->on("/app/immutable/nodes/5.DFcmTYYt.js", HTTP_GET, serveAppImmutableNodes_5DFcmTyYtJs);
    server->on("/app/immutable/nodes/6.Cutz06zA.js", HTTP_GET, serveAppImmutableNodes_6Cutz06zAJs);
    server->on("/app/immutable/nodes/7.DMAibkHI.js", HTTP_GET, serveAppImmutableNodes_7DmAibkHiJs);
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
