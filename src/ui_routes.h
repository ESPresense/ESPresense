/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,338 bytes
 * ui_app_immutable_entry_js: 63,296 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 93,556 bytes
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
    server->on("/app/immutable/entry/app.6GVvG8BH.js", HTTP_GET, serveAppImmutableEntryApp_6GVvG8BhJs);
    server->on("/app/immutable/entry/start.KIl7JDnC.js", HTTP_GET, serveAppImmutableEntryStartKIl7JDnCJs);
    server->on("/app/immutable/nodes/0.pExWIPt4.js", HTTP_GET, serveAppImmutableNodes_0PExWiPt4Js);
    server->on("/app/immutable/nodes/1.DS3i2aNl.js", HTTP_GET, serveAppImmutableNodes_1Ds3i2aNlJs);
    server->on("/app/immutable/nodes/2.BHG5Y9P7.js", HTTP_GET, serveAppImmutableNodes_2Bhg5Y9P7Js);
    server->on("/app/immutable/nodes/3.C6y7cz89.js", HTTP_GET, serveAppImmutableNodes_3C6y7cz89Js);
    server->on("/app/immutable/nodes/4.BK8t1WEZ.js", HTTP_GET, serveAppImmutableNodes_4Bk8t1WezJs);
    server->on("/app/immutable/nodes/5.BC5_9S2Z.js", HTTP_GET, serveAppImmutableNodes_5Bc5_9S2ZJs);
    server->on("/app/immutable/nodes/6.BDBOBOl_.js", HTTP_GET, serveAppImmutableNodes_6BdbobOlJs);
    server->on("/app/immutable/nodes/7.BgEnc5TZ.js", HTTP_GET, serveAppImmutableNodes_7BgEnc5TzJs);
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
