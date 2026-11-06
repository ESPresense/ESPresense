/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 16,477 bytes
 * ui_app_immutable_js: 90,192 bytes
 * ui_app_json: 26 bytes
 * ui_html: 4,872 bytes
 * ui_svg: 456 bytes
 * Total: 112,023 bytes
 */

#pragma once

#include "HttpStatic.h"
#include "ui_app_immutable_assets_css.h"
#include "ui_app_immutable_js.h"
#include "ui_app_json.h"
#include "ui_html.h"
#include "ui_svg.h"

inline void setupRoutes(httpd_handle_t server) {
    HttpWebServer::registerGet(server, "/app/immutable/assets/bundle.Du9bwH5d.css", serveAppImmutableAssetsBundleDu9bwH5dCss);
    HttpWebServer::registerGet(server, "/app/immutable/bundle.P0SwEJ8H.js", serveAppImmutableBundleP0SwEj8HJs);
    HttpWebServer::registerGet(server, "/app/version.json", serveAppVersionJson);
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
