/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 44,787 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 57,880 bytes
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
    server->on("/app/immutable/entry/app.Bi7vy9O-.js", HTTP_GET, serveAppImmutableEntryAppBi7vy9OJs);
    server->on("/app/immutable/entry/start.tX-a67sm.js", HTTP_GET, serveAppImmutableEntryStartTXA67smJs);
    server->on("/app/immutable/nodes/0.AugoMD1s.js", HTTP_GET, serveAppImmutableNodes_0AugoMd1sJs);
    server->on("/app/immutable/nodes/1.Df7P4hjq.js", HTTP_GET, serveAppImmutableNodes_1Df7P4hjqJs);
    server->on("/app/immutable/nodes/2.Bby51NCg.js", HTTP_GET, serveAppImmutableNodes_2Bby51NCgJs);
    server->on("/app/immutable/nodes/3.8bx2Gwvn.js", HTTP_GET, serveAppImmutableNodes_3_8bx2GwvnJs);
    server->on("/app/immutable/nodes/4.BeVzXPbB.js", HTTP_GET, serveAppImmutableNodes_4BeVzXPbBJs);
    server->on("/app/immutable/nodes/5.BxEtuxh0.js", HTTP_GET, serveAppImmutableNodes_5BxEtuxh0Js);
    server->on("/app/immutable/nodes/6.1_nXCC8B.js", HTTP_GET, serveAppImmutableNodes_6_1NXcc8BJs);
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
