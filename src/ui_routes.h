/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,246 bytes
 * ui_html: 4,332 bytes
 * ui_app_immutable_entry_js: 64,684 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,308 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DxBQeS86.css", HTTP_GET, serveAppImmutableAssetsInternalDxBQeS86Css);
    server->on("/app/immutable/assets/start.DxBQeS86.css", HTTP_GET, serveAppImmutableAssetsStartDxBQeS86Css);
    server->on("/app/immutable/entry/app.BO0ZOuEW.js", HTTP_GET, serveAppImmutableEntryAppBo0ZOuEwJs);
    server->on("/app/immutable/entry/start.DAWeRsV5.js", HTTP_GET, serveAppImmutableEntryStartDaWeRsV5Js);
    server->on("/app/immutable/nodes/0.B-_ECcBv.js", HTTP_GET, serveAppImmutableNodes_0BECcBvJs);
    server->on("/app/immutable/nodes/1.CKbxv8_D.js", HTTP_GET, serveAppImmutableNodes_1CKbxv8DJs);
    server->on("/app/immutable/nodes/2.DHOi6s7A.js", HTTP_GET, serveAppImmutableNodes_2DhOi6s7AJs);
    server->on("/app/immutable/nodes/3.CBSM0r0j.js", HTTP_GET, serveAppImmutableNodes_3Cbsm0r0jJs);
    server->on("/app/immutable/nodes/4.By3SYnWk.js", HTTP_GET, serveAppImmutableNodes_4By3SYnWkJs);
    server->on("/app/immutable/nodes/5.BGsX5mBC.js", HTTP_GET, serveAppImmutableNodes_5BGsX5mBcJs);
    server->on("/app/immutable/nodes/6.Bz5jTQzQ.js", HTTP_GET, serveAppImmutableNodes_6Bz5jTQzQJs);
    server->on("/app/immutable/nodes/7.CBs-OhuR.js", HTTP_GET, serveAppImmutableNodes_7CBsOhuRJs);
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
