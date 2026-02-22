/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,392 bytes
 * ui_app_immutable_entry_js: 63,226 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 93,540 bytes
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
    server->on("/app/immutable/entry/app.DGcOYku4.js", HTTP_GET, serveAppImmutableEntryAppDGcOYku4Js);
    server->on("/app/immutable/entry/start.CLec45gx.js", HTTP_GET, serveAppImmutableEntryStartCLec45gxJs);
    server->on("/app/immutable/nodes/0.DT2rLRSL.js", HTTP_GET, serveAppImmutableNodes_0Dt2rLrslJs);
    server->on("/app/immutable/nodes/1.CogiXMXO.js", HTTP_GET, serveAppImmutableNodes_1CogiXmxoJs);
    server->on("/app/immutable/nodes/2.BCEFzJ1H.js", HTTP_GET, serveAppImmutableNodes_2BceFzJ1HJs);
    server->on("/app/immutable/nodes/3.CNM7Fuxf.js", HTTP_GET, serveAppImmutableNodes_3Cnm7FuxfJs);
    server->on("/app/immutable/nodes/4.QCPOD7Jq.js", HTTP_GET, serveAppImmutableNodes_4Qcpod7JqJs);
    server->on("/app/immutable/nodes/5.C6qlVqzi.js", HTTP_GET, serveAppImmutableNodes_5C6qlVqziJs);
    server->on("/app/immutable/nodes/6.B1rRy9dn.js", HTTP_GET, serveAppImmutableNodes_6B1rRy9dnJs);
    server->on("/app/immutable/nodes/7.C8ijCeWt.js", HTTP_GET, serveAppImmutableNodes_7C8ijCeWtJs);
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
