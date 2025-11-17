/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,571 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 66,280 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 81,081 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.wrGhOo2C.css", HTTP_GET, serveAppImmutableAssetsInternalWrGhOo2CCss);
    server->on("/app/immutable/assets/start.DOTUP31V.css", HTTP_GET, serveAppImmutableAssetsStartDotup31VCss);
    server->on("/app/immutable/entry/app.Ij0llWWd.js", HTTP_GET, serveAppImmutableEntryAppIj0llWWdJs);
    server->on("/app/immutable/entry/start.CNNB7ojd.js", HTTP_GET, serveAppImmutableEntryStartCnnb7ojdJs);
    server->on("/app/immutable/nodes/0.CGHcdT6q.js", HTTP_GET, serveAppImmutableNodes_0CgHcdT6qJs);
    server->on("/app/immutable/nodes/1.DxdBPCLD.js", HTTP_GET, serveAppImmutableNodes_1DxdBpcldJs);
    server->on("/app/immutable/nodes/2.CWctgRpg.js", HTTP_GET, serveAppImmutableNodes_2CWctgRpgJs);
    server->on("/app/immutable/nodes/3.KWB9pl8-.js", HTTP_GET, serveAppImmutableNodes_3Kwb9pl8Js);
    server->on("/app/immutable/nodes/4.B39mllgl.js", HTTP_GET, serveAppImmutableNodes_4B39mllglJs);
    server->on("/app/immutable/nodes/5.B7392MfA.js", HTTP_GET, serveAppImmutableNodes_5B7392MfAJs);
    server->on("/app/immutable/nodes/6.DjwXaxDn.js", HTTP_GET, serveAppImmutableNodes_6DjwXaxDnJs);
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
