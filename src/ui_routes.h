/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,144 bytes
 * ui_html: 4,518 bytes
 * ui_app_immutable_entry_js: 67,281 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 97,989 bytes
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
    server->on("/app/immutable/entry/app.BJaezIzs.js", HTTP_GET, serveAppImmutableEntryAppBJaezIzsJs);
    server->on("/app/immutable/entry/start.DMKv5XdS.js", HTTP_GET, serveAppImmutableEntryStartDmKv5XdSJs);
    server->on("/app/immutable/nodes/0.eMxCsHZ8.js", HTTP_GET, serveAppImmutableNodes_0EMxCsHz8Js);
    server->on("/app/immutable/nodes/1.CnDz9b8Q.js", HTTP_GET, serveAppImmutableNodes_1CnDz9b8QJs);
    server->on("/app/immutable/nodes/2.C2bhtwQD.js", HTTP_GET, serveAppImmutableNodes_2C2bhtwQdJs);
    server->on("/app/immutable/nodes/3.C1wbiSxd.js", HTTP_GET, serveAppImmutableNodes_3C1wbiSxdJs);
    server->on("/app/immutable/nodes/4.aq5GQpxF.js", HTTP_GET, serveAppImmutableNodes_4Aq5GQpxFJs);
    server->on("/app/immutable/nodes/5.B7X5e9BU.js", HTTP_GET, serveAppImmutableNodes_5B7X5e9BuJs);
    server->on("/app/immutable/nodes/6.DlZdec7_.js", HTTP_GET, serveAppImmutableNodes_6DlZdec7Js);
    server->on("/app/immutable/nodes/7.Vzt5qjKW.js", HTTP_GET, serveAppImmutableNodes_7Vzt5qjKwJs);
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
