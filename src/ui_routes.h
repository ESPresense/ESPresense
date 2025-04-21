/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 44,867 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 57,955 bytes
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
    server->on("/app/immutable/entry/app.BXHWN7hw.js", HTTP_GET, serveAppImmutableEntryAppBxhwn7hwJs);
    server->on("/app/immutable/entry/start.B1rG3JIe.js", HTTP_GET, serveAppImmutableEntryStartB1rG3JIeJs);
    server->on("/app/immutable/nodes/0.f5PV36an.js", HTTP_GET, serveAppImmutableNodes_0F5Pv36anJs);
    server->on("/app/immutable/nodes/1.L4VjsWlt.js", HTTP_GET, serveAppImmutableNodes_1L4VjsWltJs);
    server->on("/app/immutable/nodes/2.COlE-wEH.js", HTTP_GET, serveAppImmutableNodes_2COlEWEhJs);
    server->on("/app/immutable/nodes/3.DYmv1N1M.js", HTTP_GET, serveAppImmutableNodes_3DYmv1N1MJs);
    server->on("/app/immutable/nodes/4.OKACfHTv.js", HTTP_GET, serveAppImmutableNodes_4OkaCfHTvJs);
    server->on("/app/immutable/nodes/5.BRFqtOCe.js", HTTP_GET, serveAppImmutableNodes_5BrFqtOCeJs);
    server->on("/app/immutable/nodes/6.CFdYhFd5.js", HTTP_GET, serveAppImmutableNodes_6CFdYhFd5Js);
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
