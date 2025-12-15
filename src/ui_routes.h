/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 23,138 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 63,828 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 91,201 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CEXAPnMf.css", HTTP_GET, serveAppImmutableAssetsInternalCexaPnMfCss);
    server->on("/app/immutable/assets/start.CEXAPnMf.css", HTTP_GET, serveAppImmutableAssetsStartCexaPnMfCss);
    server->on("/app/immutable/entry/app.CpaM7WmI.js", HTTP_GET, serveAppImmutableEntryAppCpaM7WmIJs);
    server->on("/app/immutable/entry/start.BRKQVfD4.js", HTTP_GET, serveAppImmutableEntryStartBrkqVfD4Js);
    server->on("/app/immutable/nodes/0.0Eubg4Re.js", HTTP_GET, serveAppImmutableNodes_0_0Eubg4ReJs);
    server->on("/app/immutable/nodes/1.DpzfoEyn.js", HTTP_GET, serveAppImmutableNodes_1DpzfoEynJs);
    server->on("/app/immutable/nodes/2.p1jjqT6R.js", HTTP_GET, serveAppImmutableNodes_2P1jjqT6RJs);
    server->on("/app/immutable/nodes/3._yYfp0dw.js", HTTP_GET, serveAppImmutableNodes_3YYfp0dwJs);
    server->on("/app/immutable/nodes/4.DKFLjjno.js", HTTP_GET, serveAppImmutableNodes_4DkfLjjnoJs);
    server->on("/app/immutable/nodes/5.DJD-OfaK.js", HTTP_GET, serveAppImmutableNodes_5DjdOfaKJs);
    server->on("/app/immutable/nodes/6.DwG_lgki.js", HTTP_GET, serveAppImmutableNodes_6DwGLgkiJs);
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
