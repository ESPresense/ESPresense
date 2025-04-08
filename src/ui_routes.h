/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,260 bytes
 * ui_app_immutable_entry_js: 44,878 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 57,981 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DN2mWWgh.css", HTTP_GET, serveAppImmutableAssetsInternalDn2mWWghCss);
    server->on("/app/immutable/assets/start.BfkQNPT2.css", HTTP_GET, serveAppImmutableAssetsStartBfkQnpt2Css);
    server->on("/app/immutable/entry/app.C9wPmA2T.js", HTTP_GET, serveAppImmutableEntryAppC9wPmA2TJs);
    server->on("/app/immutable/entry/start.Bu0bgQ5u.js", HTTP_GET, serveAppImmutableEntryStartBu0bgQ5uJs);
    server->on("/app/immutable/nodes/0.Bf6RALT7.js", HTTP_GET, serveAppImmutableNodes_0Bf6Ralt7Js);
    server->on("/app/immutable/nodes/1.BziBy9DT.js", HTTP_GET, serveAppImmutableNodes_1BziBy9DtJs);
    server->on("/app/immutable/nodes/2.C0BgDOMC.js", HTTP_GET, serveAppImmutableNodes_2C0BgDomcJs);
    server->on("/app/immutable/nodes/3.Ah7RNuTi.js", HTTP_GET, serveAppImmutableNodes_3Ah7RNuTiJs);
    server->on("/app/immutable/nodes/4.yRDvYGMg.js", HTTP_GET, serveAppImmutableNodes_4YRDvYgMgJs);
    server->on("/app/immutable/nodes/5.CcbnVcKE.js", HTTP_GET, serveAppImmutableNodes_5CcbnVcKeJs);
    server->on("/app/immutable/nodes/6.C7bRmeRU.js", HTTP_GET, serveAppImmutableNodes_6C7bRmeRuJs);
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
