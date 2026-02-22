/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,326 bytes
 * ui_app_immutable_entry_js: 63,697 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 93,945 bytes
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
    server->on("/app/immutable/entry/app.C-nG6OHE.js", HTTP_GET, serveAppImmutableEntryAppCNG6OheJs);
    server->on("/app/immutable/entry/start.CltgPORp.js", HTTP_GET, serveAppImmutableEntryStartCltgPoRpJs);
    server->on("/app/immutable/nodes/0.CVgHF7s8.js", HTTP_GET, serveAppImmutableNodes_0CVgHf7s8Js);
    server->on("/app/immutable/nodes/1.Q8bgZe4S.js", HTTP_GET, serveAppImmutableNodes_1Q8bgZe4SJs);
    server->on("/app/immutable/nodes/2.DSxSbTGK.js", HTTP_GET, serveAppImmutableNodes_2DSxSbTgkJs);
    server->on("/app/immutable/nodes/3.BMAVxrZO.js", HTTP_GET, serveAppImmutableNodes_3BmaVxrZoJs);
    server->on("/app/immutable/nodes/4.Bjqy64kB.js", HTTP_GET, serveAppImmutableNodes_4Bjqy64kBJs);
    server->on("/app/immutable/nodes/5.D_crY5E7.js", HTTP_GET, serveAppImmutableNodes_5DCrY5E7Js);
    server->on("/app/immutable/nodes/6.BepZ_0Tl.js", HTTP_GET, serveAppImmutableNodes_6BepZ_0TlJs);
    server->on("/app/immutable/nodes/7.BqKGOTBR.js", HTTP_GET, serveAppImmutableNodes_7BqKgotbrJs);
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
