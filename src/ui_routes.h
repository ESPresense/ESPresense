/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,876 bytes
 * ui_html: 4,320 bytes
 * ui_app_immutable_entry_js: 63,747 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 93,989 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsInternalDAt95k88Css);
    server->on("/app/immutable/assets/start.DAt95k88.css", HTTP_GET, serveAppImmutableAssetsStartDAt95k88Css);
    server->on("/app/immutable/entry/app.DDo1Orhy.js", HTTP_GET, serveAppImmutableEntryAppDDo1OrhyJs);
    server->on("/app/immutable/entry/start.BzbK-C-u.js", HTTP_GET, serveAppImmutableEntryStartBzbKCUJs);
    server->on("/app/immutable/nodes/0.DX8aetBS.js", HTTP_GET, serveAppImmutableNodes_0Dx8aetBsJs);
    server->on("/app/immutable/nodes/1.CLU86Lyr.js", HTTP_GET, serveAppImmutableNodes_1Clu86LyrJs);
    server->on("/app/immutable/nodes/2.Ba4CJkaz.js", HTTP_GET, serveAppImmutableNodes_2Ba4CJkazJs);
    server->on("/app/immutable/nodes/3.CvmsXxVy.js", HTTP_GET, serveAppImmutableNodes_3CvmsXxVyJs);
    server->on("/app/immutable/nodes/4.CzHyv4Nq.js", HTTP_GET, serveAppImmutableNodes_4CzHyv4NqJs);
    server->on("/app/immutable/nodes/5.SHSUK5kK.js", HTTP_GET, serveAppImmutableNodes_5Shsuk5kKJs);
    server->on("/app/immutable/nodes/6.B7OvUAfk.js", HTTP_GET, serveAppImmutableNodes_6B7OvUAfkJs);
    server->on("/app/immutable/nodes/7.BelIvJx2.js", HTTP_GET, serveAppImmutableNodes_7BelIvJx2Js);
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
