/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,338 bytes
 * ui_app_immutable_entry_js: 63,724 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 93,984 bytes
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
    server->on("/app/immutable/entry/app.BmlCZz0L.js", HTTP_GET, serveAppImmutableEntryAppBmlCZz0LJs);
    server->on("/app/immutable/entry/start._9qVv_Wv.js", HTTP_GET, serveAppImmutableEntryStart_9qVvWvJs);
    server->on("/app/immutable/nodes/0.D57sJuxZ.js", HTTP_GET, serveAppImmutableNodes_0D57sJuxZJs);
    server->on("/app/immutable/nodes/1.VJxFHKvU.js", HTTP_GET, serveAppImmutableNodes_1VJxFhKvUJs);
    server->on("/app/immutable/nodes/2.C0LcSLON.js", HTTP_GET, serveAppImmutableNodes_2C0LcSlonJs);
    server->on("/app/immutable/nodes/3.BPqSbsij.js", HTTP_GET, serveAppImmutableNodes_3BPqSbsijJs);
    server->on("/app/immutable/nodes/4.1WA-KDg1.js", HTTP_GET, serveAppImmutableNodes_4_1WaKDg1Js);
    server->on("/app/immutable/nodes/5.D_LW7Akd.js", HTTP_GET, serveAppImmutableNodes_5DLw7AkdJs);
    server->on("/app/immutable/nodes/6.D-B4O61j.js", HTTP_GET, serveAppImmutableNodes_6DB4O61jJs);
    server->on("/app/immutable/nodes/7.dr_cVsfj.js", HTTP_GET, serveAppImmutableNodes_7DrCVsfjJs);
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
