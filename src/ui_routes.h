/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,299 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 48,468 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 62,992 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.dcg9vOzM.css", HTTP_GET, serveAppImmutableAssetsInternalDcg9vOzMCss);
    server->on("/app/immutable/assets/start.6opmQcx_.css", HTTP_GET, serveAppImmutableAssetsStart_6opmQcxCss);
    server->on("/app/immutable/entry/app.DTIFvS-9.js", HTTP_GET, serveAppImmutableEntryAppDtiFvS_9Js);
    server->on("/app/immutable/entry/start.PzA9hfVO.js", HTTP_GET, serveAppImmutableEntryStartPzA9hfVoJs);
    server->on("/app/immutable/nodes/0.CMY6bvLF.js", HTTP_GET, serveAppImmutableNodes_0Cmy6bvLfJs);
    server->on("/app/immutable/nodes/1.zRszGu82.js", HTTP_GET, serveAppImmutableNodes_1ZRszGu82Js);
    server->on("/app/immutable/nodes/2.B5bY25w8.js", HTTP_GET, serveAppImmutableNodes_2B5bY25w8Js);
    server->on("/app/immutable/nodes/3.kFusBTJL.js", HTTP_GET, serveAppImmutableNodes_3KFusBtjlJs);
    server->on("/app/immutable/nodes/4.wNhnIpaX.js", HTTP_GET, serveAppImmutableNodes_4WNhnIpaXJs);
    server->on("/app/immutable/nodes/5.C2NOaeM5.js", HTTP_GET, serveAppImmutableNodes_5C2NOaeM5Js);
    server->on("/app/immutable/nodes/6.devkiHZs.js", HTTP_GET, serveAppImmutableNodes_6DevkiHZsJs);
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
