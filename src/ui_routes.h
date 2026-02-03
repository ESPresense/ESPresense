/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,212 bytes
 * ui_html: 3,894 bytes
 * ui_app_immutable_entry_js: 62,602 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 91,754 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.D-OnhSQM.css", HTTP_GET, serveAppImmutableAssetsInternalDOnhSqmCss);
    server->on("/app/immutable/assets/start.D-OnhSQM.css", HTTP_GET, serveAppImmutableAssetsStartDOnhSqmCss);
    server->on("/app/immutable/entry/app.BQISssZl.js", HTTP_GET, serveAppImmutableEntryAppBqiSssZlJs);
    server->on("/app/immutable/entry/start.CsxN__ah.js", HTTP_GET, serveAppImmutableEntryStartCsxNAhJs);
    server->on("/app/immutable/nodes/0.0IU7qp4u.js", HTTP_GET, serveAppImmutableNodes_0_0Iu7qp4uJs);
    server->on("/app/immutable/nodes/1.B-L03fEW.js", HTTP_GET, serveAppImmutableNodes_1BL03fEwJs);
    server->on("/app/immutable/nodes/2.DJXs3gXx.js", HTTP_GET, serveAppImmutableNodes_2DjXs3gXxJs);
    server->on("/app/immutable/nodes/3.DXhw5pGe.js", HTTP_GET, serveAppImmutableNodes_3DXhw5pGeJs);
    server->on("/app/immutable/nodes/4.Ccsb7Ag6.js", HTTP_GET, serveAppImmutableNodes_4Ccsb7Ag6Js);
    server->on("/app/immutable/nodes/5.C4jwl19d.js", HTTP_GET, serveAppImmutableNodes_5C4jwl19dJs);
    server->on("/app/immutable/nodes/6.DgmQWRVu.js", HTTP_GET, serveAppImmutableNodes_6DgmQwrVuJs);
    server->on("/app/immutable/nodes/7.CLKPmKLJ.js", HTTP_GET, serveAppImmutableNodes_7ClkPmKljJs);
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
