/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,360 bytes
 * ui_app_immutable_chunks_js: 44,229 bytes
 * ui_app_immutable_entry_js: 245 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 57,647 bytes
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
    server->on("/app/immutable/chunks/CJSWjuhU.js", HTTP_GET, serveAppImmutableChunksCjsWjuhUJs);
    server->on("/app/immutable/entry/app.BXsIDtWe.js", HTTP_GET, serveAppImmutableEntryAppBXsIDtWeJs);
    server->on("/app/immutable/entry/start.BtQYKs_K.js", HTTP_GET, serveAppImmutableEntryStartBtQyKsKJs);
    server->on("/app/immutable/nodes/0.B1FOgyEG.js", HTTP_GET, serveAppImmutableNodes_0B1FOgyEgJs);
    server->on("/app/immutable/nodes/1.CytDQ4Qx.js", HTTP_GET, serveAppImmutableNodes_1CytDq4QxJs);
    server->on("/app/immutable/nodes/2.CexcSB2M.js", HTTP_GET, serveAppImmutableNodes_2CexcSb2MJs);
    server->on("/app/immutable/nodes/3.69KBEEBD.js", HTTP_GET, serveAppImmutableNodes_3_69KbeebdJs);
    server->on("/app/immutable/nodes/4.NaEzHRBI.js", HTTP_GET, serveAppImmutableNodes_4NaEzHrbiJs);
    server->on("/app/immutable/nodes/5.DLnt3a9b.js", HTTP_GET, serveAppImmutableNodes_5DLnt3a9bJs);
    server->on("/app/immutable/nodes/6.C0SGT1MO.js", HTTP_GET, serveAppImmutableNodes_6C0Sgt1MoJs);
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
