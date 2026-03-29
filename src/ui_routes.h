/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,320 bytes
 * ui_html: 4,320 bytes
 * ui_app_immutable_entry_js: 64,734 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,420 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CiOlzjkk.css", HTTP_GET, serveAppImmutableAssetsInternalCiOlzjkkCss);
    server->on("/app/immutable/assets/start.CiOlzjkk.css", HTTP_GET, serveAppImmutableAssetsStartCiOlzjkkCss);
    server->on("/app/immutable/entry/app.AGyTNwoD.js", HTTP_GET, serveAppImmutableEntryAppAGyTNwoDJs);
    server->on("/app/immutable/entry/start.CYRTSkE9.js", HTTP_GET, serveAppImmutableEntryStartCyrtSkE9Js);
    server->on("/app/immutable/nodes/0.YOMbRlIY.js", HTTP_GET, serveAppImmutableNodes_0YoMbRlIyJs);
    server->on("/app/immutable/nodes/1.LOFvjm7K.js", HTTP_GET, serveAppImmutableNodes_1LoFvjm7KJs);
    server->on("/app/immutable/nodes/2.DGvozh--.js", HTTP_GET, serveAppImmutableNodes_2DGvozhJs);
    server->on("/app/immutable/nodes/3.CxccgXcB.js", HTTP_GET, serveAppImmutableNodes_3CxccgXcBJs);
    server->on("/app/immutable/nodes/4.lGUEBQh5.js", HTTP_GET, serveAppImmutableNodes_4LGuebQh5Js);
    server->on("/app/immutable/nodes/5.D8R8DeuR.js", HTTP_GET, serveAppImmutableNodes_5D8R8DeuRJs);
    server->on("/app/immutable/nodes/6.CglQrzca.js", HTTP_GET, serveAppImmutableNodes_6CglQrzcaJs);
    server->on("/app/immutable/nodes/7.Dm6XoU74.js", HTTP_GET, serveAppImmutableNodes_7Dm6XoU74Js);
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
