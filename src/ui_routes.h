/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 25,032 bytes
 * ui_html: 4,344 bytes
 * ui_app_immutable_entry_js: 63,850 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 94,272 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DWTNAZpk.css", HTTP_GET, serveAppImmutableAssetsInternalDwtnaZpkCss);
    server->on("/app/immutable/assets/start.DWTNAZpk.css", HTTP_GET, serveAppImmutableAssetsStartDwtnaZpkCss);
    server->on("/app/immutable/entry/app.XEfPTAjy.js", HTTP_GET, serveAppImmutableEntryAppXEfPtAjyJs);
    server->on("/app/immutable/entry/start.RcWUI9I4.js", HTTP_GET, serveAppImmutableEntryStartRcWui9I4Js);
    server->on("/app/immutable/nodes/0.CbGr49Zg.js", HTTP_GET, serveAppImmutableNodes_0CbGr49ZgJs);
    server->on("/app/immutable/nodes/1.BLGIPFcW.js", HTTP_GET, serveAppImmutableNodes_1BlgipFcWJs);
    server->on("/app/immutable/nodes/2.CRJ-m45R.js", HTTP_GET, serveAppImmutableNodes_2CrjM45RJs);
    server->on("/app/immutable/nodes/3.CqDxICP_.js", HTTP_GET, serveAppImmutableNodes_3CqDxIcpJs);
    server->on("/app/immutable/nodes/4.CWtk2m-H.js", HTTP_GET, serveAppImmutableNodes_4CWtk2mHJs);
    server->on("/app/immutable/nodes/5.dHKu998v.js", HTTP_GET, serveAppImmutableNodes_5DHKu998vJs);
    server->on("/app/immutable/nodes/6.GUNcmSDg.js", HTTP_GET, serveAppImmutableNodes_6GuNcmSDgJs);
    server->on("/app/immutable/nodes/7.Ds9PhBnQ.js", HTTP_GET, serveAppImmutableNodes_7Ds9PhBnQJs);
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
