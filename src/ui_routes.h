/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,206 bytes
 * ui_html: 3,894 bytes
 * ui_app_immutable_entry_js: 64,631 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 79,777 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.BzbuOEt-.css", HTTP_GET, serveAppImmutableAssetsInternalBzbuOEtCss);
    server->on("/app/immutable/assets/start.-4VrLTrv.css", HTTP_GET, serveAppImmutableAssetsStart_4VrLTrvCss);
    server->on("/app/immutable/entry/app.hWEFeaOs.js", HTTP_GET, serveAppImmutableEntryAppHWeFeaOsJs);
    server->on("/app/immutable/entry/start.21Paknye.js", HTTP_GET, serveAppImmutableEntryStart_21PaknyeJs);
    server->on("/app/immutable/nodes/0.JXjyZzU3.js", HTTP_GET, serveAppImmutableNodes_0JXjyZzU3Js);
    server->on("/app/immutable/nodes/1.-wiuaE2X.js", HTTP_GET, serveAppImmutableNodes_1WiuaE2XJs);
    server->on("/app/immutable/nodes/2.B7VfRWoZ.js", HTTP_GET, serveAppImmutableNodes_2B7VfRWoZJs);
    server->on("/app/immutable/nodes/3.D1Y6jtp1.js", HTTP_GET, serveAppImmutableNodes_3D1Y6jtp1Js);
    server->on("/app/immutable/nodes/4.Df9u6uKt.js", HTTP_GET, serveAppImmutableNodes_4Df9u6uKtJs);
    server->on("/app/immutable/nodes/5.DgvuJw5F.js", HTTP_GET, serveAppImmutableNodes_5DgvuJw5FJs);
    server->on("/app/immutable/nodes/6.Fk2o93Vp.js", HTTP_GET, serveAppImmutableNodes_6Fk2o93VpJs);
    server->on("/app/immutable/nodes/7.DcegvZt9.js", HTTP_GET, serveAppImmutableNodes_7DcegvZt9Js);
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
