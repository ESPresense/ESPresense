/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,246 bytes
 * ui_html: 4,338 bytes
 * ui_app_immutable_entry_js: 64,556 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 95,186 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DxBQeS86.css", HTTP_GET, serveAppImmutableAssetsInternalDxBQeS86Css);
    server->on("/app/immutable/assets/start.DxBQeS86.css", HTTP_GET, serveAppImmutableAssetsStartDxBQeS86Css);
    server->on("/app/immutable/entry/app.CbXm0tVA.js", HTTP_GET, serveAppImmutableEntryAppCbXm0tVaJs);
    server->on("/app/immutable/entry/start.Cd6WA20S.js", HTTP_GET, serveAppImmutableEntryStartCd6Wa20SJs);
    server->on("/app/immutable/nodes/0.BfMPP2Gk.js", HTTP_GET, serveAppImmutableNodes_0BfMpp2GkJs);
    server->on("/app/immutable/nodes/1.CTUlvMPQ.js", HTTP_GET, serveAppImmutableNodes_1CtUlvMpqJs);
    server->on("/app/immutable/nodes/2.QGB1Fs3k.js", HTTP_GET, serveAppImmutableNodes_2Qgb1Fs3kJs);
    server->on("/app/immutable/nodes/3.DEW_23-Q.js", HTTP_GET, serveAppImmutableNodes_3Dew_23QJs);
    server->on("/app/immutable/nodes/4.DLQuOMIC.js", HTTP_GET, serveAppImmutableNodes_4DlQuOmicJs);
    server->on("/app/immutable/nodes/5.BThL0u0_.js", HTTP_GET, serveAppImmutableNodes_5BThL0u0Js);
    server->on("/app/immutable/nodes/6.BmomsKpm.js", HTTP_GET, serveAppImmutableNodes_6BmomsKpmJs);
    server->on("/app/immutable/nodes/7.dwKgBvJx.js", HTTP_GET, serveAppImmutableNodes_7DwKgBvJxJs);
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
