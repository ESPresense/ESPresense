/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,544 bytes
 * ui_html: 3,894 bytes
 * ui_app_immutable_entry_js: 62,608 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 92,092 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CFFWtPn5.css", HTTP_GET, serveAppImmutableAssetsInternalCffWtPn5Css);
    server->on("/app/immutable/assets/start.CFFWtPn5.css", HTTP_GET, serveAppImmutableAssetsStartCffWtPn5Css);
    server->on("/app/immutable/entry/app.h9UaVWPb.js", HTTP_GET, serveAppImmutableEntryAppH9UaVwPbJs);
    server->on("/app/immutable/entry/start.Bml3zW7y.js", HTTP_GET, serveAppImmutableEntryStartBml3zW7yJs);
    server->on("/app/immutable/nodes/0.gnIS5D9_.js", HTTP_GET, serveAppImmutableNodes_0GnIs5D9Js);
    server->on("/app/immutable/nodes/1.UUCR310a.js", HTTP_GET, serveAppImmutableNodes_1Uucr310aJs);
    server->on("/app/immutable/nodes/2.CCgLsQ9N.js", HTTP_GET, serveAppImmutableNodes_2CCgLsQ9NJs);
    server->on("/app/immutable/nodes/3.CGga4GDW.js", HTTP_GET, serveAppImmutableNodes_3CGga4GdwJs);
    server->on("/app/immutable/nodes/4.BNqxaiy6.js", HTTP_GET, serveAppImmutableNodes_4BNqxaiy6Js);
    server->on("/app/immutable/nodes/5.BEbENJQM.js", HTTP_GET, serveAppImmutableNodes_5BEbEnjqmJs);
    server->on("/app/immutable/nodes/6.Dav7kln3.js", HTTP_GET, serveAppImmutableNodes_6Dav7kln3Js);
    server->on("/app/immutable/nodes/7.BNDph4bk.js", HTTP_GET, serveAppImmutableNodes_7BnDph4bkJs);
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
