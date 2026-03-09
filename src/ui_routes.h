/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,122 bytes
 * ui_html: 4,326 bytes
 * ui_app_immutable_entry_js: 64,279 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,773 bytes
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
    server->on("/app/immutable/entry/app.Cqu44_i9.js", HTTP_GET, serveAppImmutableEntryAppCqu44I9Js);
    server->on("/app/immutable/entry/start.S15Zp_0N.js", HTTP_GET, serveAppImmutableEntryStartS15Zp_0NJs);
    server->on("/app/immutable/nodes/0.Cr4q5g1l.js", HTTP_GET, serveAppImmutableNodes_0Cr4q5g1lJs);
    server->on("/app/immutable/nodes/1.DsBzBWDE.js", HTTP_GET, serveAppImmutableNodes_1DsBzBwdeJs);
    server->on("/app/immutable/nodes/2.CciDk1dH.js", HTTP_GET, serveAppImmutableNodes_2CciDk1dHJs);
    server->on("/app/immutable/nodes/3.oSf0m_II.js", HTTP_GET, serveAppImmutableNodes_3OSf0mIiJs);
    server->on("/app/immutable/nodes/4.C8bYtfx2.js", HTTP_GET, serveAppImmutableNodes_4C8bYtfx2Js);
    server->on("/app/immutable/nodes/5.DGYkMzgp.js", HTTP_GET, serveAppImmutableNodes_5DgYkMzgpJs);
    server->on("/app/immutable/nodes/6.CtxPB1J-.js", HTTP_GET, serveAppImmutableNodes_6CtxPb1JJs);
    server->on("/app/immutable/nodes/7.CLRjucZM.js", HTTP_GET, serveAppImmutableNodes_7ClRjucZmJs);
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
