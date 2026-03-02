/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,984 bytes
 * ui_html: 4,314 bytes
 * ui_app_immutable_entry_js: 64,042 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,386 bytes
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
    server->on("/app/immutable/entry/app.CtR5Wwcn.js", HTTP_GET, serveAppImmutableEntryAppCtR5WwcnJs);
    server->on("/app/immutable/entry/start.lrusjRnk.js", HTTP_GET, serveAppImmutableEntryStartLrusjRnkJs);
    server->on("/app/immutable/nodes/0.BmYcAQGG.js", HTTP_GET, serveAppImmutableNodes_0BmYcAqggJs);
    server->on("/app/immutable/nodes/1.DG85_bWR.js", HTTP_GET, serveAppImmutableNodes_1Dg85BWrJs);
    server->on("/app/immutable/nodes/2.D5xAWUYr.js", HTTP_GET, serveAppImmutableNodes_2D5xAwuYrJs);
    server->on("/app/immutable/nodes/3.C-DjE_Ep.js", HTTP_GET, serveAppImmutableNodes_3CDjEEpJs);
    server->on("/app/immutable/nodes/4.d5aDY-nX.js", HTTP_GET, serveAppImmutableNodes_4D5aDyNXJs);
    server->on("/app/immutable/nodes/5.DcT0R7FJ.js", HTTP_GET, serveAppImmutableNodes_5DcT0R7FjJs);
    server->on("/app/immutable/nodes/6.lsbtBHwF.js", HTTP_GET, serveAppImmutableNodes_6LsbtBHwFJs);
    server->on("/app/immutable/nodes/7.uNZfAM4t.js", HTTP_GET, serveAppImmutableNodes_7UNZfAm4tJs);
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
