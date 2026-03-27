/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,320 bytes
 * ui_html: 4,320 bytes
 * ui_app_immutable_entry_js: 64,700 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,386 bytes
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
    server->on("/app/immutable/entry/app.rLeIPDKQ.js", HTTP_GET, serveAppImmutableEntryAppRLeIpdkqJs);
    server->on("/app/immutable/entry/start.eL07FDn2.js", HTTP_GET, serveAppImmutableEntryStartEL07FDn2Js);
    server->on("/app/immutable/nodes/0.CJylTAXo.js", HTTP_GET, serveAppImmutableNodes_0CJylTaXoJs);
    server->on("/app/immutable/nodes/1.BxhhUfCV.js", HTTP_GET, serveAppImmutableNodes_1BxhhUfCvJs);
    server->on("/app/immutable/nodes/2.C5PYlonH.js", HTTP_GET, serveAppImmutableNodes_2C5PYlonHJs);
    server->on("/app/immutable/nodes/3.B7ijdpa9.js", HTTP_GET, serveAppImmutableNodes_3B7ijdpa9Js);
    server->on("/app/immutable/nodes/4.CD6V1uab.js", HTTP_GET, serveAppImmutableNodes_4Cd6V1uabJs);
    server->on("/app/immutable/nodes/5.BBbYC7lg.js", HTTP_GET, serveAppImmutableNodes_5BBbYc7lgJs);
    server->on("/app/immutable/nodes/6.BxqzvKZ7.js", HTTP_GET, serveAppImmutableNodes_6BxqzvKz7Js);
    server->on("/app/immutable/nodes/7.DyLbCfLF.js", HTTP_GET, serveAppImmutableNodes_7DyLbCfLfJs);
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
