/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,202 bytes
 * ui_html: 3,900 bytes
 * ui_app_immutable_entry_js: 62,600 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 91,748 bytes
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
    server->on("/app/immutable/entry/app.CC9pxwwT.js", HTTP_GET, serveAppImmutableEntryAppCc9pxwwTJs);
    server->on("/app/immutable/entry/start.DMFuKAFt.js", HTTP_GET, serveAppImmutableEntryStartDmFuKaFtJs);
    server->on("/app/immutable/nodes/0.CU0miwJv.js", HTTP_GET, serveAppImmutableNodes_0Cu0miwJvJs);
    server->on("/app/immutable/nodes/1.H1EIP4vg.js", HTTP_GET, serveAppImmutableNodes_1H1Eip4vgJs);
    server->on("/app/immutable/nodes/2.Cb4tZocU.js", HTTP_GET, serveAppImmutableNodes_2Cb4tZocUJs);
    server->on("/app/immutable/nodes/3.WsuF_oj-.js", HTTP_GET, serveAppImmutableNodes_3WsuFOjJs);
    server->on("/app/immutable/nodes/4.VcrH3yMJ.js", HTTP_GET, serveAppImmutableNodes_4VcrH3yMjJs);
    server->on("/app/immutable/nodes/5.gFD4AgD9.js", HTTP_GET, serveAppImmutableNodes_5GFd4AgD9Js);
    server->on("/app/immutable/nodes/6.BDMc5Neh.js", HTTP_GET, serveAppImmutableNodes_6BdMc5NehJs);
    server->on("/app/immutable/nodes/7.CTjNn5QG.js", HTTP_GET, serveAppImmutableNodes_7CTjNn5QgJs);
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
