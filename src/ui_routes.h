/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 3,258 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 45,015 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 52,493 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CQ2eoPXP.css", HTTP_GET, serveAppImmutableAssetsInternalCq2eoPxpCss);
    server->on("/app/immutable/assets/start.CQ2eoPXP.css", HTTP_GET, serveAppImmutableAssetsStartCq2eoPxpCss);
    server->on("/app/immutable/entry/app.YDHgZzVR.js", HTTP_GET, serveAppImmutableEntryAppYdHgZzVrJs);
    server->on("/app/immutable/entry/start.BPOoyXHl.js", HTTP_GET, serveAppImmutableEntryStartBpOoyXHlJs);
    server->on("/app/immutable/nodes/0.zbiDMdxO.js", HTTP_GET, serveAppImmutableNodes_0ZbiDMdxOJs);
    server->on("/app/immutable/nodes/1.D6Ex-3wI.js", HTTP_GET, serveAppImmutableNodes_1D6Ex_3wIJs);
    server->on("/app/immutable/nodes/2.BlRkjF_P.js", HTTP_GET, serveAppImmutableNodes_2BlRkjFPJs);
    server->on("/app/immutable/nodes/3.DELGf7vM.js", HTTP_GET, serveAppImmutableNodes_3DelGf7vMJs);
    server->on("/app/immutable/nodes/4.CG_F7ks4.js", HTTP_GET, serveAppImmutableNodes_4CgF7ks4Js);
    server->on("/app/immutable/nodes/5.Dysg7hFW.js", HTTP_GET, serveAppImmutableNodes_5Dysg7hFwJs);
    server->on("/app/immutable/nodes/6.5UusDKZD.js", HTTP_GET, serveAppImmutableNodes_6_5UusDkzdJs);
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
