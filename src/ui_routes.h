/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 11,189 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 45,153 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 60,567 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.D4hCBGO1.css", HTTP_GET, serveAppImmutableAssetsInternalD4hCbgo1Css);
    server->on("/app/immutable/assets/start.C5CHO1jv.css", HTTP_GET, serveAppImmutableAssetsStartC5Cho1jvCss);
    server->on("/app/immutable/entry/app.DYJpDPef.js", HTTP_GET, serveAppImmutableEntryAppDyJpDPefJs);
    server->on("/app/immutable/entry/start.COrEFgT_.js", HTTP_GET, serveAppImmutableEntryStartCOrEFgTJs);
    server->on("/app/immutable/nodes/0.BWMwsby6.js", HTTP_GET, serveAppImmutableNodes_0BwMwsby6Js);
    server->on("/app/immutable/nodes/1.wFN1xriZ.js", HTTP_GET, serveAppImmutableNodes_1WFn1xriZJs);
    server->on("/app/immutable/nodes/2.CkQJtU2r.js", HTTP_GET, serveAppImmutableNodes_2CkQJtU2rJs);
    server->on("/app/immutable/nodes/3.BUA0SVsG.js", HTTP_GET, serveAppImmutableNodes_3Bua0SVsGJs);
    server->on("/app/immutable/nodes/4.CNVkzFMK.js", HTTP_GET, serveAppImmutableNodes_4CnVkzFmkJs);
    server->on("/app/immutable/nodes/5.DtXHaT_r.js", HTTP_GET, serveAppImmutableNodes_5DtXHaTRJs);
    server->on("/app/immutable/nodes/6.DRy3oFhp.js", HTTP_GET, serveAppImmutableNodes_6DRy3oFhpJs);
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
