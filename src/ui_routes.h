/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,360 bytes
 * ui_app_immutable_chunks_js: 44,629 bytes
 * ui_app_immutable_entry_js: 244 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 58,046 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_chunks_js.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/index.DN2mWWgh.css", HTTP_GET, serveAppImmutableAssetsIndexDn2mWWghCss);
    server->on("/app/immutable/assets/index.BfkQNPT2.css", HTTP_GET, serveAppImmutableAssetsIndexBfkQnpt2Css);
    server->on("/app/immutable/chunks/CZi9d9Ai.js", HTTP_GET, serveAppImmutableChunksCZi9d9AiJs);
    server->on("/app/immutable/entry/app.C2sOnQk-.js", HTTP_GET, serveAppImmutableEntryAppC2sOnQkJs);
    server->on("/app/immutable/entry/start.BMo8JMNl.js", HTTP_GET, serveAppImmutableEntryStartBMo8JmNlJs);
    server->on("/app/immutable/nodes/0.Cbr1lgrf.js", HTTP_GET, serveAppImmutableNodes_0Cbr1lgrfJs);
    server->on("/app/immutable/nodes/1.CVxTH1Fq.js", HTTP_GET, serveAppImmutableNodes_1CVxTh1FqJs);
    server->on("/app/immutable/nodes/2.DLPgUGys.js", HTTP_GET, serveAppImmutableNodes_2DlPgUGysJs);
    server->on("/app/immutable/nodes/3.C0NTJVxm.js", HTTP_GET, serveAppImmutableNodes_3C0NtjVxmJs);
    server->on("/app/immutable/nodes/4.CfLYG7is.js", HTTP_GET, serveAppImmutableNodes_4CfLyg7isJs);
    server->on("/app/immutable/nodes/5.DDIjG8mS.js", HTTP_GET, serveAppImmutableNodes_5DdIjG8mSJs);
    server->on("/app/immutable/nodes/6.-8xDfO-d.js", HTTP_GET, serveAppImmutableNodes_6_8xDfODJs);
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
