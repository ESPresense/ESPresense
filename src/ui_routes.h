/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,571 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 66,285 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 81,091 bytes
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
    server->on("/app/immutable/entry/app.Dx1HKF3g.js", HTTP_GET, serveAppImmutableEntryAppDx1Hkf3gJs);
    server->on("/app/immutable/entry/start.W1RN8rT2.js", HTTP_GET, serveAppImmutableEntryStartW1Rn8rT2Js);
    server->on("/app/immutable/nodes/0.DIUCpLFg.js", HTTP_GET, serveAppImmutableNodes_0DiuCpLFgJs);
    server->on("/app/immutable/nodes/1.BNnZVVse.js", HTTP_GET, serveAppImmutableNodes_1BNnZvVseJs);
    server->on("/app/immutable/nodes/2.RuNZENTc.js", HTTP_GET, serveAppImmutableNodes_2RuNzenTcJs);
    server->on("/app/immutable/nodes/3.DOzMxhde.js", HTTP_GET, serveAppImmutableNodes_3DOzMxhdeJs);
    server->on("/app/immutable/nodes/4.CcKxlQHp.js", HTTP_GET, serveAppImmutableNodes_4CcKxlQHpJs);
    server->on("/app/immutable/nodes/5.CAkUWUJy.js", HTTP_GET, serveAppImmutableNodes_5CAkUwuJyJs);
    server->on("/app/immutable/nodes/6.BhSNBHTm.js", HTTP_GET, serveAppImmutableNodes_6BhSnbhTmJs);
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
