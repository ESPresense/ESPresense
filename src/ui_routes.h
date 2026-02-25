/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,160 bytes
 * ui_html: 4,326 bytes
 * ui_app_immutable_entry_js: 64,159 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,691 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DGAxltaV.css", HTTP_GET, serveAppImmutableAssetsInternalDgAxltaVCss);
    server->on("/app/immutable/assets/start.DGAxltaV.css", HTTP_GET, serveAppImmutableAssetsStartDgAxltaVCss);
    server->on("/app/immutable/entry/app.Cnk56V21.js", HTTP_GET, serveAppImmutableEntryAppCnk56V21Js);
    server->on("/app/immutable/entry/start.qkftbBiL.js", HTTP_GET, serveAppImmutableEntryStartQkftbBiLJs);
    server->on("/app/immutable/nodes/0.c9ettZyQ.js", HTTP_GET, serveAppImmutableNodes_0C9ettZyQJs);
    server->on("/app/immutable/nodes/1.Cwicl3JW.js", HTTP_GET, serveAppImmutableNodes_1Cwicl3JwJs);
    server->on("/app/immutable/nodes/2.MOljavcI.js", HTTP_GET, serveAppImmutableNodes_2MOljavcIJs);
    server->on("/app/immutable/nodes/3.BU4-lAo6.js", HTTP_GET, serveAppImmutableNodes_3Bu4LAo6Js);
    server->on("/app/immutable/nodes/4.qCcZZSMk.js", HTTP_GET, serveAppImmutableNodes_4QCcZzsMkJs);
    server->on("/app/immutable/nodes/5.BVkRa74m.js", HTTP_GET, serveAppImmutableNodes_5BVkRa74mJs);
    server->on("/app/immutable/nodes/6.BpEQwYqS.js", HTTP_GET, serveAppImmutableNodes_6BpEQwYqSJs);
    server->on("/app/immutable/nodes/7.Cpbt5yoz.js", HTTP_GET, serveAppImmutableNodes_7Cpbt5yozJs);
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
