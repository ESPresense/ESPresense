/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,882 bytes
 * ui_html: 4,338 bytes
 * ui_app_immutable_entry_js: 63,226 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 93,492 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.I548-e4U.css", HTTP_GET, serveAppImmutableAssetsInternalI548E4UCss);
    server->on("/app/immutable/assets/start.I548-e4U.css", HTTP_GET, serveAppImmutableAssetsStartI548E4UCss);
    server->on("/app/immutable/entry/app.BZBO0Eu1.js", HTTP_GET, serveAppImmutableEntryAppBzbo0Eu1Js);
    server->on("/app/immutable/entry/start.C-m2GPk7.js", HTTP_GET, serveAppImmutableEntryStartCM2GPk7Js);
    server->on("/app/immutable/nodes/0.wZixLjGF.js", HTTP_GET, serveAppImmutableNodes_0WZixLjGfJs);
    server->on("/app/immutable/nodes/1.jHHRDPMU.js", HTTP_GET, serveAppImmutableNodes_1JHhrdpmuJs);
    server->on("/app/immutable/nodes/2.DKOS2kvN.js", HTTP_GET, serveAppImmutableNodes_2Dkos2kvNJs);
    server->on("/app/immutable/nodes/3.d1jAQBtw.js", HTTP_GET, serveAppImmutableNodes_3D1jAqBtwJs);
    server->on("/app/immutable/nodes/4.B0YL6lQK.js", HTTP_GET, serveAppImmutableNodes_4B0Yl6lQkJs);
    server->on("/app/immutable/nodes/5.DvflHD9u.js", HTTP_GET, serveAppImmutableNodes_5DvflHd9uJs);
    server->on("/app/immutable/nodes/6.CqohWYOd.js", HTTP_GET, serveAppImmutableNodes_6CqohWyOdJs);
    server->on("/app/immutable/nodes/7.DUq7mEEW.js", HTTP_GET, serveAppImmutableNodes_7DUq7mEewJs);
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
