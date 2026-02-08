/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,464 bytes
 * ui_html: 4,338 bytes
 * ui_app_immutable_entry_js: 62,928 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 92,776 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.BinSLUrC.css", HTTP_GET, serveAppImmutableAssetsInternalBinSlUrCCss);
    server->on("/app/immutable/assets/start.BinSLUrC.css", HTTP_GET, serveAppImmutableAssetsStartBinSlUrCCss);
    server->on("/app/immutable/entry/app.DWxTyNVB.js", HTTP_GET, serveAppImmutableEntryAppDWxTyNvbJs);
    server->on("/app/immutable/entry/start.3AarPXiq.js", HTTP_GET, serveAppImmutableEntryStart_3AarPXiqJs);
    server->on("/app/immutable/nodes/0.Bs57wzUx.js", HTTP_GET, serveAppImmutableNodes_0Bs57wzUxJs);
    server->on("/app/immutable/nodes/1.qqkn1aLA.js", HTTP_GET, serveAppImmutableNodes_1Qqkn1aLaJs);
    server->on("/app/immutable/nodes/2.DX66I0tn.js", HTTP_GET, serveAppImmutableNodes_2Dx66I0tnJs);
    server->on("/app/immutable/nodes/3.Czh2qmYC.js", HTTP_GET, serveAppImmutableNodes_3Czh2qmYcJs);
    server->on("/app/immutable/nodes/4.t214lLxk.js", HTTP_GET, serveAppImmutableNodes_4T214lLxkJs);
    server->on("/app/immutable/nodes/5.BXbm33LD.js", HTTP_GET, serveAppImmutableNodes_5BXbm33LdJs);
    server->on("/app/immutable/nodes/6.DV44yqfG.js", HTTP_GET, serveAppImmutableNodes_6Dv44yqfGJs);
    server->on("/app/immutable/nodes/7.DhZGdvyl.js", HTTP_GET, serveAppImmutableNodes_7DhZGdvylJs);
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
