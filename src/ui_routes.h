/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,360 bytes
 * ui_app_immutable_chunks_js: 44,228 bytes
 * ui_app_immutable_entry_js: 223 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 57,624 bytes
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
    server->on("/app/immutable/chunks/CuGwzX0c.js", HTTP_GET, serveAppImmutableChunksCuGwzX0cJs);
    server->on("/app/immutable/entry/app.Ct83LITt.js", HTTP_GET, serveAppImmutableEntryAppCt83LiTtJs);
    server->on("/app/immutable/entry/start.DaASRfvt.js", HTTP_GET, serveAppImmutableEntryStartDaAsRfvtJs);
    server->on("/app/immutable/nodes/0.axHgPxCd.js", HTTP_GET, serveAppImmutableNodes_0AxHgPxCdJs);
    server->on("/app/immutable/nodes/1.BsUD3QoV.js", HTTP_GET, serveAppImmutableNodes_1BsUd3QoVJs);
    server->on("/app/immutable/nodes/2.B9trL_I7.js", HTTP_GET, serveAppImmutableNodes_2B9trLI7Js);
    server->on("/app/immutable/nodes/3.CxvJb3jY.js", HTTP_GET, serveAppImmutableNodes_3CxvJb3jYJs);
    server->on("/app/immutable/nodes/4.BDZbmoVa.js", HTTP_GET, serveAppImmutableNodes_4BdZbmoVaJs);
    server->on("/app/immutable/nodes/5.uAD1cIkO.js", HTTP_GET, serveAppImmutableNodes_5UAd1cIkOJs);
    server->on("/app/immutable/nodes/6.C_EVxkND.js", HTTP_GET, serveAppImmutableNodes_6CEVxkNdJs);
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
