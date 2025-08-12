/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,210 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 47,490 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 61,930 bytes
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
    server->on("/app/immutable/entry/app.CbQ8zhJi.js", HTTP_GET, serveAppImmutableEntryAppCbQ8zhJiJs);
    server->on("/app/immutable/entry/start.DARC7OAV.js", HTTP_GET, serveAppImmutableEntryStartDarc7OavJs);
    server->on("/app/immutable/nodes/0.PUUIgmyn.js", HTTP_GET, serveAppImmutableNodes_0PuuIgmynJs);
    server->on("/app/immutable/nodes/1.Xrd5Zu6r.js", HTTP_GET, serveAppImmutableNodes_1Xrd5Zu6rJs);
    server->on("/app/immutable/nodes/2.CQktS0WM.js", HTTP_GET, serveAppImmutableNodes_2CQktS0WmJs);
    server->on("/app/immutable/nodes/3.CLGTGq5-.js", HTTP_GET, serveAppImmutableNodes_3ClgtGq5Js);
    server->on("/app/immutable/nodes/4.CjwcJVpe.js", HTTP_GET, serveAppImmutableNodes_4CjwcJVpeJs);
    server->on("/app/immutable/nodes/5.DPuH5rXm.js", HTTP_GET, serveAppImmutableNodes_5DPuH5rXmJs);
    server->on("/app/immutable/nodes/6.DnXwQGGC.js", HTTP_GET, serveAppImmutableNodes_6DnXwQggcJs);
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
