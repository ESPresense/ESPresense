/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,571 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 66,274 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 81,080 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.wrGhOo2C.css", HTTP_GET, serveAppImmutableAssetsInternalWrGhOo2CCss);
    server->on("/app/immutable/assets/start.DOTUP31V.css", HTTP_GET, serveAppImmutableAssetsStartDotup31VCss);
    server->on("/app/immutable/entry/app.yMuxMYtY.js", HTTP_GET, serveAppImmutableEntryAppYMuxMYtYJs);
    server->on("/app/immutable/entry/start.B-4jpoqH.js", HTTP_GET, serveAppImmutableEntryStartB_4jpoqHJs);
    server->on("/app/immutable/nodes/0.DV-STsaS.js", HTTP_GET, serveAppImmutableNodes_0DvSTsaSJs);
    server->on("/app/immutable/nodes/1.CD46yAmv.js", HTTP_GET, serveAppImmutableNodes_1Cd46yAmvJs);
    server->on("/app/immutable/nodes/2.8V8ZG30N.js", HTTP_GET, serveAppImmutableNodes_2_8V8Zg30NJs);
    server->on("/app/immutable/nodes/3.Bd2xm1IO.js", HTTP_GET, serveAppImmutableNodes_3Bd2xm1IoJs);
    server->on("/app/immutable/nodes/4.CoYJeCHg.js", HTTP_GET, serveAppImmutableNodes_4CoYJeCHgJs);
    server->on("/app/immutable/nodes/5.CYmDA91v.js", HTTP_GET, serveAppImmutableNodes_5CYmDa91vJs);
    server->on("/app/immutable/nodes/6.CijU0gg6.js", HTTP_GET, serveAppImmutableNodes_6CijU0gg6Js);
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
