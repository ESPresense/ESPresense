/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,338 bytes
 * ui_app_immutable_entry_js: 63,875 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,135 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsInternalDAt95k88Css);
    server->on("/app/immutable/assets/start.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsStartDAt95k88Css);
    server->on("/app/immutable/entry/app.C2g_uW-Z.js", HTTP_GET, serveAppImmutableEntryAppC2gUWZJs);
    server->on("/app/immutable/entry/start.MM8tXp7D.js", HTTP_GET, serveAppImmutableEntryStartMm8tXp7DJs);
    server->on("/app/immutable/nodes/0.4lAoBQG_.js", HTTP_GET, serveAppImmutableNodes_0_4lAoBqgJs);
    server->on("/app/immutable/nodes/1.B19B3hqE.js", HTTP_GET, serveAppImmutableNodes_1B19B3hqEJs);
    server->on("/app/immutable/nodes/2.El07kw_D.js", HTTP_GET, serveAppImmutableNodes_2El07kwDJs);
    server->on("/app/immutable/nodes/3.da7wNV8J.js", HTTP_GET, serveAppImmutableNodes_3Da7wNv8JJs);
    server->on("/app/immutable/nodes/4.CPfVRD2b.js", HTTP_GET, serveAppImmutableNodes_4CPfVrd2bJs);
    server->on("/app/immutable/nodes/5.PzUIM1f2.js", HTTP_GET, serveAppImmutableNodes_5PzUim1f2Js);
    server->on("/app/immutable/nodes/6.B90QSe0x.js", HTTP_GET, serveAppImmutableNodes_6B90QSe0xJs);
    server->on("/app/immutable/nodes/7.CgitXJy3.js", HTTP_GET, serveAppImmutableNodes_7CgitXJy3Js);
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
