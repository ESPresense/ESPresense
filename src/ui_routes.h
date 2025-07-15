/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,210 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 45,332 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 59,767 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.R2nWuKlp.css", HTTP_GET, serveAppImmutableAssetsInternalR2nWuKlpCss);
    server->on("/app/immutable/assets/start.CquYjHqP.css", HTTP_GET, serveAppImmutableAssetsStartCquYjHqPCss);
    server->on("/app/immutable/entry/app.Dvt1XATt.js", HTTP_GET, serveAppImmutableEntryAppDvt1XaTtJs);
    server->on("/app/immutable/entry/start.5IQYvLmS.js", HTTP_GET, serveAppImmutableEntryStart_5IqYvLmSJs);
    server->on("/app/immutable/nodes/0.CyLskLnn.js", HTTP_GET, serveAppImmutableNodes_0CyLskLnnJs);
    server->on("/app/immutable/nodes/1.BhIWcBE-.js", HTTP_GET, serveAppImmutableNodes_1BhIWcBeJs);
    server->on("/app/immutable/nodes/2.DLBbxCU6.js", HTTP_GET, serveAppImmutableNodes_2DlBbxCu6Js);
    server->on("/app/immutable/nodes/3.Ba4TeazH.js", HTTP_GET, serveAppImmutableNodes_3Ba4TeazHJs);
    server->on("/app/immutable/nodes/4.BhK6DhYa.js", HTTP_GET, serveAppImmutableNodes_4BhK6DhYaJs);
    server->on("/app/immutable/nodes/5.dVFdPNOJ.js", HTTP_GET, serveAppImmutableNodes_5DVFdPnojJs);
    server->on("/app/immutable/nodes/6.BmNGb9U1.js", HTTP_GET, serveAppImmutableNodes_6BmNGb9U1Js);
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
