/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,571 bytes
 * ui_html: 3,912 bytes
 * ui_app_immutable_entry_js: 66,580 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 82,109 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.wrGhOo2C.css", HTTP_GET, serveAppImmutableAssetsInternalWrGhOo2CCss);
    server->on("/app/immutable/assets/start.DOTUP31V.css", HTTP_GET, serveAppImmutableAssetsStartDotup31VCss);
    server->on("/app/immutable/entry/app._4342lxZ.js", HTTP_GET, serveAppImmutableEntryApp_4342lxZJs);
    server->on("/app/immutable/entry/start.BxmqOgvn.js", HTTP_GET, serveAppImmutableEntryStartBxmqOgvnJs);
    server->on("/app/immutable/nodes/0.BFN82UGM.js", HTTP_GET, serveAppImmutableNodes_0Bfn82UgmJs);
    server->on("/app/immutable/nodes/1.CaRIXrav.js", HTTP_GET, serveAppImmutableNodes_1CaRiXravJs);
    server->on("/app/immutable/nodes/2.DgQ-mq0E.js", HTTP_GET, serveAppImmutableNodes_2DgQMq0EJs);
    server->on("/app/immutable/nodes/3.CSG04xn0.js", HTTP_GET, serveAppImmutableNodes_3Csg04xn0Js);
    server->on("/app/immutable/nodes/4.Beki81Qe.js", HTTP_GET, serveAppImmutableNodes_4Beki81QeJs);
    server->on("/app/immutable/nodes/5.BakcMFFx.js", HTTP_GET, serveAppImmutableNodes_5BakcMfFxJs);
    server->on("/app/immutable/nodes/6.fuFX2G1p.js", HTTP_GET, serveAppImmutableNodes_6FuFx2G1pJs);
    server->on("/app/immutable/nodes/7.BBmHkUOL.js", HTTP_GET, serveAppImmutableNodes_7BBmHkUolJs);
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
