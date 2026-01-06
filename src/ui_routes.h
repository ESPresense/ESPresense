/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,202 bytes
 * ui_html: 3,906 bytes
 * ui_app_immutable_entry_js: 62,519 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 91,673 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.BRhgnC6i.css", HTTP_GET, serveAppImmutableAssetsInternalBRhgnC6iCss);
    server->on("/app/immutable/assets/start.BRhgnC6i.css", HTTP_GET, serveAppImmutableAssetsStartBRhgnC6iCss);
    server->on("/app/immutable/entry/app.qZk1UjAt.js", HTTP_GET, serveAppImmutableEntryAppQZk1UjAtJs);
    server->on("/app/immutable/entry/start.D5k5EfVl.js", HTTP_GET, serveAppImmutableEntryStartD5k5EfVlJs);
    server->on("/app/immutable/nodes/0.DslRPEbc.js", HTTP_GET, serveAppImmutableNodes_0DslRpEbcJs);
    server->on("/app/immutable/nodes/1.D5na268C.js", HTTP_GET, serveAppImmutableNodes_1D5na268CJs);
    server->on("/app/immutable/nodes/2.C5omFhL6.js", HTTP_GET, serveAppImmutableNodes_2C5omFhL6Js);
    server->on("/app/immutable/nodes/3.DdXev1Xh.js", HTTP_GET, serveAppImmutableNodes_3DdXev1XhJs);
    server->on("/app/immutable/nodes/4.BiQkaOfz.js", HTTP_GET, serveAppImmutableNodes_4BiQkaOfzJs);
    server->on("/app/immutable/nodes/5.tbH9WdYE.js", HTTP_GET, serveAppImmutableNodes_5TbH9WdYeJs);
    server->on("/app/immutable/nodes/6.lGq4LlA7.js", HTTP_GET, serveAppImmutableNodes_6LGq4LlA7Js);
    server->on("/app/immutable/nodes/7.DMo_JEyN.js", HTTP_GET, serveAppImmutableNodes_7DMoJEyNJs);
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
