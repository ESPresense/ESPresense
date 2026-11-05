/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 16,348 bytes
 * ui_app_immutable_js: 89,351 bytes
 * ui_html: 4,795 bytes
 * ui_svg: 456 bytes
 * Total: 110,950 bytes
 */

#pragma once

#include "HttpStatic.h"
#include "ui_app_immutable_assets_css.h"
#include "ui_app_immutable_js.h"
#include "ui_html.h"
#include "ui_svg.h"

inline void setupRoutes(httpd_handle_t server) {
    HttpWebServer::registerGet(server, "/app/immutable/assets/bundle.Ct3es0Zj.css", serveAppImmutableAssetsBundleCt3es0ZjCss);
    HttpWebServer::registerGet(server, "/app/immutable/bundle.FXwAA7BX.js", serveAppImmutableBundleFXwAa7BxJs);
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
    HttpWebServer::registerGet(server, "/templates", serveTemplatesHtml);
    HttpWebServer::registerGet(server, "/templates.html", serveTemplatesHtml);
}
