/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,326 bytes
 * ui_app_immutable_entry_js: 63,752 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,000 bytes
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
    server->on("/app/immutable/entry/app.mvW8dDfa.js", HTTP_GET, serveAppImmutableEntryAppMvW8dDfaJs);
    server->on("/app/immutable/entry/start.ConPkmQJ.js", HTTP_GET, serveAppImmutableEntryStartConPkmQjJs);
    server->on("/app/immutable/nodes/0.DwKKj7bE.js", HTTP_GET, serveAppImmutableNodes_0DwKKj7bEJs);
    server->on("/app/immutable/nodes/1.KgR3w--k.js", HTTP_GET, serveAppImmutableNodes_1KgR3wKJs);
    server->on("/app/immutable/nodes/2.DREvOqDm.js", HTTP_GET, serveAppImmutableNodes_2DrEvOqDmJs);
    server->on("/app/immutable/nodes/3.DgeA73fO.js", HTTP_GET, serveAppImmutableNodes_3DgeA73fOJs);
    server->on("/app/immutable/nodes/4.DxRk6pu7.js", HTTP_GET, serveAppImmutableNodes_4DxRk6pu7Js);
    server->on("/app/immutable/nodes/5.DcOn6X42.js", HTTP_GET, serveAppImmutableNodes_5DcOn6X42Js);
    server->on("/app/immutable/nodes/6.D83Jpz9G.js", HTTP_GET, serveAppImmutableNodes_6D83Jpz9GJs);
    server->on("/app/immutable/nodes/7.CrtP902R.js", HTTP_GET, serveAppImmutableNodes_7CrtP902RJs);
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
