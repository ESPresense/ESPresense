/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,144 bytes
 * ui_html: 4,529 bytes
 * ui_app_immutable_entry_js: 66,794 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 97,513 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.BieM-44V.css", HTTP_GET, serveAppImmutableAssetsInternalBieM_44VCss);
    server->on("/app/immutable/assets/start.BieM-44V.css", HTTP_GET, serveAppImmutableAssetsStartBieM_44VCss);
    server->on("/app/immutable/entry/app.DC-QzOnU.js", HTTP_GET, serveAppImmutableEntryAppDcQzOnUJs);
    server->on("/app/immutable/entry/start.DGvnT5xL.js", HTTP_GET, serveAppImmutableEntryStartDGvnT5xLJs);
    server->on("/app/immutable/nodes/0.C7lSotV4.js", HTTP_GET, serveAppImmutableNodes_0C7lSotV4Js);
    server->on("/app/immutable/nodes/1.Doidn_du.js", HTTP_GET, serveAppImmutableNodes_1DoidnDuJs);
    server->on("/app/immutable/nodes/2.CC65b6fN.js", HTTP_GET, serveAppImmutableNodes_2Cc65b6fNJs);
    server->on("/app/immutable/nodes/3.DGMjSVzq.js", HTTP_GET, serveAppImmutableNodes_3DgMjSVzqJs);
    server->on("/app/immutable/nodes/4.Uca_HlvG.js", HTTP_GET, serveAppImmutableNodes_4UcaHlvGJs);
    server->on("/app/immutable/nodes/5.BkHnnZ5d.js", HTTP_GET, serveAppImmutableNodes_5BkHnnZ5dJs);
    server->on("/app/immutable/nodes/6.Cp9RLUVi.js", HTTP_GET, serveAppImmutableNodes_6Cp9RluViJs);
    server->on("/app/immutable/nodes/7.iwhc0SZ9.js", HTTP_GET, serveAppImmutableNodes_7Iwhc0Sz9Js);
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
