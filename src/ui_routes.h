/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,240 bytes
 * ui_app_immutable_entry_js: 44,976 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 58,059 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DN2mWWgh.css", HTTP_GET, serveAppImmutableAssetsInternalDn2mWWghCss);
    server->on("/app/immutable/assets/start.BfkQNPT2.css", HTTP_GET, serveAppImmutableAssetsStartBfkQnpt2Css);
    server->on("/app/immutable/entry/app.9WaD0ma-.js", HTTP_GET, serveAppImmutableEntryApp_9WaD0maJs);
    server->on("/app/immutable/entry/start.BXaYhmmZ.js", HTTP_GET, serveAppImmutableEntryStartBXaYhmmZJs);
    server->on("/app/immutable/nodes/0.B6iRQFrt.js", HTTP_GET, serveAppImmutableNodes_0B6iRqFrtJs);
    server->on("/app/immutable/nodes/1.m-QtdzVK.js", HTTP_GET, serveAppImmutableNodes_1MQtdzVkJs);
    server->on("/app/immutable/nodes/2.CY9FjkaX.js", HTTP_GET, serveAppImmutableNodes_2Cy9FjkaXJs);
    server->on("/app/immutable/nodes/3.9j4DSPn0.js", HTTP_GET, serveAppImmutableNodes_3_9j4DsPn0Js);
    server->on("/app/immutable/nodes/4.D5sg3lil.js", HTTP_GET, serveAppImmutableNodes_4D5sg3lilJs);
    server->on("/app/immutable/nodes/5.BYECyL-6.js", HTTP_GET, serveAppImmutableNodes_5ByeCyL_6Js);
    server->on("/app/immutable/nodes/6.s8jvjww1.js", HTTP_GET, serveAppImmutableNodes_6S8jvjww1Js);
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
