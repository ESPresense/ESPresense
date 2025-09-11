/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 23,962 bytes
 * ui_html: 3,906 bytes
 * ui_app_immutable_entry_js: 62,517 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 91,431 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.C_uWwSbL.css", HTTP_GET, serveAppImmutableAssetsInternalCUWwSbLCss);
    server->on("/app/immutable/assets/start.C_uWwSbL.css", HTTP_GET, serveAppImmutableAssetsStartCUWwSbLCss);
    server->on("/app/immutable/entry/app.PEZOHPY3.js", HTTP_GET, serveAppImmutableEntryAppPezohpy3Js);
    server->on("/app/immutable/entry/start.Bdb8AdAm.js", HTTP_GET, serveAppImmutableEntryStartBdb8AdAmJs);
    server->on("/app/immutable/nodes/0.BNBpbF5w.js", HTTP_GET, serveAppImmutableNodes_0BnBpbF5wJs);
    server->on("/app/immutable/nodes/1.OAvRqjPG.js", HTTP_GET, serveAppImmutableNodes_1OAvRqjPgJs);
    server->on("/app/immutable/nodes/2.CtpvPxNs.js", HTTP_GET, serveAppImmutableNodes_2CtpvPxNsJs);
    server->on("/app/immutable/nodes/3.D2bgXdnf.js", HTTP_GET, serveAppImmutableNodes_3D2bgXdnfJs);
    server->on("/app/immutable/nodes/4.C-P0D2XE.js", HTTP_GET, serveAppImmutableNodes_4CP0D2XeJs);
    server->on("/app/immutable/nodes/5.BgtIooiV.js", HTTP_GET, serveAppImmutableNodes_5BgtIooiVJs);
    server->on("/app/immutable/nodes/6.C96oncxC.js", HTTP_GET, serveAppImmutableNodes_6C96oncxCJs);
    server->on("/app/immutable/nodes/7.Dmz_BkgR.js", HTTP_GET, serveAppImmutableNodes_7DmzBkgRJs);
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
