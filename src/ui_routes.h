/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,365 bytes
 * ui_app_immutable_chunks_js: 44,203 bytes
 * ui_app_immutable_entry_js: 225 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 57,606 bytes
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
    server->on("/app/immutable/chunks/BneOhVRX.js", HTTP_GET, serveAppImmutableChunksBneOhVrxJs);
    server->on("/app/immutable/entry/app.B6JxObgv.js", HTTP_GET, serveAppImmutableEntryAppB6JxObgvJs);
    server->on("/app/immutable/entry/start.CJX_gF25.js", HTTP_GET, serveAppImmutableEntryStartCjxGF25Js);
    server->on("/app/immutable/nodes/0.QaC1Z02e.js", HTTP_GET, serveAppImmutableNodes_0QaC1Z02eJs);
    server->on("/app/immutable/nodes/1.B_85vmqD.js", HTTP_GET, serveAppImmutableNodes_1B_85vmqDJs);
    server->on("/app/immutable/nodes/2.B9nHh1jl.js", HTTP_GET, serveAppImmutableNodes_2B9nHh1jlJs);
    server->on("/app/immutable/nodes/3.BzyfEOXy.js", HTTP_GET, serveAppImmutableNodes_3BzyfEoXyJs);
    server->on("/app/immutable/nodes/4.BRiyYIgC.js", HTTP_GET, serveAppImmutableNodes_4BRiyYIgCJs);
    server->on("/app/immutable/nodes/5.iiEp8n2u.js", HTTP_GET, serveAppImmutableNodes_5IiEp8n2uJs);
    server->on("/app/immutable/nodes/6.C-eOvenc.js", HTTP_GET, serveAppImmutableNodes_6CEOvencJs);
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
