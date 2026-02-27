/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,984 bytes
 * ui_html: 4,326 bytes
 * ui_app_immutable_entry_js: 63,849 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,205 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CMfVrT3e.css", HTTP_GET, serveAppImmutableAssetsInternalCMfVrT3eCss);
    server->on("/app/immutable/assets/start.CMfVrT3e.css", HTTP_GET, serveAppImmutableAssetsStartCMfVrT3eCss);
    server->on("/app/immutable/entry/app.CnfZK7rm.js", HTTP_GET, serveAppImmutableEntryAppCnfZk7rmJs);
    server->on("/app/immutable/entry/start.D6Lq0z0B.js", HTTP_GET, serveAppImmutableEntryStartD6Lq0z0BJs);
    server->on("/app/immutable/nodes/0.DdKoBOa3.js", HTTP_GET, serveAppImmutableNodes_0DdKoBOa3Js);
    server->on("/app/immutable/nodes/1.D8i7Iiow.js", HTTP_GET, serveAppImmutableNodes_1D8i7IiowJs);
    server->on("/app/immutable/nodes/2.BdKe7OjK.js", HTTP_GET, serveAppImmutableNodes_2BdKe7OjKJs);
    server->on("/app/immutable/nodes/3.Cd0q1jqo.js", HTTP_GET, serveAppImmutableNodes_3Cd0q1jqoJs);
    server->on("/app/immutable/nodes/4.CTLYl01S.js", HTTP_GET, serveAppImmutableNodes_4CtlYl01SJs);
    server->on("/app/immutable/nodes/5.BiWNTS2i.js", HTTP_GET, serveAppImmutableNodes_5BiWnts2iJs);
    server->on("/app/immutable/nodes/6.CqxCyhY1.js", HTTP_GET, serveAppImmutableNodes_6CqxCyhY1Js);
    server->on("/app/immutable/nodes/7.DTlN5mb8.js", HTTP_GET, serveAppImmutableNodes_7DTlN5mb8Js);
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
