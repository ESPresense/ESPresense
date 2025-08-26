/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 12,128 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 51,342 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 67,685 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.os1IhCiP.css", HTTP_GET, serveAppImmutableAssetsInternalOs1IhCiPCss);
    server->on("/app/immutable/assets/start.DHbawTJH.css", HTTP_GET, serveAppImmutableAssetsStartDHbawTjhCss);
    server->on("/app/immutable/entry/app.BEr8i9np.js", HTTP_GET, serveAppImmutableEntryAppBEr8i9npJs);
    server->on("/app/immutable/entry/start.DWXsAVjM.js", HTTP_GET, serveAppImmutableEntryStartDwXsAVjMJs);
    server->on("/app/immutable/nodes/0.myxnCFI-.js", HTTP_GET, serveAppImmutableNodes_0MyxnCfiJs);
    server->on("/app/immutable/nodes/1.DMb5ZH7y.js", HTTP_GET, serveAppImmutableNodes_1DMb5Zh7yJs);
    server->on("/app/immutable/nodes/2.gE7IbMVR.js", HTTP_GET, serveAppImmutableNodes_2GE7IbMvrJs);
    server->on("/app/immutable/nodes/3.CIHBZPvI.js", HTTP_GET, serveAppImmutableNodes_3CihbzPvIJs);
    server->on("/app/immutable/nodes/4.Bcw8kLxz.js", HTTP_GET, serveAppImmutableNodes_4Bcw8kLxzJs);
    server->on("/app/immutable/nodes/5.BHvAPXRo.js", HTTP_GET, serveAppImmutableNodes_5BHvApxRoJs);
    server->on("/app/immutable/nodes/6.BnEFbnpu.js", HTTP_GET, serveAppImmutableNodes_6BnEFbnpuJs);
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
