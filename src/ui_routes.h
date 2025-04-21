/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 44,908 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 58,001 bytes
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
    server->on("/app/immutable/entry/app.DZNPCS1S.js", HTTP_GET, serveAppImmutableEntryAppDznpcs1SJs);
    server->on("/app/immutable/entry/start.Dd77JiiQ.js", HTTP_GET, serveAppImmutableEntryStartDd77JiiQJs);
    server->on("/app/immutable/nodes/0.DMcoIYKN.js", HTTP_GET, serveAppImmutableNodes_0DMcoIyknJs);
    server->on("/app/immutable/nodes/1.y32R8rCg.js", HTTP_GET, serveAppImmutableNodes_1Y32R8rCgJs);
    server->on("/app/immutable/nodes/2.C8oBJg6U.js", HTTP_GET, serveAppImmutableNodes_2C8oBJg6UJs);
    server->on("/app/immutable/nodes/3.DkGd5Q_1.js", HTTP_GET, serveAppImmutableNodes_3DkGd5Q_1Js);
    server->on("/app/immutable/nodes/4.h91gu1QD.js", HTTP_GET, serveAppImmutableNodes_4H91gu1QdJs);
    server->on("/app/immutable/nodes/5.B9vmF3D5.js", HTTP_GET, serveAppImmutableNodes_5B9vmF3D5Js);
    server->on("/app/immutable/nodes/6.CcBrRKgT.js", HTTP_GET, serveAppImmutableNodes_6CcBrRKgTJs);
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
