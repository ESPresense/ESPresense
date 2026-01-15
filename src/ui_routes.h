/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,202 bytes
 * ui_html: 3,888 bytes
 * ui_app_immutable_entry_js: 62,225 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 91,361 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.BRhgnC6i.css", HTTP_GET, serveAppImmutableAssetsInternalBRhgnC6iCss);
    server->on("/app/immutable/assets/start.BRhgnC6i.css", HTTP_GET, serveAppImmutableAssetsStartBRhgnC6iCss);
    server->on("/app/immutable/entry/app.2HsJSDLo.js", HTTP_GET, serveAppImmutableEntryApp_2HsJsdLoJs);
    server->on("/app/immutable/entry/start.BLUoA6T0.js", HTTP_GET, serveAppImmutableEntryStartBlUoA6T0Js);
    server->on("/app/immutable/nodes/0.Bv11qm5U.js", HTTP_GET, serveAppImmutableNodes_0Bv11qm5UJs);
    server->on("/app/immutable/nodes/1.D8gSMgxW.js", HTTP_GET, serveAppImmutableNodes_1D8gSMgxWJs);
    server->on("/app/immutable/nodes/2.DNKsGiAZ.js", HTTP_GET, serveAppImmutableNodes_2DnKsGiAzJs);
    server->on("/app/immutable/nodes/3.CQYpfVDU.js", HTTP_GET, serveAppImmutableNodes_3CqYpfVduJs);
    server->on("/app/immutable/nodes/4.qhcGqqU-.js", HTTP_GET, serveAppImmutableNodes_4QhcGqqUJs);
    server->on("/app/immutable/nodes/5.DKvCm58O.js", HTTP_GET, serveAppImmutableNodes_5DKvCm58OJs);
    server->on("/app/immutable/nodes/6.isHlk7jq.js", HTTP_GET, serveAppImmutableNodes_6IsHlk7jqJs);
    server->on("/app/immutable/nodes/7.DlVH4zZj.js", HTTP_GET, serveAppImmutableNodes_7DlVh4zZjJs);
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
