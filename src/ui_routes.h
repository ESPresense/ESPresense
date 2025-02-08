/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 7,762 bytes
 * ui_html: 2,736 bytes
 * ui_app_immutable_chunks_js: 41,073 bytes
 * ui_app_immutable_entry_js: 222 bytes
 * ui_app_immutable_nodes_js: 418 bytes
 * ui_svg: 456 bytes
 * Total: 52,667 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_chunks_js.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/ui/app/immutable/assets/index.Dwd10Ur_.css", HTTP_GET, serveAppImmutableAssetsIndexDwd10UrCss);
    server->on("/ui/app/immutable/assets/index.B4D-OeO4.css", HTTP_GET, serveAppImmutableAssetsIndexB4DOeO4Css);
    server->on("/ui/app/immutable/chunks/CwmgqbGo.js", HTTP_GET, serveAppImmutableChunksCwmgqbGoJs);
    server->on("/ui/app/immutable/entry/app.DY92gEiD.js", HTTP_GET, serveAppImmutableEntryAppDy92gEiDJs);
    server->on("/ui/app/immutable/entry/start.BIi5kmWf.js", HTTP_GET, serveAppImmutableEntryStartBIi5kmWfJs);
    server->on("/ui/app/immutable/nodes/0.CiducD5Y.js", HTTP_GET, serveAppImmutableNodes_0CiducD5YJs);
    server->on("/ui/app/immutable/nodes/1.LeT5mEKL.js", HTTP_GET, serveAppImmutableNodes_1LeT5mEklJs);
    server->on("/ui/app/immutable/nodes/2.CJ7Wws6y.js", HTTP_GET, serveAppImmutableNodes_2Cj7Wws6yJs);
    server->on("/ui/app/immutable/nodes/3.BNlM2Uq7.js", HTTP_GET, serveAppImmutableNodes_3BNlM2Uq7Js);
    server->on("/ui/app/immutable/nodes/4.Ck3oJlhj.js", HTTP_GET, serveAppImmutableNodes_4Ck3oJlhjJs);
    server->on("/ui/app/immutable/nodes/5.DeSM9F6v.js", HTTP_GET, serveAppImmutableNodes_5DeSm9F6vJs);
    server->on("/ui/favicon.svg", HTTP_GET, serveFaviconSvg);
    // HTML routes
    server->on("/ui/devices", HTTP_GET, serveDevicesHtml);
    server->on("/ui/devices.html", HTTP_GET, serveDevicesHtml);
    server->on("/ui/fingerprints", HTTP_GET, serveFingerprintsHtml);
    server->on("/ui/fingerprints.html", HTTP_GET, serveFingerprintsHtml);
    server->on("/ui/", HTTP_GET, serveIndexHtml);
    server->on("/ui/settings", HTTP_GET, serveSettingsHtml);
    server->on("/ui/settings.html", HTTP_GET, serveSettingsHtml);
}
