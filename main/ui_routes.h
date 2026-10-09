/*
 * Web UI Routes
 *
 * Compressed Size Summary:
<<<<<<< HEAD
 * ui_app_immutable_assets_css: 14,745 bytes
 * ui_app_immutable_js: 81,088 bytes
 * ui_html: 4,781 bytes
 * ui_svg: 456 bytes
 * Total: 101,070 bytes
=======
 * ui_app_immutable_assets_css: 14,520 bytes
 * ui_app_immutable_js: 79,656 bytes
 * ui_html: 4,092 bytes
 * ui_svg: 456 bytes
 * Total: 98,724 bytes
>>>>>>> 0d3fb970 (feat: relay outputs published to Home Assistant as switches (#1316))
 */

#pragma once

#include "HttpStatic.h"
#include "ui_app_immutable_assets_css.h"
#include "ui_app_immutable_js.h"
#include "ui_html.h"
#include "ui_svg.h"

inline void setupRoutes(httpd_handle_t server) {
<<<<<<< HEAD
    HttpWebServer::registerGet(server, "/app/immutable/assets/bundle.D2Rqk1sM.css", serveAppImmutableAssetsBundleD2Rqk1sMCss);
    HttpWebServer::registerGet(server, "/app/immutable/bundle.a_2b8IJv.js", serveAppImmutableBundleA_2b8IJvJs);
=======
    HttpWebServer::registerGet(server, "/app/immutable/assets/bundle.DKQns9on.css", serveAppImmutableAssetsBundleDkQns9onCss);
    HttpWebServer::registerGet(server, "/app/immutable/bundle.CvU4ShYo.js", serveAppImmutableBundleCvU4ShYoJs);
>>>>>>> 0d3fb970 (feat: relay outputs published to Home Assistant as switches (#1316))
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
