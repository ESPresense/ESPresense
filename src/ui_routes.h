/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,984 bytes
 * ui_html: 4,338 bytes
 * ui_app_immutable_entry_js: 64,062 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,430 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CMfVrT3e.css", HTTP_GET, serveAppImmutableAssetsInternalCMfVrT3eCss);
    server->on("/app/immutable/assets/start.CMfVrT3e.css", HTTP_GET, serveAppImmutableAssetsStartCMfVrT3eCss);
    server->on("/app/immutable/entry/app.Dn0YXpHP.js", HTTP_GET, serveAppImmutableEntryAppDn0YXpHpJs);
    server->on("/app/immutable/entry/start.DTrQKI17.js", HTTP_GET, serveAppImmutableEntryStartDTrQki17Js);
    server->on("/app/immutable/nodes/0.DEx71AcX.js", HTTP_GET, serveAppImmutableNodes_0DEx71AcXJs);
    server->on("/app/immutable/nodes/1.Df8APzTw.js", HTTP_GET, serveAppImmutableNodes_1Df8APzTwJs);
    server->on("/app/immutable/nodes/2.BP3mcvtm.js", HTTP_GET, serveAppImmutableNodes_2Bp3mcvtmJs);
    server->on("/app/immutable/nodes/3.C5w8CR2B.js", HTTP_GET, serveAppImmutableNodes_3C5w8Cr2BJs);
    server->on("/app/immutable/nodes/4.CsjbFNo6.js", HTTP_GET, serveAppImmutableNodes_4CsjbFNo6Js);
    server->on("/app/immutable/nodes/5.B53hE2xk.js", HTTP_GET, serveAppImmutableNodes_5B53hE2xkJs);
    server->on("/app/immutable/nodes/6.C9C4ydEf.js", HTTP_GET, serveAppImmutableNodes_6C9C4ydEfJs);
    server->on("/app/immutable/nodes/7.CYa5o0A6.js", HTTP_GET, serveAppImmutableNodes_7CYa5o0A6Js);
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
