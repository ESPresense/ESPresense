/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 5,278 bytes
 * ui_app_immutable_entry_js: 49,228 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 55,476 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/start.DV1CVczO.css", HTTP_GET, serveAppImmutableAssetsStartDv1CVczOCss);
    server->on("/app/immutable/entry/start.DttqJO-o.js", HTTP_GET, serveAppImmutableEntryStartDttqJoOJs);
    server->on("/app/immutable/entry/app.CdML5CPn.js", HTTP_GET, serveAppImmutableEntryAppCdMl5CPnJs);
    server->on("/app/immutable/nodes/0.BqgGQ-Ox.js", HTTP_GET, serveAppImmutableNodes_0BqgGqOxJs);
    server->on("/app/immutable/nodes/1.Crep9qCP.js", HTTP_GET, serveAppImmutableNodes_1Crep9qCpJs);
    server->on("/app/immutable/nodes/2.e_1aCVeL.js", HTTP_GET, serveAppImmutableNodes_2E_1aCVeLJs);
    server->on("/app/immutable/nodes/3.BsoS8Hrq.js", HTTP_GET, serveAppImmutableNodes_3BsoS8HrqJs);
    server->on("/app/immutable/nodes/4.BbicNRrS.js", HTTP_GET, serveAppImmutableNodes_4BbicNRrSJs);
    server->on("/app/immutable/nodes/5.D_QAaPkM.js", HTTP_GET, serveAppImmutableNodes_5DQAaPkMJs);
    server->on("/app/immutable/nodes/6.D7SmEy0u.js", HTTP_GET, serveAppImmutableNodes_6D7SmEy0uJs);
    server->on("/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes

}
