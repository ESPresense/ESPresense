/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 14,520 bytes
 * ui_app_immutable_chunks_js: 66,872 bytes
 * ui_app_immutable_entry_js: 266 bytes
 * ui_app_immutable_nodes_js: 542 bytes
 * ui_html: 4,642 bytes
 * ui_svg: 456 bytes
 * Total: 87,298 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_app_immutable_chunks_js.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_html.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/index.DKQns9on.css", HTTP_GET, serveAppImmutableAssetsIndexDkQns9onCss);
    server->on("/app/immutable/chunks/uPnh9dmP.js", HTTP_GET, serveAppImmutableChunksUPnh9dmPJs);
    server->on("/app/immutable/entry/app.BOZ4E_NW.js", HTTP_GET, serveAppImmutableEntryAppBoz4ENwJs);
    server->on("/app/immutable/entry/start.DxOdetmg.js", HTTP_GET, serveAppImmutableEntryStartDxOdetmgJs);
    server->on("/app/immutable/nodes/0.CzehTWhP.js", HTTP_GET, serveAppImmutableNodes_0CzehTWhPJs);
    server->on("/app/immutable/nodes/1.BbP6JRJN.js", HTTP_GET, serveAppImmutableNodes_1BbP6JrjnJs);
    server->on("/app/immutable/nodes/2.BjN4zlCD.js", HTTP_GET, serveAppImmutableNodes_2BjN4zlCdJs);
    server->on("/app/immutable/nodes/3.Bsfh03KU.js", HTTP_GET, serveAppImmutableNodes_3Bsfh03KuJs);
    server->on("/app/immutable/nodes/4.CL8DBNuy.js", HTTP_GET, serveAppImmutableNodes_4Cl8DbNuyJs);
    server->on("/app/immutable/nodes/5.BbrCS0Mt.js", HTTP_GET, serveAppImmutableNodes_5BbrCs0MtJs);
    server->on("/app/immutable/nodes/6.BeQyzt12.js", HTTP_GET, serveAppImmutableNodes_6BeQyzt12Js);
    server->on("/app/immutable/nodes/7.BpwNrJXk.js", HTTP_GET, serveAppImmutableNodes_7BpwNrJXkJs);
    server->on("/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes
    server->on("/", HTTP_GET, serveIndexHtml);
    server->on("/devices", HTTP_GET, serveDevicesHtml);
    server->on("/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/hardware", HTTP_GET, serveHardwareHtml);
    server->on("/hardware.html", HTTP_GET, serveHardwareHtml);
    server->on("/network", HTTP_GET, serveNetworkHtml);
    server->on("/network.html", HTTP_GET, serveNetworkHtml);
    server->on("/settings", HTTP_GET, serveSettingsHtml);
    server->on("/settings.html", HTTP_GET, serveSettingsHtml);
}
