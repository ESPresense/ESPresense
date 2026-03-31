/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,144 bytes
 * ui_html: 4,332 bytes
 * ui_app_immutable_entry_js: 64,550 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,072 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.BieM-44V.css", HTTP_GET, serveAppImmutableAssetsInternalBieM_44VCss);
    server->on("/app/immutable/assets/start.BieM-44V.css", HTTP_GET, serveAppImmutableAssetsStartBieM_44VCss);
    server->on("/app/immutable/entry/app.CKDlZqZR.js", HTTP_GET, serveAppImmutableEntryAppCkDlZqZrJs);
    server->on("/app/immutable/entry/start.kYp__ENL.js", HTTP_GET, serveAppImmutableEntryStartKYpEnlJs);
    server->on("/app/immutable/nodes/0.C_yTliLR.js", HTTP_GET, serveAppImmutableNodes_0CYTliLrJs);
    server->on("/app/immutable/nodes/1.uX_7Q5NU.js", HTTP_GET, serveAppImmutableNodes_1UX_7Q5NuJs);
    server->on("/app/immutable/nodes/2.BVuagVD0.js", HTTP_GET, serveAppImmutableNodes_2BVuagVd0Js);
    server->on("/app/immutable/nodes/3.DupZIlfl.js", HTTP_GET, serveAppImmutableNodes_3DupZIlflJs);
    server->on("/app/immutable/nodes/4.DYRVp_qV.js", HTTP_GET, serveAppImmutableNodes_4DyrVpQVJs);
    server->on("/app/immutable/nodes/5.HiusXgN_.js", HTTP_GET, serveAppImmutableNodes_5HiusXgNJs);
    server->on("/app/immutable/nodes/6.CgqgwNbj.js", HTTP_GET, serveAppImmutableNodes_6CgqgwNbjJs);
    server->on("/app/immutable/nodes/7.BcbbU8UX.js", HTTP_GET, serveAppImmutableNodes_7BcbbU8UxJs);
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
