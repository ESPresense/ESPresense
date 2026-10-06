/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 14,520 bytes
 * ui_app_immutable_chunks_js: 66,933 bytes
 * ui_app_immutable_entry_js: 268 bytes
 * ui_app_immutable_nodes_js: 542 bytes
 * ui_html: 4,654 bytes
 * ui_svg: 456 bytes
 * Total: 87,373 bytes
 */

#pragma once

#include "HttpStatic.h"
#include "ui_app_immutable_assets_css.h"
#include "ui_app_immutable_chunks_js.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_html.h"
#include "ui_svg.h"

inline void setupRoutes(httpd_handle_t server) {
    HttpWebServer::registerGet(server, "/app/immutable/assets/index.DKQns9on.css", serveAppImmutableAssetsIndexDkQns9onCss);
    HttpWebServer::registerGet(server, "/app/immutable/chunks/D8_JLsGt.js", serveAppImmutableChunksD8JLsGtJs);
    HttpWebServer::registerGet(server, "/app/immutable/entry/app.C2GjHXIv.js", serveAppImmutableEntryAppC2GjHxIvJs);
    HttpWebServer::registerGet(server, "/app/immutable/entry/start.BmmilLnl.js", serveAppImmutableEntryStartBmmilLnlJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/0.DMf2xGh3.js", serveAppImmutableNodes_0DMf2xGh3Js);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/1.C-gC6q8B.js", serveAppImmutableNodes_1CGC6q8BJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/2.DpXyyvBL.js", serveAppImmutableNodes_2DpXyyvBlJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/3.BkNFl8Yg.js", serveAppImmutableNodes_3BkNFl8YgJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/4.Cewoj5fK.js", serveAppImmutableNodes_4Cewoj5fKJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/5.C3_GHFxa.js", serveAppImmutableNodes_5C3GhFxaJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/6.CLBKkNjJ.js", serveAppImmutableNodes_6ClbKkNjJJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/7.iZGv-SNw.js", serveAppImmutableNodes_7IZGvSNwJs);
    HttpWebServer::registerGet(server, "/favicon.svg", serveFaviconSvg);
    // HTML routes
    HttpWebServer::registerGet(server, "/", serveIndexHtml);
    HttpWebServer::registerGet(server, "/devices", serveDevicesHtml);
    HttpWebServer::registerGet(server, "/devices.html", serveDevicesHtml);
    HttpWebServer::registerGet(server, "/fingerprints", serveFingerprintsHtml);
    HttpWebServer::registerGet(server, "/fingerprints.html", serveFingerprintsHtml);
    HttpWebServer::registerGet(server, "/hardware", serveHardwareHtml);
    HttpWebServer::registerGet(server, "/hardware.html", serveHardwareHtml);
    HttpWebServer::registerGet(server, "/network", serveNetworkHtml);
    HttpWebServer::registerGet(server, "/network.html", serveNetworkHtml);
    HttpWebServer::registerGet(server, "/settings", serveSettingsHtml);
    HttpWebServer::registerGet(server, "/settings.html", serveSettingsHtml);
}
