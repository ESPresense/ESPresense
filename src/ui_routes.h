/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,522 bytes
 * ui_html: 3,912 bytes
 * ui_app_immutable_entry_js: 62,532 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 92,012 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.Bb3R8x5k.css", HTTP_GET, serveAppImmutableAssetsInternalBb3R8x5kCss);
    server->on("/app/immutable/assets/start.Bb3R8x5k.css", HTTP_GET, serveAppImmutableAssetsStartBb3R8x5kCss);
    server->on("/app/immutable/entry/app.BrcT2R1u.js", HTTP_GET, serveAppImmutableEntryAppBrcT2R1uJs);
    server->on("/app/immutable/entry/start.Cu3XXSjz.js", HTTP_GET, serveAppImmutableEntryStartCu3XxSjzJs);
    server->on("/app/immutable/nodes/0.BxvnnIae.js", HTTP_GET, serveAppImmutableNodes_0BxvnnIaeJs);
    server->on("/app/immutable/nodes/1.CaWbwXsT.js", HTTP_GET, serveAppImmutableNodes_1CaWbwXsTJs);
    server->on("/app/immutable/nodes/2.Cpx9vJMx.js", HTTP_GET, serveAppImmutableNodes_2Cpx9vJMxJs);
    server->on("/app/immutable/nodes/3.0KCQHlvp.js", HTTP_GET, serveAppImmutableNodes_3_0KcqHlvpJs);
    server->on("/app/immutable/nodes/4.CjGsfKin.js", HTTP_GET, serveAppImmutableNodes_4CjGsfKinJs);
    server->on("/app/immutable/nodes/5.vhllqgmt.js", HTTP_GET, serveAppImmutableNodes_5VhllqgmtJs);
    server->on("/app/immutable/nodes/6.735Ezuob.js", HTTP_GET, serveAppImmutableNodes_6_735EzuobJs);
    server->on("/app/immutable/nodes/7.DHOBoYMI.js", HTTP_GET, serveAppImmutableNodes_7DhoBoYmiJs);
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
