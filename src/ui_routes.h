/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,148 bytes
 * ui_html: 4,531 bytes
 * ui_app_immutable_entry_js: 67,330 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 98,055 bytes
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
    server->on("/app/immutable/entry/app.B40BGFse.js", HTTP_GET, serveAppImmutableEntryAppB40BgFseJs);
    server->on("/app/immutable/entry/start.4E9w6-np.js", HTTP_GET, serveAppImmutableEntryStart_4E9w6NpJs);
    server->on("/app/immutable/nodes/0.BIVp4ROW.js", HTTP_GET, serveAppImmutableNodes_0BiVp4RowJs);
    server->on("/app/immutable/nodes/1.Heivqskp.js", HTTP_GET, serveAppImmutableNodes_1HeivqskpJs);
    server->on("/app/immutable/nodes/2.CqmZxPOl.js", HTTP_GET, serveAppImmutableNodes_2CqmZxPOlJs);
    server->on("/app/immutable/nodes/3.C6N-TYNv.js", HTTP_GET, serveAppImmutableNodes_3C6NTyNvJs);
    server->on("/app/immutable/nodes/4.bSXctdvo.js", HTTP_GET, serveAppImmutableNodes_4BSXctdvoJs);
    server->on("/app/immutable/nodes/5.BIJeUjX-.js", HTTP_GET, serveAppImmutableNodes_5BiJeUjXJs);
    server->on("/app/immutable/nodes/6.Dh_XBW0F.js", HTTP_GET, serveAppImmutableNodes_6DhXbw0FJs);
    server->on("/app/immutable/nodes/7.IOk7sQpP.js", HTTP_GET, serveAppImmutableNodes_7IOk7sQpPJs);
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
