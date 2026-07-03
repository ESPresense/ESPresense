/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,144 bytes
 * ui_html: 4,320 bytes
 * ui_app_immutable_entry_js: 64,706 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,216 bytes
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
    server->on("/app/immutable/entry/app.C_OyAO4Y.js", HTTP_GET, serveAppImmutableEntryAppCOyAo4YJs);
    server->on("/app/immutable/entry/start.zQoP3ojs.js", HTTP_GET, serveAppImmutableEntryStartZQoP3ojsJs);
    server->on("/app/immutable/nodes/0.BiULqjKy.js", HTTP_GET, serveAppImmutableNodes_0BiULqjKyJs);
    server->on("/app/immutable/nodes/1.C0JmufU5.js", HTTP_GET, serveAppImmutableNodes_1C0JmufU5Js);
    server->on("/app/immutable/nodes/2.B4kYxQzi.js", HTTP_GET, serveAppImmutableNodes_2B4kYxQziJs);
    server->on("/app/immutable/nodes/3.DitgBr7K.js", HTTP_GET, serveAppImmutableNodes_3DitgBr7KJs);
    server->on("/app/immutable/nodes/4.CgrEwFFP.js", HTTP_GET, serveAppImmutableNodes_4CgrEwFfpJs);
    server->on("/app/immutable/nodes/5.6cAUaHa3.js", HTTP_GET, serveAppImmutableNodes_5_6cAUaHa3Js);
    server->on("/app/immutable/nodes/6.CChFhmz0.js", HTTP_GET, serveAppImmutableNodes_6CChFhmz0Js);
    server->on("/app/immutable/nodes/7.Bq-80Ywp.js", HTTP_GET, serveAppImmutableNodes_7Bq_80YwpJs);
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
