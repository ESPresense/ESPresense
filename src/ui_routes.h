/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,338 bytes
 * ui_app_immutable_entry_js: 63,220 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 93,480 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsInternalDAt95k88Css);
    server->on("/app/immutable/assets/start.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsStartDAt95k88Css);
    server->on("/app/immutable/entry/app.DgF10YYJ.js", HTTP_GET, serveAppImmutableEntryAppDgF10YyjJs);
    server->on("/app/immutable/entry/start.CK_0Vcuo.js", HTTP_GET, serveAppImmutableEntryStartCk_0VcuoJs);
    server->on("/app/immutable/nodes/0.BtNVftvH.js", HTTP_GET, serveAppImmutableNodes_0BtNVftvHJs);
    server->on("/app/immutable/nodes/1.B5XSSpKd.js", HTTP_GET, serveAppImmutableNodes_1B5XsSpKdJs);
    server->on("/app/immutable/nodes/2.Ci72xFx0.js", HTTP_GET, serveAppImmutableNodes_2Ci72xFx0Js);
    server->on("/app/immutable/nodes/3.D-DcWWzt.js", HTTP_GET, serveAppImmutableNodes_3DDcWWztJs);
    server->on("/app/immutable/nodes/4.BlLjcmm6.js", HTTP_GET, serveAppImmutableNodes_4BlLjcmm6Js);
    server->on("/app/immutable/nodes/5.BCnSzR1l.js", HTTP_GET, serveAppImmutableNodes_5BCnSzR1lJs);
    server->on("/app/immutable/nodes/6.BJtlPMYs.js", HTTP_GET, serveAppImmutableNodes_6BJtlPmYsJs);
    server->on("/app/immutable/nodes/7.QByZrWYV.js", HTTP_GET, serveAppImmutableNodes_7QByZrWyvJs);
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
