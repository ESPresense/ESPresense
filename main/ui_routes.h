/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 14,520 bytes
 * ui_app_immutable_chunks_js: 66,840 bytes
 * ui_app_immutable_entry_js: 270 bytes
 * ui_app_immutable_nodes_js: 542 bytes
 * ui_html: 4,647 bytes
 * ui_svg: 456 bytes
 * Total: 87,275 bytes
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
    HttpWebServer::registerGet(server, "/app/immutable/chunks/CknlNzFR.js", serveAppImmutableChunksCknlNzFrJs);
    HttpWebServer::registerGet(server, "/app/immutable/entry/app.BoTWed8q.js", serveAppImmutableEntryAppBoTWed8qJs);
    HttpWebServer::registerGet(server, "/app/immutable/entry/start.3WO0VHtn.js", serveAppImmutableEntryStart_3Wo0VHtnJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/0.DfWx6LNl.js", serveAppImmutableNodes_0DfWx6LNlJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/1.C0SoZoun.js", serveAppImmutableNodes_1C0SoZounJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/2.C2P3AQp-.js", serveAppImmutableNodes_2C2P3AQpJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/3.DCiJv5KX.js", serveAppImmutableNodes_3DCiJv5KxJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/4.CXqEmZiQ.js", serveAppImmutableNodes_4CXqEmZiQJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/5.4pZA4BPK.js", serveAppImmutableNodes_5_4pZa4BpkJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/6.DA80xjrK.js", serveAppImmutableNodes_6Da80xjrKJs);
    HttpWebServer::registerGet(server, "/app/immutable/nodes/7.CQJBQHga.js", serveAppImmutableNodes_7CqjbqHgaJs);
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
