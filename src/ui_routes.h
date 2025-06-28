/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 45,098 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 58,186 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DN2mWWgh.css", HTTP_GET, serveAppImmutableAssetsInternalDn2mWWghCss);
    server->on("/app/immutable/assets/start.BfkQNPT2.css", HTTP_GET, serveAppImmutableAssetsStartBfkQnpt2Css);
    server->on("/app/immutable/entry/app.BNye2WsS.js", HTTP_GET, serveAppImmutableEntryAppBNye2WsSJs);
    server->on("/app/immutable/entry/start.DnMec06s.js", HTTP_GET, serveAppImmutableEntryStartDnMec06sJs);
    server->on("/app/immutable/nodes/0.DwE92fXh.js", HTTP_GET, serveAppImmutableNodes_0DwE92fXhJs);
    server->on("/app/immutable/nodes/1.DeMN0cdX.js", HTTP_GET, serveAppImmutableNodes_1DeMn0cdXJs);
    server->on("/app/immutable/nodes/2.CsKChuMM.js", HTTP_GET, serveAppImmutableNodes_2CsKChuMmJs);
    server->on("/app/immutable/nodes/3.WS4NQrYb.js", HTTP_GET, serveAppImmutableNodes_3Ws4NQrYbJs);
    server->on("/app/immutable/nodes/4.C2cLfl-0.js", HTTP_GET, serveAppImmutableNodes_4C2cLfl_0Js);
    server->on("/app/immutable/nodes/5.Bbqgdpuc.js", HTTP_GET, serveAppImmutableNodes_5BbqgdpucJs);
    server->on("/app/immutable/nodes/6.BA2M1Bjc.js", HTTP_GET, serveAppImmutableNodes_6Ba2M1BjcJs);
    server->on("/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes
    server->on("/devices", HTTP_GET, serveDevicesHtml);
    server->on("/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/", HTTP_GET, serveIndexHtml);
    server->on("/network", HTTP_GET, serveNetworkHtml);
    server->on("/network.html", HTTP_GET, serveNetworkHtml);
    server->on("/settings", HTTP_GET, serveSettingsHtml);
    server->on("/settings.html", HTTP_GET, serveSettingsHtml);
}
