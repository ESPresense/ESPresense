/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,291 bytes
 * ui_html: 3,255 bytes
 * ui_app_immutable_entry_js: 45,553 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 60,069 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.L4iuwCjw.css", HTTP_GET, serveAppImmutableAssetsInternalL4iuwCjwCss);
    server->on("/app/immutable/assets/start.B3RPpGV3.css", HTTP_GET, serveAppImmutableAssetsStartB3RPpGv3Css);
    server->on("/app/immutable/entry/app.CvpVAVRu.js", HTTP_GET, serveAppImmutableEntryAppCvpVavRuJs);
    server->on("/app/immutable/entry/start.569gwKzU.js", HTTP_GET, serveAppImmutableEntryStart_569gwKzUJs);
    server->on("/app/immutable/nodes/0.B6yh7lCg.js", HTTP_GET, serveAppImmutableNodes_0B6yh7lCgJs);
    server->on("/app/immutable/nodes/1.DGrzI1Ku.js", HTTP_GET, serveAppImmutableNodes_1DGrzI1KuJs);
    server->on("/app/immutable/nodes/2.CLWKa98L.js", HTTP_GET, serveAppImmutableNodes_2ClwKa98LJs);
    server->on("/app/immutable/nodes/3.BCOhw58x.js", HTTP_GET, serveAppImmutableNodes_3BcOhw58xJs);
    server->on("/app/immutable/nodes/4.rTwmxgzp.js", HTTP_GET, serveAppImmutableNodes_4RTwmxgzpJs);
    server->on("/app/immutable/nodes/5.BPbMaUp3.js", HTTP_GET, serveAppImmutableNodes_5BPbMaUp3Js);
    server->on("/app/immutable/nodes/6.gXAZFNAp.js", HTTP_GET, serveAppImmutableNodes_6GXazfnApJs);
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
