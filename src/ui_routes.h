/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,365 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 50,828 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 65,423 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.T25hC3XV.css", HTTP_GET, serveAppImmutableAssetsInternalT25hC3XvCss);
    server->on("/app/immutable/assets/start.BbZCJWr7.css", HTTP_GET, serveAppImmutableAssetsStartBbZcjWr7Css);
    server->on("/app/immutable/entry/app.BaSQORL5.js", HTTP_GET, serveAppImmutableEntryAppBaSqorl5Js);
    server->on("/app/immutable/entry/start.Dj5225D5.js", HTTP_GET, serveAppImmutableEntryStartDj5225D5Js);
    server->on("/app/immutable/nodes/0.BKQ5ODrP.js", HTTP_GET, serveAppImmutableNodes_0Bkq5ODrPJs);
    server->on("/app/immutable/nodes/1.BOZPPFmQ.js", HTTP_GET, serveAppImmutableNodes_1BozppFmQJs);
    server->on("/app/immutable/nodes/2.pZdBIor1.js", HTTP_GET, serveAppImmutableNodes_2PZdBIor1Js);
    server->on("/app/immutable/nodes/3.fMjpV8-y.js", HTTP_GET, serveAppImmutableNodes_3FMjpV8YJs);
    server->on("/app/immutable/nodes/4.fd2WTT65.js", HTTP_GET, serveAppImmutableNodes_4Fd2Wtt65Js);
    server->on("/app/immutable/nodes/5.DMJfF8dp.js", HTTP_GET, serveAppImmutableNodes_5DmJfF8dpJs);
    server->on("/app/immutable/nodes/6.BAuI7jfu.js", HTTP_GET, serveAppImmutableNodes_6BAuI7jfuJs);
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
