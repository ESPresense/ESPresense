/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,210 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 45,085 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 59,515 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.R2nWuKlp.css", HTTP_GET, serveAppImmutableAssetsInternalR2nWuKlpCss);
    server->on("/app/immutable/assets/start.CquYjHqP.css", HTTP_GET, serveAppImmutableAssetsStartCquYjHqPCss);
    server->on("/app/immutable/entry/app.D_oxjT7f.js", HTTP_GET, serveAppImmutableEntryAppDOxjT7fJs);
    server->on("/app/immutable/entry/start.Cj_j-UZw.js", HTTP_GET, serveAppImmutableEntryStartCjJUZwJs);
    server->on("/app/immutable/nodes/0.DtCMxQRN.js", HTTP_GET, serveAppImmutableNodes_0DtCMxQrnJs);
    server->on("/app/immutable/nodes/1.CQ2x0bST.js", HTTP_GET, serveAppImmutableNodes_1Cq2x0bStJs);
    server->on("/app/immutable/nodes/2.nAdmUitD.js", HTTP_GET, serveAppImmutableNodes_2NAdmUitDJs);
    server->on("/app/immutable/nodes/3.BzLmW3yr.js", HTTP_GET, serveAppImmutableNodes_3BzLmW3yrJs);
    server->on("/app/immutable/nodes/4.DTRpoX6z.js", HTTP_GET, serveAppImmutableNodes_4DtRpoX6zJs);
    server->on("/app/immutable/nodes/5.BZsnPiPF.js", HTTP_GET, serveAppImmutableNodes_5BZsnPiPfJs);
    server->on("/app/immutable/nodes/6.BdN3OT0-.js", HTTP_GET, serveAppImmutableNodes_6BdN3Ot0Js);
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
