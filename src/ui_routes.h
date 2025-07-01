/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 9,890 bytes
 * ui_html: 3,240 bytes
 * ui_app_immutable_entry_js: 44,974 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 59,074 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.D3ATewl9.css", HTTP_GET, serveAppImmutableAssetsInternalD3ATewl9Css);
    server->on("/app/immutable/assets/start.uxFJ81Pk.css", HTTP_GET, serveAppImmutableAssetsStartUxFj81PkCss);
    server->on("/app/immutable/entry/app.DbgMR1Wl.js", HTTP_GET, serveAppImmutableEntryAppDbgMr1WlJs);
    server->on("/app/immutable/entry/start.CrOpmmKI.js", HTTP_GET, serveAppImmutableEntryStartCrOpmmKiJs);
    server->on("/app/immutable/nodes/0.sc-sefNw.js", HTTP_GET, serveAppImmutableNodes_0ScSefNwJs);
    server->on("/app/immutable/nodes/1.jNPvjqZF.js", HTTP_GET, serveAppImmutableNodes_1JNPvjqZfJs);
    server->on("/app/immutable/nodes/2.DN0Rkfqn.js", HTTP_GET, serveAppImmutableNodes_2Dn0RkfqnJs);
    server->on("/app/immutable/nodes/3.BscMLsfp.js", HTTP_GET, serveAppImmutableNodes_3BscMLsfpJs);
    server->on("/app/immutable/nodes/4.DZrlC2c7.js", HTTP_GET, serveAppImmutableNodes_4DZrlC2c7Js);
    server->on("/app/immutable/nodes/5.D4eG0Tng.js", HTTP_GET, serveAppImmutableNodes_5D4eG0TngJs);
    server->on("/app/immutable/nodes/6.D3lVXwX1.js", HTTP_GET, serveAppImmutableNodes_6D3lVXwX1Js);
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
