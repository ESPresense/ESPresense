/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,365 bytes
 * ui_app_immutable_chunks_js: 44,328 bytes
 * ui_app_immutable_entry_js: 224 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 57,730 bytes
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
    server->on("/app/immutable/chunks/DfKMpNM3.js", HTTP_GET, serveAppImmutableChunksDfKMpNm3Js);
    server->on("/app/immutable/entry/app.f1xA8tYB.js", HTTP_GET, serveAppImmutableEntryAppF1xA8tYbJs);
    server->on("/app/immutable/entry/start.DWUfuLlk.js", HTTP_GET, serveAppImmutableEntryStartDwUfuLlkJs);
    server->on("/app/immutable/nodes/0.BpUMrsxe.js", HTTP_GET, serveAppImmutableNodes_0BpUMrsxeJs);
    server->on("/app/immutable/nodes/1.FH0nAYsZ.js", HTTP_GET, serveAppImmutableNodes_1Fh0nAYsZJs);
    server->on("/app/immutable/nodes/2.B9h3vkVi.js", HTTP_GET, serveAppImmutableNodes_2B9h3vkViJs);
    server->on("/app/immutable/nodes/3.DNSqvfMd.js", HTTP_GET, serveAppImmutableNodes_3DnSqvfMdJs);
    server->on("/app/immutable/nodes/4.Ci0Xog3L.js", HTTP_GET, serveAppImmutableNodes_4Ci0Xog3LJs);
    server->on("/app/immutable/nodes/5.Do1_zzRQ.js", HTTP_GET, serveAppImmutableNodes_5Do1ZzRqJs);
    server->on("/app/immutable/nodes/6.D7KGf8V1.js", HTTP_GET, serveAppImmutableNodes_6D7KGf8V1Js);
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
