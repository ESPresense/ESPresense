/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,144 bytes
 * ui_html: 4,320 bytes
 * ui_app_immutable_entry_js: 66,696 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 97,206 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.BieM-44V.css", HTTP_GET, serveAppImmutableAssetsInternalBieM_44VCss);
    server->on("/app/immutable/assets/start.BieM-44V.css", HTTP_GET, serveAppImmutableAssetsStartBieM_44VCss);
    server->on("/app/immutable/entry/app.DlfSQFv4.js", HTTP_GET, serveAppImmutableEntryAppDlfSqFv4Js);
    server->on("/app/immutable/entry/start.CQlUj3-n.js", HTTP_GET, serveAppImmutableEntryStartCQlUj3NJs);
    server->on("/app/immutable/nodes/0.1R4oXa5B.js", HTTP_GET, serveAppImmutableNodes_0_1R4oXa5BJs);
    server->on("/app/immutable/nodes/1.C-uu40Lx.js", HTTP_GET, serveAppImmutableNodes_1CUu40LxJs);
    server->on("/app/immutable/nodes/2.2mDZkmq8.js", HTTP_GET, serveAppImmutableNodes_2_2mDZkmq8Js);
    server->on("/app/immutable/nodes/3.DmVWxbE1.js", HTTP_GET, serveAppImmutableNodes_3DmVWxbE1Js);
    server->on("/app/immutable/nodes/4.CdoNOaTW.js", HTTP_GET, serveAppImmutableNodes_4CdoNOaTwJs);
    server->on("/app/immutable/nodes/5.D8FdhGEF.js", HTTP_GET, serveAppImmutableNodes_5D8FdhGefJs);
    server->on("/app/immutable/nodes/6.PgNk6-Hp.js", HTTP_GET, serveAppImmutableNodes_6PgNk6HpJs);
    server->on("/app/immutable/nodes/7.Ch3jiJp2.js", HTTP_GET, serveAppImmutableNodes_7Ch3jiJp2Js);
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
