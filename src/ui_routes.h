/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,144 bytes
 * ui_html: 4,314 bytes
 * ui_app_immutable_entry_js: 64,342 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,846 bytes
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
    server->on("/app/immutable/entry/app.DBpciuRI.js", HTTP_GET, serveAppImmutableEntryAppDBpciuRiJs);
    server->on("/app/immutable/entry/start.ljye1N-E.js", HTTP_GET, serveAppImmutableEntryStartLjye1NEJs);
    server->on("/app/immutable/nodes/0.BW8ufG5o.js", HTTP_GET, serveAppImmutableNodes_0Bw8ufG5oJs);
    server->on("/app/immutable/nodes/1.C1cKxtGp.js", HTTP_GET, serveAppImmutableNodes_1C1cKxtGpJs);
    server->on("/app/immutable/nodes/2.CCNsN-vm.js", HTTP_GET, serveAppImmutableNodes_2CcNsNVmJs);
    server->on("/app/immutable/nodes/3.CMgMSHVZ.js", HTTP_GET, serveAppImmutableNodes_3CMgMshvzJs);
    server->on("/app/immutable/nodes/4.J6MChd6w.js", HTTP_GET, serveAppImmutableNodes_4J6MChd6wJs);
    server->on("/app/immutable/nodes/5.jJendaWR.js", HTTP_GET, serveAppImmutableNodes_5JJendaWrJs);
    server->on("/app/immutable/nodes/6.DqhlCP8E.js", HTTP_GET, serveAppImmutableNodes_6DqhlCp8EJs);
    server->on("/app/immutable/nodes/7.CJM7E9jW.js", HTTP_GET, serveAppImmutableNodes_7Cjm7E9jWJs);
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
