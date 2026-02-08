/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,896 bytes
 * ui_html: 4,326 bytes
 * ui_app_immutable_entry_js: 63,219 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 93,487 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.Bh-VUp50.css", HTTP_GET, serveAppImmutableAssetsInternalBhVUp50Css);
    server->on("/app/immutable/assets/start.Bh-VUp50.css", HTTP_GET, serveAppImmutableAssetsStartBhVUp50Css);
    server->on("/app/immutable/entry/app.BwYjq-E-.js", HTTP_GET, serveAppImmutableEntryAppBwYjqEJs);
    server->on("/app/immutable/entry/start.BmP83ufm.js", HTTP_GET, serveAppImmutableEntryStartBmP83ufmJs);
    server->on("/app/immutable/nodes/0.C99ytfql.js", HTTP_GET, serveAppImmutableNodes_0C99ytfqlJs);
    server->on("/app/immutable/nodes/1.9kNu6Kf3.js", HTTP_GET, serveAppImmutableNodes_1_9kNu6Kf3Js);
    server->on("/app/immutable/nodes/2.CYXHa36S.js", HTTP_GET, serveAppImmutableNodes_2CyxHa36SJs);
    server->on("/app/immutable/nodes/3.AnKGz-_s.js", HTTP_GET, serveAppImmutableNodes_3AnKGzSJs);
    server->on("/app/immutable/nodes/4.Cyv0yE-R.js", HTTP_GET, serveAppImmutableNodes_4Cyv0yERJs);
    server->on("/app/immutable/nodes/5.CeXvc4pU.js", HTTP_GET, serveAppImmutableNodes_5CeXvc4pUJs);
    server->on("/app/immutable/nodes/6.jGUw2ey6.js", HTTP_GET, serveAppImmutableNodes_6JGUw2ey6Js);
    server->on("/app/immutable/nodes/7.BfOV419L.js", HTTP_GET, serveAppImmutableNodes_7BfOv419LJs);
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
