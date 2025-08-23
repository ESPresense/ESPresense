/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,291 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 48,225 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 62,731 bytes
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
    server->on("/app/immutable/entry/app.DgDi8hR5.js", HTTP_GET, serveAppImmutableEntryAppDgDi8hR5Js);
    server->on("/app/immutable/entry/start.dhmFyWrz.js", HTTP_GET, serveAppImmutableEntryStartDhmFyWrzJs);
    server->on("/app/immutable/nodes/0.DEfHCPVj.js", HTTP_GET, serveAppImmutableNodes_0DEfHcpVjJs);
    server->on("/app/immutable/nodes/1.BZ0f_0Of.js", HTTP_GET, serveAppImmutableNodes_1Bz0f_0OfJs);
    server->on("/app/immutable/nodes/2.CliBp25i.js", HTTP_GET, serveAppImmutableNodes_2CliBp25iJs);
    server->on("/app/immutable/nodes/3.lZZJQD-O.js", HTTP_GET, serveAppImmutableNodes_3LZzjqdOJs);
    server->on("/app/immutable/nodes/4.ClpblSKG.js", HTTP_GET, serveAppImmutableNodes_4ClpblSkgJs);
    server->on("/app/immutable/nodes/5.BVB09CuJ.js", HTTP_GET, serveAppImmutableNodes_5Bvb09CuJJs);
    server->on("/app/immutable/nodes/6.DJec4NFE.js", HTTP_GET, serveAppImmutableNodes_6DJec4NfeJs);
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
