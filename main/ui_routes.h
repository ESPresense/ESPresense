/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 14,520 bytes
 * ui_app_immutable_chunks_js: 66,367 bytes
 * ui_app_immutable_entry_js: 268 bytes
 * ui_app_immutable_nodes_js: 542 bytes
 * ui_html: 4,642 bytes
 * ui_svg: 456 bytes
 * Total: 86,795 bytes
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
    HttpWebServer::registerGet(server, "/app/immutable/chunks/DMngkxwn.js", serveAppImmutableChunksDMngkxwnJs);
    HttpWebServer::registerGet(server, "/app/immutable/entry/app.BGo4gh21.js", serveAppImmutableEntryAppBGo4gh21Js);
    HttpWebServer::registerGet(server, "/app/immutable/entry/start.BjmyzvU5.js", serveAppImmutableEntryStartBjmyzvU5Js);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/0.5Pyn4wEx.js", serveAppImmutableNodes_0_5Pyn4wExJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/1.CDamB5mx.js", serveAppImmutableNodes_1CDamB5mxJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/2.Dqq4iJSc.js", serveAppImmutableNodes_2Dqq4iJScJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/3.CNTzsAj4.js", serveAppImmutableNodes_3CnTzsAj4Js);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/4.BfcZzNfr.js", serveAppImmutableNodes_4BfcZzNfrJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/5.DfrgYewy.js", serveAppImmutableNodes_5DfrgYewyJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/6.CbTkKIC0.js", serveAppImmutableNodes_6CbTkKic0Js);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/7.DrbGeEtQ.js", serveAppImmutableNodes_7DrbGeEtQJs);
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
