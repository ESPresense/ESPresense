/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 14,520 bytes
 * ui_app_immutable_chunks_js: 65,979 bytes
 * ui_app_immutable_entry_js: 271 bytes
 * ui_app_immutable_nodes_js: 542 bytes
 * ui_html: 4,646 bytes
 * ui_svg: 456 bytes
 * Total: 86,414 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_app_immutable_chunks_js.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_html.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/index.DKQns9on.css", HTTP_GET, serveAppImmutableAssetsIndexDkQns9onCss);
    server->on("/app/immutable/chunks/o9YD4YNW.js", HTTP_GET, serveAppImmutableChunksO9Yd4YnwJs);
    server->on("/app/immutable/entry/app.uVjG-hNS.js", HTTP_GET, serveAppImmutableEntryAppUVjGHNsJs);
    server->on("/app/immutable/entry/start.DeWkjCvY.js", HTTP_GET, serveAppImmutableEntryStartDeWkjCvYJs);
    server->on("/app/immutable/nodes/0.DWuwOXWR.js", HTTP_GET, serveAppImmutableNodes_0DWuwOxwrJs);
    server->on("/app/immutable/nodes/1.CSNwa8u7.js", HTTP_GET, serveAppImmutableNodes_1CsNwa8u7Js);
    server->on("/app/immutable/nodes/2.DDvk0RNv.js", HTTP_GET, serveAppImmutableNodes_2DDvk0RNvJs);
    server->on("/app/immutable/nodes/3.CtYBpJbM.js", HTTP_GET, serveAppImmutableNodes_3CtYBpJbMJs);
    server->on("/app/immutable/nodes/4.B3E1r4Cf.js", HTTP_GET, serveAppImmutableNodes_4B3E1r4CfJs);
    server->on("/app/immutable/nodes/5.Bzxbedlv.js", HTTP_GET, serveAppImmutableNodes_5BzxbedlvJs);
    server->on("/app/immutable/nodes/6.BkVf2kGu.js", HTTP_GET, serveAppImmutableNodes_6BkVf2kGuJs);
    server->on("/app/immutable/nodes/7.By0TK9L6.js", HTTP_GET, serveAppImmutableNodes_7By0Tk9L6Js);
    server->on("/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes
    server->on("/", HTTP_GET, serveIndexHtml);
    server->on("/devices", HTTP_GET, serveDevicesHtml);
    server->on("/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/hardware", HTTP_GET, serveHardwareHtml);
    server->on("/hardware.html", HTTP_GET, serveHardwareHtml);
    server->on("/network", HTTP_GET, serveNetworkHtml);
    server->on("/network.html", HTTP_GET, serveNetworkHtml);
    server->on("/settings", HTTP_GET, serveSettingsHtml);
    server->on("/settings.html", HTTP_GET, serveSettingsHtml);
}
