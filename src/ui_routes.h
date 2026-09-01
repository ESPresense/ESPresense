/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 14,520 bytes
 * ui_app_immutable_chunks_js: 66,325 bytes
 * ui_app_immutable_entry_js: 270 bytes
 * ui_app_immutable_nodes_js: 542 bytes
 * ui_html: 4,648 bytes
 * ui_svg: 456 bytes
 * Total: 86,761 bytes
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
    server->on("/app/immutable/assets/index.DKQns9on.css", HTTP_GET, serveAppImmutableAssetsIndexDkQns9onCss);
    server->on("/app/immutable/chunks/CZL5u05L.js", HTTP_GET, serveAppImmutableChunksCzl5u05LJs);
    server->on("/app/immutable/entry/app.ul1wpksJ.js", HTTP_GET, serveAppImmutableEntryAppUl1wpksJJs);
    server->on("/app/immutable/entry/start.BsBKly8Y.js", HTTP_GET, serveAppImmutableEntryStartBsBKly8YJs);
    server->on("/app/immutable/nodes/0.BK_GtiXX.js", HTTP_GET, serveAppImmutableNodes_0BkGtiXxJs);
    server->on("/app/immutable/nodes/1.BCjOb8pG.js", HTTP_GET, serveAppImmutableNodes_1BCjOb8pGJs);
    server->on("/app/immutable/nodes/2.CZpOwcz4.js", HTTP_GET, serveAppImmutableNodes_2CZpOwcz4Js);
    server->on("/app/immutable/nodes/3.BSm8V1pK.js", HTTP_GET, serveAppImmutableNodes_3BSm8V1pKJs);
    server->on("/app/immutable/nodes/4.BYUv8guk.js", HTTP_GET, serveAppImmutableNodes_4ByUv8gukJs);
    server->on("/app/immutable/nodes/5.DhDitXsL.js", HTTP_GET, serveAppImmutableNodes_5DhDitXsLJs);
    server->on("/app/immutable/nodes/6.paC06G5a.js", HTTP_GET, serveAppImmutableNodes_6PaC06G5aJs);
    server->on("/app/immutable/nodes/7.ClQOM2E9.js", HTTP_GET, serveAppImmutableNodes_7ClQom2E9Js);
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
