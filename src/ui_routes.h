/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 14,608 bytes
 * ui_app_immutable_chunks_js: 66,771 bytes
 * ui_app_immutable_entry_js: 270 bytes
 * ui_app_immutable_nodes_js: 542 bytes
 * ui_html: 4,649 bytes
 * ui_svg: 456 bytes
 * Total: 87,296 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_app_immutable_chunks_js.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_html.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/index.KudJptE9.css", HTTP_GET, serveAppImmutableAssetsIndexKudJptE9Css);
    server->on("/app/immutable/chunks/BSs3I4VU.js", HTTP_GET, serveAppImmutableChunksBSs3I4VuJs);
    server->on("/app/immutable/entry/app.DH-m-eQg.js", HTTP_GET, serveAppImmutableEntryAppDhMEQgJs);
    server->on("/app/immutable/entry/start.aXIzD-VY.js", HTTP_GET, serveAppImmutableEntryStartAXIzDVyJs);
    server->on("/app/immutable/nodes/0.DRkd2lkX.js", HTTP_GET, serveAppImmutableNodes_0DRkd2lkXJs);
    server->on("/app/immutable/nodes/1.BvrBTz0m.js", HTTP_GET, serveAppImmutableNodes_1BvrBTz0mJs);
    server->on("/app/immutable/nodes/2.Crao-LX_.js", HTTP_GET, serveAppImmutableNodes_2CraoLxJs);
    server->on("/app/immutable/nodes/3.BZGiU0Q8.js", HTTP_GET, serveAppImmutableNodes_3BzGiU0Q8Js);
    server->on("/app/immutable/nodes/4.fL2L6Ees.js", HTTP_GET, serveAppImmutableNodes_4FL2L6EesJs);
    server->on("/app/immutable/nodes/5.Cfzip6HL.js", HTTP_GET, serveAppImmutableNodes_5Cfzip6HlJs);
    server->on("/app/immutable/nodes/6.B0k7Ft3X.js", HTTP_GET, serveAppImmutableNodes_6B0k7Ft3XJs);
    server->on("/app/immutable/nodes/7.D5WVT_ON.js", HTTP_GET, serveAppImmutableNodes_7D5WvtOnJs);
    server->on("/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes
    server->on("/", HTTP_GET, serveIndexHtml);
    server->on("/devices", HTTP_GET, serveDevicesHtml);
    server->on("/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/hardware", HTTP_GET, serveHardwareHtml);
    server->on("/hardware.html", HTTP_GET, serveHardwareHtml);
    server->on("/network", HTTP_GET, serveNetworkHtml);
    server->on("/network.html", HTTP_GET, serveNetworkHtml);
    server->on("/settings", HTTP_GET, serveSettingsHtml);
    server->on("/settings.html", HTTP_GET, serveSettingsHtml);
}
