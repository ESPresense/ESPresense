/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 45,063 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 58,156 bytes
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
    server->on("/app/immutable/entry/app.C3iYWUFS.js", HTTP_GET, serveAppImmutableEntryAppC3iYwufsJs);
    server->on("/app/immutable/entry/start.DHa3VzfB.js", HTTP_GET, serveAppImmutableEntryStartDHa3VzfBJs);
    server->on("/app/immutable/nodes/0.Zq-ZQzm6.js", HTTP_GET, serveAppImmutableNodes_0ZqZQzm6Js);
    server->on("/app/immutable/nodes/1.DluJr3Vb.js", HTTP_GET, serveAppImmutableNodes_1DluJr3VbJs);
    server->on("/app/immutable/nodes/2.DCxcsCPG.js", HTTP_GET, serveAppImmutableNodes_2DCxcsCpgJs);
    server->on("/app/immutable/nodes/3.9YEmamKg.js", HTTP_GET, serveAppImmutableNodes_3_9YEmamKgJs);
    server->on("/app/immutable/nodes/4.CvSetHOk.js", HTTP_GET, serveAppImmutableNodes_4CvSetHOkJs);
    server->on("/app/immutable/nodes/5.DK_QoN8c.js", HTTP_GET, serveAppImmutableNodes_5DkQoN8cJs);
    server->on("/app/immutable/nodes/6.D7SIbWo5.js", HTTP_GET, serveAppImmutableNodes_6D7SIbWo5Js);
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
