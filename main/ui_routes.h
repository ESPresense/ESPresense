/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 14,520 bytes
 * ui_app_immutable_chunks_js: 66,872 bytes
 * ui_app_immutable_entry_js: 266 bytes
 * ui_app_immutable_nodes_js: 542 bytes
 * ui_html: 4,642 bytes
 * ui_svg: 456 bytes
 * Total: 87,298 bytes
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
    HttpWebServer::registerGet(server, "/app/immutable/chunks/uPnh9dmP.js", serveAppImmutableChunksUPnh9dmPJs);
    HttpWebServer::registerGet(server, "/app/immutable/entry/app.BOZ4E_NW.js", serveAppImmutableEntryAppBoz4ENwJs);
    HttpWebServer::registerGet(server, "/app/immutable/entry/start.DxOdetmg.js", serveAppImmutableEntryStartDxOdetmgJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/0.CzehTWhP.js", serveAppImmutableNodes_0CzehTWhPJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/1.BbP6JRJN.js", serveAppImmutableNodes_1BbP6JrjnJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/2.BjN4zlCD.js", serveAppImmutableNodes_2BjN4zlCdJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/3.Bsfh03KU.js", serveAppImmutableNodes_3Bsfh03KuJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/4.CL8DBNuy.js", serveAppImmutableNodes_4Cl8DbNuyJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/5.BbrCS0Mt.js", serveAppImmutableNodes_5BbrCs0MtJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/6.BeQyzt12.js", serveAppImmutableNodes_6BeQyzt12Js);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/7.BpwNrJXk.js", serveAppImmutableNodes_7BpwNrJXkJs);
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
