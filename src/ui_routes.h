/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,246 bytes
 * ui_html: 4,320 bytes
 * ui_app_immutable_entry_js: 64,611 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,223 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DxBQeS86.css", HTTP_GET, serveAppImmutableAssetsInternalDxBQeS86Css);
    server->on("/app/immutable/assets/start.DxBQeS86.css", HTTP_GET, serveAppImmutableAssetsStartDxBQeS86Css);
    server->on("/app/immutable/entry/app.Cs4xUQ4y.js", HTTP_GET, serveAppImmutableEntryAppCs4xUq4yJs);
    server->on("/app/immutable/entry/start.DkKsQ5Ux.js", HTTP_GET, serveAppImmutableEntryStartDkKsQ5UxJs);
    server->on("/app/immutable/nodes/0.B-B7hdsN.js", HTTP_GET, serveAppImmutableNodes_0BB7hdsNJs);
    server->on("/app/immutable/nodes/1.DPY27pfR.js", HTTP_GET, serveAppImmutableNodes_1Dpy27pfRJs);
    server->on("/app/immutable/nodes/2.BBVah1tz.js", HTTP_GET, serveAppImmutableNodes_2BbVah1tzJs);
    server->on("/app/immutable/nodes/3.CwCaRckL.js", HTTP_GET, serveAppImmutableNodes_3CwCaRckLJs);
    server->on("/app/immutable/nodes/4.BkiiwQWR.js", HTTP_GET, serveAppImmutableNodes_4BkiiwQwrJs);
    server->on("/app/immutable/nodes/5.7p0KlEDU.js", HTTP_GET, serveAppImmutableNodes_5_7p0KlEduJs);
    server->on("/app/immutable/nodes/6.Dp3i_DqJ.js", HTTP_GET, serveAppImmutableNodes_6Dp3iDqJJs);
    server->on("/app/immutable/nodes/7.DDyvUwEZ.js", HTTP_GET, serveAppImmutableNodes_7DDyvUwEzJs);
    server->on("/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes
    server->on("/devices", HTTP_GET, serveDevicesHtml);
    server->on("/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/hardware", HTTP_GET, serveHardwareHtml);
    server->on("/hardware.html", HTTP_GET, serveHardwareHtml);
    server->on("/", HTTP_GET, serveIndexHtml);
    server->on("/network", HTTP_GET, serveNetworkHtml);
    server->on("/network.html", HTTP_GET, serveNetworkHtml);
    server->on("/settings", HTTP_GET, serveSettingsHtml);
    server->on("/settings.html", HTTP_GET, serveSettingsHtml);
}
