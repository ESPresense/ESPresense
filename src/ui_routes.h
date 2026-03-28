/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,246 bytes
 * ui_html: 4,332 bytes
 * ui_app_immutable_entry_js: 64,499 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,123 bytes
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
    server->on("/app/immutable/entry/app.BPpV5hO_.js", HTTP_GET, serveAppImmutableEntryAppBPpV5hOJs);
    server->on("/app/immutable/entry/start.BNHS7yNc.js", HTTP_GET, serveAppImmutableEntryStartBnhs7yNcJs);
    server->on("/app/immutable/nodes/0.BPzmUT7p.js", HTTP_GET, serveAppImmutableNodes_0BPzmUt7pJs);
    server->on("/app/immutable/nodes/1.B4NIaeKW.js", HTTP_GET, serveAppImmutableNodes_1B4NIaeKwJs);
    server->on("/app/immutable/nodes/2.AV8M1AjZ.js", HTTP_GET, serveAppImmutableNodes_2Av8M1AjZJs);
    server->on("/app/immutable/nodes/3.BunB7a83.js", HTTP_GET, serveAppImmutableNodes_3BunB7a83Js);
    server->on("/app/immutable/nodes/4.wUG6T0nc.js", HTTP_GET, serveAppImmutableNodes_4WUg6T0ncJs);
    server->on("/app/immutable/nodes/5.CRS1rZF_.js", HTTP_GET, serveAppImmutableNodes_5Crs1rZfJs);
    server->on("/app/immutable/nodes/6.CGqX7RPJ.js", HTTP_GET, serveAppImmutableNodes_6CGqX7RpjJs);
    server->on("/app/immutable/nodes/7.qLFN36YG.js", HTTP_GET, serveAppImmutableNodes_7QLfn36YgJs);
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
