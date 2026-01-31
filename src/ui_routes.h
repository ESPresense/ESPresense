/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,202 bytes
 * ui_html: 3,894 bytes
 * ui_app_immutable_entry_js: 62,531 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 91,673 bytes
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
    server->on("/app/immutable/entry/app.DnhVzljM.js", HTTP_GET, serveAppImmutableEntryAppDnhVzljMJs);
    server->on("/app/immutable/entry/start.Dl_IA65e.js", HTTP_GET, serveAppImmutableEntryStartDlIa65eJs);
    server->on("/app/immutable/nodes/0.NNyu2y9c.js", HTTP_GET, serveAppImmutableNodes_0NNyu2y9cJs);
    server->on("/app/immutable/nodes/1.CdMkzJht.js", HTTP_GET, serveAppImmutableNodes_1CdMkzJhtJs);
    server->on("/app/immutable/nodes/2.DvdEbNBH.js", HTTP_GET, serveAppImmutableNodes_2DvdEbNbhJs);
    server->on("/app/immutable/nodes/3.Bqvhyk_I.js", HTTP_GET, serveAppImmutableNodes_3BqvhykIJs);
    server->on("/app/immutable/nodes/4.CuOG8U1m.js", HTTP_GET, serveAppImmutableNodes_4CuOg8U1mJs);
    server->on("/app/immutable/nodes/5.CMpb7UBu.js", HTTP_GET, serveAppImmutableNodes_5CMpb7UBuJs);
    server->on("/app/immutable/nodes/6.iFb9owvf.js", HTTP_GET, serveAppImmutableNodes_6IFb9owvfJs);
    server->on("/app/immutable/nodes/7.izziTYMJ.js", HTTP_GET, serveAppImmutableNodes_7IzziTymjJs);
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
