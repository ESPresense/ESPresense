/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 45,471 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 58,564 bytes
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
    server->on("/app/immutable/entry/app.CSrTpksp.js", HTTP_GET, serveAppImmutableEntryAppCSrTpkspJs);
    server->on("/app/immutable/entry/start.BovXMG2k.js", HTTP_GET, serveAppImmutableEntryStartBovXmg2kJs);
    server->on("/app/immutable/nodes/0.BzhMeSQR.js", HTTP_GET, serveAppImmutableNodes_0BzhMeSqrJs);
    server->on("/app/immutable/nodes/1.w8-ZzjF5.js", HTTP_GET, serveAppImmutableNodes_1W8ZzjF5Js);
    server->on("/app/immutable/nodes/2.CEfxyUhg.js", HTTP_GET, serveAppImmutableNodes_2CEfxyUhgJs);
    server->on("/app/immutable/nodes/3.7_UBrdXc.js", HTTP_GET, serveAppImmutableNodes_3_7UBrdXcJs);
    server->on("/app/immutable/nodes/4.BF_Mhmlh.js", HTTP_GET, serveAppImmutableNodes_4BfMhmlhJs);
    server->on("/app/immutable/nodes/5.rcRtN5OM.js", HTTP_GET, serveAppImmutableNodes_5RcRtN5OmJs);
    server->on("/app/immutable/nodes/6.Ca0Rs48T.js", HTTP_GET, serveAppImmutableNodes_6Ca0Rs48TJs);
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
