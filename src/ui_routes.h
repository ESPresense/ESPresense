/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,246 bytes
 * ui_html: 4,338 bytes
 * ui_app_immutable_entry_js: 64,590 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,220 bytes
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
    server->on("/app/immutable/entry/app.B-mp6H4L.js", HTTP_GET, serveAppImmutableEntryAppBMp6H4LJs);
    server->on("/app/immutable/entry/start.HBxAYF3Z.js", HTTP_GET, serveAppImmutableEntryStartHBxAyf3ZJs);
    server->on("/app/immutable/nodes/0.DQhD2XaG.js", HTTP_GET, serveAppImmutableNodes_0DQhD2XaGJs);
    server->on("/app/immutable/nodes/1.C_ljPB9I.js", HTTP_GET, serveAppImmutableNodes_1CLjPb9IJs);
    server->on("/app/immutable/nodes/2.d1KAmo_x.js", HTTP_GET, serveAppImmutableNodes_2D1KAmoXJs);
    server->on("/app/immutable/nodes/3.7YN5jLyV.js", HTTP_GET, serveAppImmutableNodes_3_7Yn5jLyVJs);
    server->on("/app/immutable/nodes/4.DgXJbwdK.js", HTTP_GET, serveAppImmutableNodes_4DgXJbwdKJs);
    server->on("/app/immutable/nodes/5.Nf6HnOSX.js", HTTP_GET, serveAppImmutableNodes_5Nf6HnOsxJs);
    server->on("/app/immutable/nodes/6.PlRdEYFU.js", HTTP_GET, serveAppImmutableNodes_6PlRdEyfuJs);
    server->on("/app/immutable/nodes/7.C0Rf5oOS.js", HTTP_GET, serveAppImmutableNodes_7C0Rf5oOsJs);
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
