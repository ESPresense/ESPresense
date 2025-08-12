/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,210 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 45,389 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 59,829 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.R2nWuKlp.css", HTTP_GET, serveAppImmutableAssetsInternalR2nWuKlpCss);
    server->on("/app/immutable/assets/start.CquYjHqP.css", HTTP_GET, serveAppImmutableAssetsStartCquYjHqPCss);
    server->on("/app/immutable/entry/app.B8gdwm4O.js", HTTP_GET, serveAppImmutableEntryAppB8gdwm4OJs);
    server->on("/app/immutable/entry/start.DD4IK5-p.js", HTTP_GET, serveAppImmutableEntryStartDd4Ik5PJs);
    server->on("/app/immutable/nodes/0.BoAJmDwQ.js", HTTP_GET, serveAppImmutableNodes_0BoAJmDwQJs);
    server->on("/app/immutable/nodes/1.DdpLyfHz.js", HTTP_GET, serveAppImmutableNodes_1DdpLyfHzJs);
    server->on("/app/immutable/nodes/2.BWuqOQnK.js", HTTP_GET, serveAppImmutableNodes_2BWuqOQnKJs);
    server->on("/app/immutable/nodes/3.DKPjN6eF.js", HTTP_GET, serveAppImmutableNodes_3DkPjN6eFJs);
    server->on("/app/immutable/nodes/4.DygRHcHR.js", HTTP_GET, serveAppImmutableNodes_4DygRHcHrJs);
    server->on("/app/immutable/nodes/5.BsnFwXbi.js", HTTP_GET, serveAppImmutableNodes_5BsnFwXbiJs);
    server->on("/app/immutable/nodes/6.DiZZFE_9.js", HTTP_GET, serveAppImmutableNodes_6DiZzfe_9Js);
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
