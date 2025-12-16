/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 23,916 bytes
 * ui_html: 3,894 bytes
 * ui_app_immutable_entry_js: 62,505 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 91,361 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CE_zV8oA.css", HTTP_GET, serveAppImmutableAssetsInternalCeZV8oACss);
    server->on("/app/immutable/assets/start.CE_zV8oA.css", HTTP_GET, serveAppImmutableAssetsStartCeZV8oACss);
    server->on("/app/immutable/entry/app.QJb8griK.js", HTTP_GET, serveAppImmutableEntryAppQJb8griKJs);
    server->on("/app/immutable/entry/start.De3oG-1E.js", HTTP_GET, serveAppImmutableEntryStartDe3oG_1EJs);
    server->on("/app/immutable/nodes/0.De7uLy9d.js", HTTP_GET, serveAppImmutableNodes_0De7uLy9dJs);
    server->on("/app/immutable/nodes/1.CajOcG0K.js", HTTP_GET, serveAppImmutableNodes_1CajOcG0KJs);
    server->on("/app/immutable/nodes/2.C44MdsKH.js", HTTP_GET, serveAppImmutableNodes_2C44MdsKhJs);
    server->on("/app/immutable/nodes/3.BKQnYfaB.js", HTTP_GET, serveAppImmutableNodes_3BkQnYfaBJs);
    server->on("/app/immutable/nodes/4.6cirHy2V.js", HTTP_GET, serveAppImmutableNodes_4_6cirHy2VJs);
    server->on("/app/immutable/nodes/5.C7lQYxWc.js", HTTP_GET, serveAppImmutableNodes_5C7lQYxWcJs);
    server->on("/app/immutable/nodes/6.CRDgtLA-.js", HTTP_GET, serveAppImmutableNodes_6CrDgtLaJs);
    server->on("/app/immutable/nodes/7.Cktc9-Kx.js", HTTP_GET, serveAppImmutableNodes_7Cktc9KxJs);
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
