/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,206 bytes
 * ui_html: 3,906 bytes
 * ui_app_immutable_entry_js: 64,629 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 79,787 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.BzbuOEt-.css", HTTP_GET, serveAppImmutableAssetsInternalBzbuOEtCss);
    server->on("/app/immutable/assets/start.-4VrLTrv.css", HTTP_GET, serveAppImmutableAssetsStart_4VrLTrvCss);
    server->on("/app/immutable/entry/app.BICD25jh.js", HTTP_GET, serveAppImmutableEntryAppBicd25jhJs);
    server->on("/app/immutable/entry/start.Cish4VEN.js", HTTP_GET, serveAppImmutableEntryStartCish4VenJs);
    server->on("/app/immutable/nodes/0.BZlr8C09.js", HTTP_GET, serveAppImmutableNodes_0BZlr8C09Js);
    server->on("/app/immutable/nodes/1.DBLDRLkE.js", HTTP_GET, serveAppImmutableNodes_1DbldrLkEJs);
    server->on("/app/immutable/nodes/2.BlEKZG4K.js", HTTP_GET, serveAppImmutableNodes_2BlEkzg4KJs);
    server->on("/app/immutable/nodes/3.Cg_uADzX.js", HTTP_GET, serveAppImmutableNodes_3CgUADzXJs);
    server->on("/app/immutable/nodes/4.B8eQt7C4.js", HTTP_GET, serveAppImmutableNodes_4B8eQt7C4Js);
    server->on("/app/immutable/nodes/5.IcVqiM_O.js", HTTP_GET, serveAppImmutableNodes_5IcVqiMOJs);
    server->on("/app/immutable/nodes/6.C38xkRPl.js", HTTP_GET, serveAppImmutableNodes_6C38xkRPlJs);
    server->on("/app/immutable/nodes/7.BrtvIw-q.js", HTTP_GET, serveAppImmutableNodes_7BrtvIwQJs);
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
