/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,122 bytes
 * ui_html: 4,326 bytes
 * ui_app_immutable_entry_js: 64,335 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,829 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CKNao7CM.css", HTTP_GET, serveAppImmutableAssetsInternalCkNao7CmCss);
    server->on("/app/immutable/assets/start.CKNao7CM.css", HTTP_GET, serveAppImmutableAssetsStartCkNao7CmCss);
    server->on("/app/immutable/entry/app.BnvTkN43.js", HTTP_GET, serveAppImmutableEntryAppBnvTkN43Js);
    server->on("/app/immutable/entry/start.DCduz8B0.js", HTTP_GET, serveAppImmutableEntryStartDCduz8B0Js);
    server->on("/app/immutable/nodes/0.DMVx6Ik4.js", HTTP_GET, serveAppImmutableNodes_0DmVx6Ik4Js);
    server->on("/app/immutable/nodes/1.Cq4MLs9E.js", HTTP_GET, serveAppImmutableNodes_1Cq4MLs9EJs);
    server->on("/app/immutable/nodes/2.Dvx-qM_y.js", HTTP_GET, serveAppImmutableNodes_2DvxQMYJs);
    server->on("/app/immutable/nodes/3.BK-GkgCh.js", HTTP_GET, serveAppImmutableNodes_3BkGkgChJs);
    server->on("/app/immutable/nodes/4.BClNOwGR.js", HTTP_GET, serveAppImmutableNodes_4BClNOwGrJs);
    server->on("/app/immutable/nodes/5.D30F4klc.js", HTTP_GET, serveAppImmutableNodes_5D30F4klcJs);
    server->on("/app/immutable/nodes/6.CSdQ8u99.js", HTTP_GET, serveAppImmutableNodes_6CSdQ8u99Js);
    server->on("/app/immutable/nodes/7.BQU-e8sw.js", HTTP_GET, serveAppImmutableNodes_7BquE8swJs);
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
