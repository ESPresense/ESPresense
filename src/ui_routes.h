/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,462 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 66,271 bytes
 * ui_app_immutable_nodes_js: 519 bytes
 * ui_svg: 456 bytes
 * Total: 80,953 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CcwqoBYG.css", HTTP_GET, serveAppImmutableAssetsInternalCcwqoBygCss);
    server->on("/app/immutable/assets/start.XzWBJjFS.css", HTTP_GET, serveAppImmutableAssetsStartXzWbJjFsCss);
    server->on("/app/immutable/entry/app.BdCrjAuG.js", HTTP_GET, serveAppImmutableEntryAppBdCrjAuGJs);
    server->on("/app/immutable/entry/start.Bi7BJqnl.js", HTTP_GET, serveAppImmutableEntryStartBi7BJqnlJs);
    server->on("/app/immutable/nodes/0.B7St6AAe.js", HTTP_GET, serveAppImmutableNodes_0B7St6AAeJs);
    server->on("/app/immutable/nodes/1.DNp3LPWs.js", HTTP_GET, serveAppImmutableNodes_1DNp3LpWsJs);
    server->on("/app/immutable/nodes/2.BP9aWRiZ.js", HTTP_GET, serveAppImmutableNodes_2Bp9aWRiZJs);
    server->on("/app/immutable/nodes/3.CMivW41x.js", HTTP_GET, serveAppImmutableNodes_3CMivW41xJs);
    server->on("/app/immutable/nodes/4.D9gghxzG.js", HTTP_GET, serveAppImmutableNodes_4D9gghxzGJs);
    server->on("/app/immutable/nodes/5.uF9VGUPQ.js", HTTP_GET, serveAppImmutableNodes_5UF9VgupqJs);
    server->on("/app/immutable/nodes/6.ivTlQ2Rp.js", HTTP_GET, serveAppImmutableNodes_6IvTlQ2RpJs);
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
