/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,367 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 48,138 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 62,735 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.C8cCt54N.css", HTTP_GET, serveAppImmutableAssetsInternalC8cCt54NCss);
    server->on("/app/immutable/assets/start.CB0Tr19N.css", HTTP_GET, serveAppImmutableAssetsStartCb0Tr19NCss);
    server->on("/app/immutable/entry/app.l_-4_H5l.js", HTTP_GET, serveAppImmutableEntryAppL_4H5lJs);
    server->on("/app/immutable/entry/start.D3wTG2pJ.js", HTTP_GET, serveAppImmutableEntryStartD3wTg2pJJs);
    server->on("/app/immutable/nodes/0.Ce_zU0hl.js", HTTP_GET, serveAppImmutableNodes_0CeZU0hlJs);
    server->on("/app/immutable/nodes/1.BMF_sAR2.js", HTTP_GET, serveAppImmutableNodes_1BmfSAr2Js);
    server->on("/app/immutable/nodes/2.DNY-GPLZ.js", HTTP_GET, serveAppImmutableNodes_2DnyGplzJs);
    server->on("/app/immutable/nodes/3.BWinamzY.js", HTTP_GET, serveAppImmutableNodes_3BWinamzYJs);
    server->on("/app/immutable/nodes/4.CiB3pmI-.js", HTTP_GET, serveAppImmutableNodes_4CiB3pmIJs);
    server->on("/app/immutable/nodes/5.CS-vd3ms.js", HTTP_GET, serveAppImmutableNodes_5CsVd3msJs);
    server->on("/app/immutable/nodes/6.DfjkdnoJ.js", HTTP_GET, serveAppImmutableNodes_6DfjkdnoJJs);
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
