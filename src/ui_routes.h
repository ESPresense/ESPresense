/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 14,520 bytes
 * ui_app_immutable_chunks_js: 66,443 bytes
 * ui_app_immutable_entry_js: 269 bytes
 * ui_app_immutable_nodes_js: 542 bytes
 * ui_html: 4,635 bytes
 * ui_svg: 456 bytes
 * Total: 86,865 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_app_immutable_chunks_js.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_html.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/index.DKQns9on.css", HTTP_GET, serveAppImmutableAssetsIndexDkQns9onCss);
    server->on("/app/immutable/chunks/CMP9cdi5.js", HTTP_GET, serveAppImmutableChunksCmp9cdi5Js);
    server->on("/app/immutable/entry/app.I_zQ5d7g.js", HTTP_GET, serveAppImmutableEntryAppIZQ5d7gJs);
    server->on("/app/immutable/entry/start.CjzvHTP5.js", HTTP_GET, serveAppImmutableEntryStartCjzvHtp5Js);
    server->on("/app/immutable/nodes/0.DOy7f0Ou.js", HTTP_GET, serveAppImmutableNodes_0DOy7f0OuJs);
    server->on("/app/immutable/nodes/1.CGXh3qT7.js", HTTP_GET, serveAppImmutableNodes_1CgXh3qT7Js);
    server->on("/app/immutable/nodes/2.Doza1Mz-.js", HTTP_GET, serveAppImmutableNodes_2Doza1MzJs);
    server->on("/app/immutable/nodes/3.Noau5fab.js", HTTP_GET, serveAppImmutableNodes_3Noau5fabJs);
    server->on("/app/immutable/nodes/4.B1_Gk9uf.js", HTTP_GET, serveAppImmutableNodes_4B1Gk9ufJs);
    server->on("/app/immutable/nodes/5.B837YNxc.js", HTTP_GET, serveAppImmutableNodes_5B837YNxcJs);
    server->on("/app/immutable/nodes/6.CGLNHZ0m.js", HTTP_GET, serveAppImmutableNodes_6Cglnhz0mJs);
    server->on("/app/immutable/nodes/7.Bs_uqq-2.js", HTTP_GET, serveAppImmutableNodes_7BsUqq_2Js);
    server->on("/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes
    server->on("/", HTTP_GET, serveIndexHtml);
    server->on("/devices", HTTP_GET, serveDevicesHtml);
    server->on("/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/hardware", HTTP_GET, serveHardwareHtml);
    server->on("/hardware.html", HTTP_GET, serveHardwareHtml);
    server->on("/network", HTTP_GET, serveNetworkHtml);
    server->on("/network.html", HTTP_GET, serveNetworkHtml);
    server->on("/settings", HTTP_GET, serveSettingsHtml);
    server->on("/settings.html", HTTP_GET, serveSettingsHtml);
}
