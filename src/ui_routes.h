/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,541 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 49,136 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 63,902 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DAvTNVNL.css", HTTP_GET, serveAppImmutableAssetsInternalDAvTnvnlCss);
    server->on("/app/immutable/assets/start.DV1CVczO.css", HTTP_GET, serveAppImmutableAssetsStartDv1CVczOCss);
    server->on("/app/immutable/entry/app.ChbvkBnu.js", HTTP_GET, serveAppImmutableEntryAppChbvkBnuJs);
    server->on("/app/immutable/entry/start.C0zoVC_q.js", HTTP_GET, serveAppImmutableEntryStartC0zoVcQJs);
    server->on("/app/immutable/nodes/0.By9Cbb79.js", HTTP_GET, serveAppImmutableNodes_0By9Cbb79Js);
    server->on("/app/immutable/nodes/1.B87085a0.js", HTTP_GET, serveAppImmutableNodes_1B87085a0Js);
    server->on("/app/immutable/nodes/2.DzVdmT_Q.js", HTTP_GET, serveAppImmutableNodes_2DzVdmTQJs);
    server->on("/app/immutable/nodes/3.5G_P5ypj.js", HTTP_GET, serveAppImmutableNodes_3_5GP5ypjJs);
    server->on("/app/immutable/nodes/4.DIw5rsol.js", HTTP_GET, serveAppImmutableNodes_4DIw5rsolJs);
    server->on("/app/immutable/nodes/5.z-R1OmTN.js", HTTP_GET, serveAppImmutableNodes_5ZR1OmTnJs);
    server->on("/app/immutable/nodes/6.2TS6ddTV.js", HTTP_GET, serveAppImmutableNodes_6_2Ts6ddTvJs);
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
