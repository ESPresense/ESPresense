/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,144 bytes
 * ui_html: 4,314 bytes
 * ui_app_immutable_entry_js: 64,880 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,384 bytes
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
    server->on("/app/immutable/entry/app.DwhlPcZD.js", HTTP_GET, serveAppImmutableEntryAppDwhlPcZdJs);
    server->on("/app/immutable/entry/start.C-saEuv-.js", HTTP_GET, serveAppImmutableEntryStartCSaEuvJs);
    server->on("/app/immutable/nodes/0.DaC4LDnF.js", HTTP_GET, serveAppImmutableNodes_0DaC4LDnFJs);
    server->on("/app/immutable/nodes/1.yCWpYI8i.js", HTTP_GET, serveAppImmutableNodes_1YCWpYi8iJs);
    server->on("/app/immutable/nodes/2.ml_1-Qbo.js", HTTP_GET, serveAppImmutableNodes_2Ml_1QboJs);
    server->on("/app/immutable/nodes/3.BgZLZ17H.js", HTTP_GET, serveAppImmutableNodes_3BgZlz17HJs);
    server->on("/app/immutable/nodes/4.C7L61xWw.js", HTTP_GET, serveAppImmutableNodes_4C7L61xWwJs);
    server->on("/app/immutable/nodes/5.W9lW7sh8.js", HTTP_GET, serveAppImmutableNodes_5W9lW7sh8Js);
    server->on("/app/immutable/nodes/6.DYydU-fz.js", HTTP_GET, serveAppImmutableNodes_6DYydUFzJs);
    server->on("/app/immutable/nodes/7.C_z1CDRk.js", HTTP_GET, serveAppImmutableNodes_7CZ1CdRkJs);
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
