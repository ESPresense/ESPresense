/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,361 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 48,492 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 63,073 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CNjsDNpd.css", HTTP_GET, serveAppImmutableAssetsInternalCNjsDNpdCss);
    server->on("/app/immutable/assets/start.BSuysxAz.css", HTTP_GET, serveAppImmutableAssetsStartBSuysxAzCss);
    server->on("/app/immutable/entry/app.m_hIAJCm.js", HTTP_GET, serveAppImmutableEntryAppMHIajCmJs);
    server->on("/app/immutable/entry/start.Cg6gxku0.js", HTTP_GET, serveAppImmutableEntryStartCg6gxku0Js);
    server->on("/app/immutable/nodes/0.DzO6GfrZ.js", HTTP_GET, serveAppImmutableNodes_0DzO6GfrZJs);
    server->on("/app/immutable/nodes/1.DcqrHjid.js", HTTP_GET, serveAppImmutableNodes_1DcqrHjidJs);
    server->on("/app/immutable/nodes/2.B89De5w0.js", HTTP_GET, serveAppImmutableNodes_2B89De5w0Js);
    server->on("/app/immutable/nodes/3.BriTbe72.js", HTTP_GET, serveAppImmutableNodes_3BriTbe72Js);
    server->on("/app/immutable/nodes/4.DGHs1Utu.js", HTTP_GET, serveAppImmutableNodes_4DgHs1UtuJs);
    server->on("/app/immutable/nodes/5.C1yW5WP1.js", HTTP_GET, serveAppImmutableNodes_5C1yW5Wp1Js);
    server->on("/app/immutable/nodes/6.CCGzHOHx.js", HTTP_GET, serveAppImmutableNodes_6CcGzHoHxJs);
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
