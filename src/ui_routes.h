/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 23,912 bytes
 * ui_html: 3,906 bytes
 * ui_app_immutable_entry_js: 62,214 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 91,078 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.C5JQPr64.css", HTTP_GET, serveAppImmutableAssetsInternalC5JqPr64Css);
    server->on("/app/immutable/assets/start.C5JQPr64.css", HTTP_GET, serveAppImmutableAssetsStartC5JqPr64Css);
    server->on("/app/immutable/entry/app.Bmz5PfhO.js", HTTP_GET, serveAppImmutableEntryAppBmz5PfhOJs);
    server->on("/app/immutable/entry/start.BNHyY-Pu.js", HTTP_GET, serveAppImmutableEntryStartBnHyYPuJs);
    server->on("/app/immutable/nodes/0.BswgUfS0.js", HTTP_GET, serveAppImmutableNodes_0BswgUfS0Js);
    server->on("/app/immutable/nodes/1.B3lmXa01.js", HTTP_GET, serveAppImmutableNodes_1B3lmXa01Js);
    server->on("/app/immutable/nodes/2.D3W3y4k3.js", HTTP_GET, serveAppImmutableNodes_2D3W3y4k3Js);
    server->on("/app/immutable/nodes/3.L0aZomqz.js", HTTP_GET, serveAppImmutableNodes_3L0aZomqzJs);
    server->on("/app/immutable/nodes/4.Q9HmPWa_.js", HTTP_GET, serveAppImmutableNodes_4Q9HmPWaJs);
    server->on("/app/immutable/nodes/5.-TdfVRC8.js", HTTP_GET, serveAppImmutableNodes_5TdfVrc8Js);
    server->on("/app/immutable/nodes/6.-P2i9R0k.js", HTTP_GET, serveAppImmutableNodes_6P2i9R0kJs);
    server->on("/app/immutable/nodes/7.ByT_POEx.js", HTTP_GET, serveAppImmutableNodes_7ByTPoExJs);
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
