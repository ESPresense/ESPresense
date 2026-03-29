/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,246 bytes
 * ui_html: 4,338 bytes
 * ui_app_immutable_entry_js: 64,327 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,957 bytes
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
    server->on("/app/immutable/entry/app.CXNuhsxa.js", HTTP_GET, serveAppImmutableEntryAppCxNuhsxaJs);
    server->on("/app/immutable/entry/start.DXdK9ANz.js", HTTP_GET, serveAppImmutableEntryStartDXdK9ANzJs);
    server->on("/app/immutable/nodes/0.Dc6NJQW7.js", HTTP_GET, serveAppImmutableNodes_0Dc6Njqw7Js);
    server->on("/app/immutable/nodes/1.BrkUwOQf.js", HTTP_GET, serveAppImmutableNodes_1BrkUwOQfJs);
    server->on("/app/immutable/nodes/2.lArXYYMd.js", HTTP_GET, serveAppImmutableNodes_2LArXyyMdJs);
    server->on("/app/immutable/nodes/3.BPK-dqrr.js", HTTP_GET, serveAppImmutableNodes_3BpkDqrrJs);
    server->on("/app/immutable/nodes/4.BR7c8WyO.js", HTTP_GET, serveAppImmutableNodes_4Br7c8WyOJs);
    server->on("/app/immutable/nodes/5.CWj7sVEf.js", HTTP_GET, serveAppImmutableNodes_5CWj7sVEfJs);
    server->on("/app/immutable/nodes/6.DZp3Nd0n.js", HTTP_GET, serveAppImmutableNodes_6DZp3Nd0nJs);
    server->on("/app/immutable/nodes/7.CtDjRDrT.js", HTTP_GET, serveAppImmutableNodes_7CtDjRDrTJs);
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
