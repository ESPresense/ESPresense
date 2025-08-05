/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,210 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 45,392 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 59,832 bytes
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
    server->on("/app/immutable/entry/app.fowr95T4.js", HTTP_GET, serveAppImmutableEntryAppFowr95T4Js);
    server->on("/app/immutable/entry/start.DM5jymCJ.js", HTTP_GET, serveAppImmutableEntryStartDm5jymCjJs);
    server->on("/app/immutable/nodes/0.VJ1oX0BK.js", HTTP_GET, serveAppImmutableNodes_0Vj1oX0BkJs);
    server->on("/app/immutable/nodes/1.CM-B5IBc.js", HTTP_GET, serveAppImmutableNodes_1CmB5IBcJs);
    server->on("/app/immutable/nodes/2.0-Mkxr1H.js", HTTP_GET, serveAppImmutableNodes_2_0Mkxr1HJs);
    server->on("/app/immutable/nodes/3.Dt2nhPDU.js", HTTP_GET, serveAppImmutableNodes_3Dt2nhPduJs);
    server->on("/app/immutable/nodes/4.tlW1ubHJ.js", HTTP_GET, serveAppImmutableNodes_4TlW1ubHjJs);
    server->on("/app/immutable/nodes/5.CU_kX_PQ.js", HTTP_GET, serveAppImmutableNodes_5CuKXPqJs);
    server->on("/app/immutable/nodes/6.DLNJI1X9.js", HTTP_GET, serveAppImmutableNodes_6Dlnji1X9Js);
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
