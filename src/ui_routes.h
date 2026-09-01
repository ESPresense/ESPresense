/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 14,520 bytes
 * ui_app_immutable_chunks_js: 66,524 bytes
 * ui_app_immutable_entry_js: 268 bytes
 * ui_app_immutable_nodes_js: 542 bytes
 * ui_html: 4,644 bytes
 * ui_svg: 456 bytes
 * Total: 86,954 bytes
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
    server->on("/app/immutable/chunks/m64qQlfo.js", HTTP_GET, serveAppImmutableChunksM64qQlfoJs);
    server->on("/app/immutable/entry/app.D1h1UQku.js", HTTP_GET, serveAppImmutableEntryAppD1h1UQkuJs);
    server->on("/app/immutable/entry/start.Co80w-rq.js", HTTP_GET, serveAppImmutableEntryStartCo80wRqJs);
    server->on("/app/immutable/nodes/0.DCPe1RfW.js", HTTP_GET, serveAppImmutableNodes_0DcPe1RfWJs);
    server->on("/app/immutable/nodes/1.zeFQ2f15.js", HTTP_GET, serveAppImmutableNodes_1ZeFq2f15Js);
    server->on("/app/immutable/nodes/2.B6tIo_Wz.js", HTTP_GET, serveAppImmutableNodes_2B6tIoWzJs);
    server->on("/app/immutable/nodes/3.QzzF1NBC.js", HTTP_GET, serveAppImmutableNodes_3QzzF1NbcJs);
    server->on("/app/immutable/nodes/4.CQeYeU6E.js", HTTP_GET, serveAppImmutableNodes_4CQeYeU6EJs);
    server->on("/app/immutable/nodes/5.CfvQBzbd.js", HTTP_GET, serveAppImmutableNodes_5CfvQBzbdJs);
    server->on("/app/immutable/nodes/6.BT2qhynk.js", HTTP_GET, serveAppImmutableNodes_6Bt2qhynkJs);
    server->on("/app/immutable/nodes/7.C25vf_al.js", HTTP_GET, serveAppImmutableNodes_7C25vfAlJs);
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
