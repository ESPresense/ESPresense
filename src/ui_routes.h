/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,367 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 48,540 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 63,127 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.C8cCt54N.css", HTTP_GET, serveAppImmutableAssetsInternalC8cCt54NCss);
    server->on("/app/immutable/assets/start.CB0Tr19N.css", HTTP_GET, serveAppImmutableAssetsStartCb0Tr19NCss);
    server->on("/app/immutable/entry/app.CC-Nn9XV.js", HTTP_GET, serveAppImmutableEntryAppCcNn9XvJs);
    server->on("/app/immutable/entry/start.BSoQ3Fmi.js", HTTP_GET, serveAppImmutableEntryStartBSoQ3FmiJs);
    server->on("/app/immutable/nodes/0.DTVoJsTz.js", HTTP_GET, serveAppImmutableNodes_0DtVoJsTzJs);
    server->on("/app/immutable/nodes/1.CSKmMQk0.js", HTTP_GET, serveAppImmutableNodes_1CsKmMQk0Js);
    server->on("/app/immutable/nodes/2.DYAm1q49.js", HTTP_GET, serveAppImmutableNodes_2DyAm1q49Js);
    server->on("/app/immutable/nodes/3.B1A2wEDR.js", HTTP_GET, serveAppImmutableNodes_3B1A2wEdrJs);
    server->on("/app/immutable/nodes/4.Dt_gUpa-.js", HTTP_GET, serveAppImmutableNodes_4DtGUpaJs);
    server->on("/app/immutable/nodes/5.DTPKb_0h.js", HTTP_GET, serveAppImmutableNodes_5DtpKb_0hJs);
    server->on("/app/immutable/nodes/6.BlgmpObe.js", HTTP_GET, serveAppImmutableNodes_6BlgmpObeJs);
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
