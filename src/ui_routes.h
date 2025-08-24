/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,486 bytes
 * ui_html: 3,906 bytes
 * ui_app_immutable_entry_js: 65,308 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,746 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.C5UPK447.css", HTTP_GET, serveAppImmutableAssetsInternalC5Upk447Css);
    server->on("/app/immutable/assets/start.C5UPK447.css", HTTP_GET, serveAppImmutableAssetsStartC5Upk447Css);
    server->on("/app/immutable/entry/app.CJWtIpYA.js", HTTP_GET, serveAppImmutableEntryAppCjWtIpYaJs);
    server->on("/app/immutable/entry/start.CCn8J75H.js", HTTP_GET, serveAppImmutableEntryStartCCn8J75HJs);
    server->on("/app/immutable/nodes/0.BjolEIyE.js", HTTP_GET, serveAppImmutableNodes_0BjolEIyEJs);
    server->on("/app/immutable/nodes/1.DCRpMsor.js", HTTP_GET, serveAppImmutableNodes_1DcRpMsorJs);
    server->on("/app/immutable/nodes/2.dvGXlu5P.js", HTTP_GET, serveAppImmutableNodes_2DvGXlu5PJs);
    server->on("/app/immutable/nodes/3.CpF5iVrC.js", HTTP_GET, serveAppImmutableNodes_3CpF5iVrCJs);
    server->on("/app/immutable/nodes/4.D-OcbNEQ.js", HTTP_GET, serveAppImmutableNodes_4DOcbNeqJs);
    server->on("/app/immutable/nodes/5.DzIPI5FM.js", HTTP_GET, serveAppImmutableNodes_5DzIpi5FmJs);
    server->on("/app/immutable/nodes/6.2Lrmp0zk.js", HTTP_GET, serveAppImmutableNodes_6_2Lrmp0zkJs);
    server->on("/app/immutable/nodes/7.BsOnzn1E.js", HTTP_GET, serveAppImmutableNodes_7BsOnzn1EJs);
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
