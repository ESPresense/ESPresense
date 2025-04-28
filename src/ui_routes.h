/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 44,929 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 58,022 bytes
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
    server->on("/app/immutable/entry/app.BATHx2kE.js", HTTP_GET, serveAppImmutableEntryAppBatHx2kEJs);
    server->on("/app/immutable/entry/start.DroznD_7.js", HTTP_GET, serveAppImmutableEntryStartDroznD_7Js);
    server->on("/app/immutable/nodes/0.X_smzdYI.js", HTTP_GET, serveAppImmutableNodes_0XSmzdYiJs);
    server->on("/app/immutable/nodes/1.Cg69G9qH.js", HTTP_GET, serveAppImmutableNodes_1Cg69G9qHJs);
    server->on("/app/immutable/nodes/2.Rdpf3CQh.js", HTTP_GET, serveAppImmutableNodes_2Rdpf3CQhJs);
    server->on("/app/immutable/nodes/3.ImQngyp5.js", HTTP_GET, serveAppImmutableNodes_3ImQngyp5Js);
    server->on("/app/immutable/nodes/4.DRNefNQ3.js", HTTP_GET, serveAppImmutableNodes_4DrNefNq3Js);
    server->on("/app/immutable/nodes/5.BwLM_Ql_.js", HTTP_GET, serveAppImmutableNodes_5BwLmQlJs);
    server->on("/app/immutable/nodes/6.XLu-jzBM.js", HTTP_GET, serveAppImmutableNodes_6XLuJzBmJs);
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
