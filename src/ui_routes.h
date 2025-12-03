/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,691 bytes
 * ui_html: 3,900 bytes
 * ui_app_immutable_entry_js: 64,630 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 80,267 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DV06I2rx.css", HTTP_GET, serveAppImmutableAssetsInternalDv06I2rxCss);
    server->on("/app/immutable/assets/start.DqUW1J3d.css", HTTP_GET, serveAppImmutableAssetsStartDqUw1J3dCss);
    server->on("/app/immutable/entry/app.D7o-QQST.js", HTTP_GET, serveAppImmutableEntryAppD7oQqstJs);
    server->on("/app/immutable/entry/start.DQLu-_qb.js", HTTP_GET, serveAppImmutableEntryStartDqLuQbJs);
    server->on("/app/immutable/nodes/0.Cv51rvyU.js", HTTP_GET, serveAppImmutableNodes_0Cv51rvyUJs);
    server->on("/app/immutable/nodes/1.Cv5mTDzQ.js", HTTP_GET, serveAppImmutableNodes_1Cv5mTDzQJs);
    server->on("/app/immutable/nodes/2.D8XbL09I.js", HTTP_GET, serveAppImmutableNodes_2D8XbL09IJs);
    server->on("/app/immutable/nodes/3.BQqtlG9q.js", HTTP_GET, serveAppImmutableNodes_3BQqtlG9qJs);
    server->on("/app/immutable/nodes/4.CeJoQC3U.js", HTTP_GET, serveAppImmutableNodes_4CeJoQc3UJs);
    server->on("/app/immutable/nodes/5.BuhmY0gp.js", HTTP_GET, serveAppImmutableNodes_5BuhmY0gpJs);
    server->on("/app/immutable/nodes/6.DcFKUu3F.js", HTTP_GET, serveAppImmutableNodes_6DcFkUu3FJs);
    server->on("/app/immutable/nodes/7.D4QlTGXt.js", HTTP_GET, serveAppImmutableNodes_7D4QlTgXtJs);
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
