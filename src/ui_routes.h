/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,122 bytes
 * ui_html: 4,332 bytes
 * ui_app_immutable_entry_js: 64,122 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,622 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CKNao7CM.css", HTTP_GET, serveAppImmutableAssetsInternalCkNao7CmCss);
    server->on("/app/immutable/assets/start.CKNao7CM.css", HTTP_GET, serveAppImmutableAssetsStartCkNao7CmCss);
    server->on("/app/immutable/entry/app.C6m6evUL.js", HTTP_GET, serveAppImmutableEntryAppC6m6evUlJs);
    server->on("/app/immutable/entry/start.D__Gq5S1.js", HTTP_GET, serveAppImmutableEntryStartDGq5S1Js);
    server->on("/app/immutable/nodes/0.CZ3bYSW0.js", HTTP_GET, serveAppImmutableNodes_0Cz3bYsw0Js);
    server->on("/app/immutable/nodes/1.DYPQJ93K.js", HTTP_GET, serveAppImmutableNodes_1Dypqj93KJs);
    server->on("/app/immutable/nodes/2.CdE57A4I.js", HTTP_GET, serveAppImmutableNodes_2CdE57A4IJs);
    server->on("/app/immutable/nodes/3.Q38ljbQp.js", HTTP_GET, serveAppImmutableNodes_3Q38ljbQpJs);
    server->on("/app/immutable/nodes/4.D4kYrIlC.js", HTTP_GET, serveAppImmutableNodes_4D4kYrIlCJs);
    server->on("/app/immutable/nodes/5.BqW2f-tn.js", HTTP_GET, serveAppImmutableNodes_5BqW2fTnJs);
    server->on("/app/immutable/nodes/6.DMJbvbQH.js", HTTP_GET, serveAppImmutableNodes_6DmJbvbQhJs);
    server->on("/app/immutable/nodes/7.DGa1U-Zk.js", HTTP_GET, serveAppImmutableNodes_7DGa1UZkJs);
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
