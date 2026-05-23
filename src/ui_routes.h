/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,144 bytes
 * ui_html: 4,326 bytes
 * ui_app_immutable_entry_js: 67,092 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 97,608 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.BieM-44V.css", HTTP_GET, serveAppImmutableAssetsInternalBieM_44VCss);
    server->on("/app/immutable/assets/start.BieM-44V.css", HTTP_GET, serveAppImmutableAssetsStartBieM_44VCss);
    server->on("/app/immutable/entry/app.BI0yzuEI.js", HTTP_GET, serveAppImmutableEntryAppBi0yzuEiJs);
    server->on("/app/immutable/entry/start.AMbuDRM-.js", HTTP_GET, serveAppImmutableEntryStartAMbuDrmJs);
    server->on("/app/immutable/nodes/0.BQ6f94XE.js", HTTP_GET, serveAppImmutableNodes_0Bq6f94XeJs);
    server->on("/app/immutable/nodes/1.C18990tW.js", HTTP_GET, serveAppImmutableNodes_1C18990tWJs);
    server->on("/app/immutable/nodes/2.BUd1ACDF.js", HTTP_GET, serveAppImmutableNodes_2BUd1AcdfJs);
    server->on("/app/immutable/nodes/3.CJ6YAFOd.js", HTTP_GET, serveAppImmutableNodes_3Cj6YafOdJs);
    server->on("/app/immutable/nodes/4.DU7jKFKz.js", HTTP_GET, serveAppImmutableNodes_4Du7jKfKzJs);
    server->on("/app/immutable/nodes/5.x8kA3M8x.js", HTTP_GET, serveAppImmutableNodes_5X8kA3M8xJs);
    server->on("/app/immutable/nodes/6.rCnLjCA1.js", HTTP_GET, serveAppImmutableNodes_6RCnLjCa1Js);
    server->on("/app/immutable/nodes/7.D6bETxM7.js", HTTP_GET, serveAppImmutableNodes_7D6bETxM7Js);
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
