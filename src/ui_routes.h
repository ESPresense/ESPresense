/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,355 bytes
 * ui_app_immutable_chunks_js: 44,671 bytes
 * ui_app_immutable_entry_js: 245 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 58,084 bytes
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
    server->on("/app/immutable/chunks/DvbrZJkI.js", HTTP_GET, serveAppImmutableChunksDvbrZJkIJs);
    server->on("/app/immutable/entry/app.BCs34oZZ.js", HTTP_GET, serveAppImmutableEntryAppBCs34oZzJs);
    server->on("/app/immutable/entry/start.C4rLcjnK.js", HTTP_GET, serveAppImmutableEntryStartC4rLcjnKJs);
    server->on("/app/immutable/nodes/0.LeMJtQZX.js", HTTP_GET, serveAppImmutableNodes_0LeMJtQzxJs);
    server->on("/app/immutable/nodes/1.q45BifHO.js", HTTP_GET, serveAppImmutableNodes_1Q45BifHoJs);
    server->on("/app/immutable/nodes/2.oYPKgX5t.js", HTTP_GET, serveAppImmutableNodes_2OYpKgX5tJs);
    server->on("/app/immutable/nodes/3.BkuBdjlZ.js", HTTP_GET, serveAppImmutableNodes_3BkuBdjlZJs);
    server->on("/app/immutable/nodes/4.N6rHGgFH.js", HTTP_GET, serveAppImmutableNodes_4N6rHGgFhJs);
    server->on("/app/immutable/nodes/5.DNYmlgwG.js", HTTP_GET, serveAppImmutableNodes_5DnYmlgwGJs);
    server->on("/app/immutable/nodes/6.0_it5A9d.js", HTTP_GET, serveAppImmutableNodes_6_0It5A9dJs);
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
