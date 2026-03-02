/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,122 bytes
 * ui_html: 4,326 bytes
 * ui_app_immutable_entry_js: 64,002 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,496 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CKNao7CM.css", HTTP_GET, serveAppImmutableAssetsInternalCkNao7CmCss);
    server->on("/app/immutable/assets/start.CKNao7CM.css", HTTP_GET, serveAppImmutableAssetsStartCkNao7CmCss);
    server->on("/app/immutable/entry/app.GXVMwpsF.js", HTTP_GET, serveAppImmutableEntryAppGxvMwpsFJs);
    server->on("/app/immutable/entry/start.CN9W8DCx.js", HTTP_GET, serveAppImmutableEntryStartCn9W8DCxJs);
    server->on("/app/immutable/nodes/0.Bm6g_qqy.js", HTTP_GET, serveAppImmutableNodes_0Bm6gQqyJs);
    server->on("/app/immutable/nodes/1.C-8T2DF4.js", HTTP_GET, serveAppImmutableNodes_1C_8T2Df4Js);
    server->on("/app/immutable/nodes/2.BhG8tDz2.js", HTTP_GET, serveAppImmutableNodes_2BhG8tDz2Js);
    server->on("/app/immutable/nodes/3.DvyiHZ9O.js", HTTP_GET, serveAppImmutableNodes_3DvyiHz9OJs);
    server->on("/app/immutable/nodes/4.x4a2Qrji.js", HTTP_GET, serveAppImmutableNodes_4X4a2QrjiJs);
    server->on("/app/immutable/nodes/5.IqXFnsRc.js", HTTP_GET, serveAppImmutableNodes_5IqXFnsRcJs);
    server->on("/app/immutable/nodes/6.5w_M2EZQ.js", HTTP_GET, serveAppImmutableNodes_6_5wM2EzqJs);
    server->on("/app/immutable/nodes/7.BhD_YW_g.js", HTTP_GET, serveAppImmutableNodes_7BhDYwGJs);
    server->on("/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes
    server->on("/devices", HTTP_GET, serveDevicesHtml);
    server->on("/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/hardware", HTTP_GET, serveHardwareHtml);
    server->on("/hardware.html", HTTP_GET, serveHardwareHtml);
    server->on("/", HTTP_GET, serveIndexHtml);
    server->on("/network", HTTP_GET, serveNetworkHtml);
    server->on("/network.html", HTTP_GET, serveNetworkHtml);
    server->on("/settings", HTTP_GET, serveSettingsHtml);
    server->on("/settings.html", HTTP_GET, serveSettingsHtml);
}
