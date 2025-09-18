/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,361 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 48,652 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 63,228 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.CNjsDNpd.css", HTTP_GET, serveAppImmutableAssetsInternalCNjsDNpdCss);
    server->on("/app/immutable/assets/start.BSuysxAz.css", HTTP_GET, serveAppImmutableAssetsStartBSuysxAzCss);
    server->on("/app/immutable/entry/app.BkTeeMLn.js", HTTP_GET, serveAppImmutableEntryAppBkTeeMLnJs);
    server->on("/app/immutable/entry/start.D35vfqWt.js", HTTP_GET, serveAppImmutableEntryStartD35vfqWtJs);
    server->on("/app/immutable/nodes/0.vZ7txN8u.js", HTTP_GET, serveAppImmutableNodes_0VZ7txN8uJs);
    server->on("/app/immutable/nodes/1.CtNlw0f7.js", HTTP_GET, serveAppImmutableNodes_1CtNlw0f7Js);
    server->on("/app/immutable/nodes/2.yKaP6LZI.js", HTTP_GET, serveAppImmutableNodes_2YKaP6LziJs);
    server->on("/app/immutable/nodes/3.DLfkJV-v.js", HTTP_GET, serveAppImmutableNodes_3DLfkJvVJs);
    server->on("/app/immutable/nodes/4.Ca6VrJod.js", HTTP_GET, serveAppImmutableNodes_4Ca6VrJodJs);
    server->on("/app/immutable/nodes/5.BBhQNA4D.js", HTTP_GET, serveAppImmutableNodes_5BBhQna4DJs);
    server->on("/app/immutable/nodes/6.FC9JyOa-.js", HTTP_GET, serveAppImmutableNodes_6Fc9JyOaJs);
    server->on("/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes
    server->on("/devices", HTTP_GET, serveDevicesHtml);
    server->on("/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/", HTTP_GET, serveIndexHtml);
    server->on("/network", HTTP_GET, serveNetworkHtml);
    server->on("/network.html", HTTP_GET, serveNetworkHtml);
    server->on("/settings", HTTP_GET, serveSettingsHtml);
    server->on("/settings.html", HTTP_GET, serveSettingsHtml);
}
