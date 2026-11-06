/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 16,368 bytes
 * ui_app_immutable_js: 89,809 bytes
 * ui_app_json: 26 bytes
 * ui_html: 4,879 bytes
 * ui_svg: 456 bytes
 * Total: 111,538 bytes
 */

#pragma once

#include "HttpStatic.h"
#include "ui_app_immutable_assets_css.h"
#include "ui_app_immutable_js.h"
#include "ui_app_json.h"
#include "ui_html.h"
#include "ui_svg.h"

inline void setupRoutes(httpd_handle_t server) {
    HttpWebServer::registerGet(server, "/app/immutable/assets/bundle.CiQkqXRL.css", serveAppImmutableAssetsBundleCiQkqXrlCss);
    HttpWebServer::registerGet(server, "/app/immutable/bundle.CQeFR80S.js", serveAppImmutableBundleCQeFr80SJs);
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
