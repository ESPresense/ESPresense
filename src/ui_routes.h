/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 23,138 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 63,841 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 91,209 bytes
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
    server->on("/app/immutable/entry/app.B50cMbwu.js", HTTP_GET, serveAppImmutableEntryAppB50cMbwuJs);
    server->on("/app/immutable/entry/start.DAMOcvUl.js", HTTP_GET, serveAppImmutableEntryStartDamOcvUlJs);
    server->on("/app/immutable/nodes/0.D2_gC_3V.js", HTTP_GET, serveAppImmutableNodes_0D2GC_3VJs);
    server->on("/app/immutable/nodes/1.D1Xi7aY1.js", HTTP_GET, serveAppImmutableNodes_1D1Xi7aY1Js);
    server->on("/app/immutable/nodes/2.BtfBVX90.js", HTTP_GET, serveAppImmutableNodes_2BtfBvx90Js);
    server->on("/app/immutable/nodes/3.BlgqN0KW.js", HTTP_GET, serveAppImmutableNodes_3BlgqN0KwJs);
    server->on("/app/immutable/nodes/4.vW0wFG9Q.js", HTTP_GET, serveAppImmutableNodes_4VW0wFg9QJs);
    server->on("/app/immutable/nodes/5.BGFSkLdu.js", HTTP_GET, serveAppImmutableNodes_5BgfSkLduJs);
    server->on("/app/immutable/nodes/6.Bjh8wetL.js", HTTP_GET, serveAppImmutableNodes_6Bjh8wetLJs);
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
