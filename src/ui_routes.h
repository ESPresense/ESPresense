/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,148 bytes
 * ui_html: 4,537 bytes
 * ui_app_immutable_entry_js: 67,277 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 98,008 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DJlzcx4I.css", HTTP_GET, serveAppImmutableAssetsInternalDJlzcx4ICss);
    server->on("/app/immutable/assets/start.DJlzcx4I.css", HTTP_GET, serveAppImmutableAssetsStartDJlzcx4ICss);
    server->on("/app/immutable/entry/app.CMBZZYYD.js", HTTP_GET, serveAppImmutableEntryAppCmbzzyydJs);
    server->on("/app/immutable/entry/start.RWAh_bUd.js", HTTP_GET, serveAppImmutableEntryStartRwAhBUdJs);
    server->on("/app/immutable/nodes/0.SKKkmR09.js", HTTP_GET, serveAppImmutableNodes_0SkKkmR09Js);
    server->on("/app/immutable/nodes/1.D1LtaxYW.js", HTTP_GET, serveAppImmutableNodes_1D1LtaxYwJs);
    server->on("/app/immutable/nodes/2.yjSDJQ6b.js", HTTP_GET, serveAppImmutableNodes_2YjSdjq6bJs);
    server->on("/app/immutable/nodes/3.Bliub1ZQ.js", HTTP_GET, serveAppImmutableNodes_3Bliub1ZqJs);
    server->on("/app/immutable/nodes/4.CtYhgZr5.js", HTTP_GET, serveAppImmutableNodes_4CtYhgZr5Js);
    server->on("/app/immutable/nodes/5.Ba-9AqA0.js", HTTP_GET, serveAppImmutableNodes_5Ba_9AqA0Js);
    server->on("/app/immutable/nodes/6.CYfzM-Ms.js", HTTP_GET, serveAppImmutableNodes_6CYfzMMsJs);
    server->on("/app/immutable/nodes/7.XJCqox-N.js", HTTP_GET, serveAppImmutableNodes_7XjCqoxNJs);
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
