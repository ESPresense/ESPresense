/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,984 bytes
 * ui_html: 4,326 bytes
 * ui_app_immutable_entry_js: 63,837 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,193 bytes
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
    server->on("/app/immutable/entry/app.BfAMXyqL.js", HTTP_GET, serveAppImmutableEntryAppBfAmXyqLJs);
    server->on("/app/immutable/entry/start.Biokm-IG.js", HTTP_GET, serveAppImmutableEntryStartBiokmIgJs);
    server->on("/app/immutable/nodes/0.XV2LXSmT.js", HTTP_GET, serveAppImmutableNodes_0Xv2LxSmTJs);
    server->on("/app/immutable/nodes/1.CNBX-W8s.js", HTTP_GET, serveAppImmutableNodes_1CnbxW8sJs);
    server->on("/app/immutable/nodes/2.BUL7UOsH.js", HTTP_GET, serveAppImmutableNodes_2Bul7UOsHJs);
    server->on("/app/immutable/nodes/3.CpWOAr1L.js", HTTP_GET, serveAppImmutableNodes_3CpWoAr1LJs);
    server->on("/app/immutable/nodes/4.Bsx9NPfo.js", HTTP_GET, serveAppImmutableNodes_4Bsx9NPfoJs);
    server->on("/app/immutable/nodes/5.DAZeIDrH.js", HTTP_GET, serveAppImmutableNodes_5DaZeIDrHJs);
    server->on("/app/immutable/nodes/6.DlLwoQyP.js", HTTP_GET, serveAppImmutableNodes_6DlLwoQyPJs);
    server->on("/app/immutable/nodes/7.lj_k3TSX.js", HTTP_GET, serveAppImmutableNodes_7LjK3TsxJs);
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
