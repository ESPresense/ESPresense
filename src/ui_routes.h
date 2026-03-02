/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,984 bytes
 * ui_html: 4,320 bytes
 * ui_app_immutable_entry_js: 64,110 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,460 bytes
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
    server->on("/app/immutable/entry/app.q_kxyDoa.js", HTTP_GET, serveAppImmutableEntryAppQKxyDoaJs);
    server->on("/app/immutable/entry/start.BXcEkfuq.js", HTTP_GET, serveAppImmutableEntryStartBXcEkfuqJs);
    server->on("/app/immutable/nodes/0.BnFXgm_G.js", HTTP_GET, serveAppImmutableNodes_0BnFXgmGJs);
    server->on("/app/immutable/nodes/1.DvaoaEGp.js", HTTP_GET, serveAppImmutableNodes_1DvaoaEGpJs);
    server->on("/app/immutable/nodes/2.DIcRN-Mf.js", HTTP_GET, serveAppImmutableNodes_2DIcRnMfJs);
    server->on("/app/immutable/nodes/3.nzbY4VwT.js", HTTP_GET, serveAppImmutableNodes_3NzbY4VwTJs);
    server->on("/app/immutable/nodes/4.7GzvUPvv.js", HTTP_GET, serveAppImmutableNodes_4_7GzvUPvvJs);
    server->on("/app/immutable/nodes/5.0goXOojf.js", HTTP_GET, serveAppImmutableNodes_5_0goXOojfJs);
    server->on("/app/immutable/nodes/6.DfcNNWqH.js", HTTP_GET, serveAppImmutableNodes_6DfcNnWqHJs);
    server->on("/app/immutable/nodes/7.B0sdtMFu.js", HTTP_GET, serveAppImmutableNodes_7B0sdtMFuJs);
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
