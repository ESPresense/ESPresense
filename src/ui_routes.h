/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,138 bytes
 * ui_html: 4,534 bytes
 * ui_app_immutable_entry_js: 67,304 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 98,022 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DPaphot8.css", HTTP_GET, serveAppImmutableAssetsInternalDPaphot8Css);
    server->on("/app/immutable/assets/start.DPaphot8.css", HTTP_GET, serveAppImmutableAssetsStartDPaphot8Css);
    server->on("/app/immutable/entry/app.DJ3Age8q.js", HTTP_GET, serveAppImmutableEntryAppDj3Age8qJs);
    server->on("/app/immutable/entry/start.BLXtNkZ1.js", HTTP_GET, serveAppImmutableEntryStartBlXtNkZ1Js);
    server->on("/app/immutable/nodes/0.BXFY0R9H.js", HTTP_GET, serveAppImmutableNodes_0Bxfy0R9HJs);
    server->on("/app/immutable/nodes/1.qDCOvZdP.js", HTTP_GET, serveAppImmutableNodes_1QDcOvZdPJs);
    server->on("/app/immutable/nodes/2.VOVKHOwo.js", HTTP_GET, serveAppImmutableNodes_2VovkhOwoJs);
    server->on("/app/immutable/nodes/3.i_QEse9X.js", HTTP_GET, serveAppImmutableNodes_3IQEse9XJs);
    server->on("/app/immutable/nodes/4.BSoX6pAG.js", HTTP_GET, serveAppImmutableNodes_4BSoX6pAgJs);
    server->on("/app/immutable/nodes/5.9GqusJ4p.js", HTTP_GET, serveAppImmutableNodes_5_9GqusJ4pJs);
    server->on("/app/immutable/nodes/6.CGXoMkeY.js", HTTP_GET, serveAppImmutableNodes_6CgXoMkeYJs);
    server->on("/app/immutable/nodes/7.LQHZq6dB.js", HTTP_GET, serveAppImmutableNodes_7LqhZq6dBJs);
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
