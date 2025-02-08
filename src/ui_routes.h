/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 7,762 bytes
 * ui_html: 2,740 bytes
 * ui_app_immutable_chunks_js: 41,078 bytes
 * ui_app_immutable_entry_js: 224 bytes
 * ui_app_immutable_nodes_js: 418 bytes
 * ui_svg: 456 bytes
 * Total: 52,678 bytes
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
    server->on("/ui/app/immutable/chunks/Dm2QCkzB.js", HTTP_GET, serveAppImmutableChunksDm2QCkzBJs);
    server->on("/ui/app/immutable/entry/app.Cb2VW3WQ.js", HTTP_GET, serveAppImmutableEntryAppCb2Vw3WqJs);
    server->on("/ui/app/immutable/entry/start.7iSqqDB1.js", HTTP_GET, serveAppImmutableEntryStart_7iSqqDb1Js);
    server->on("/ui/app/immutable/nodes/0.mRmfOCQ2.js", HTTP_GET, serveAppImmutableNodes_0MRmfOcq2Js);
    server->on("/ui/app/immutable/nodes/1.0EiJ79V4.js", HTTP_GET, serveAppImmutableNodes_1_0EiJ79V4Js);
    server->on("/ui/app/immutable/nodes/2.Bxme43Dr.js", HTTP_GET, serveAppImmutableNodes_2Bxme43DrJs);
    server->on("/ui/app/immutable/nodes/3.CR6EXnD7.js", HTTP_GET, serveAppImmutableNodes_3Cr6EXnD7Js);
    server->on("/ui/app/immutable/nodes/4.DrdVjXcz.js", HTTP_GET, serveAppImmutableNodes_4DrdVjXczJs);
    server->on("/ui/app/immutable/nodes/5.BXsMqBj-.js", HTTP_GET, serveAppImmutableNodes_5BXsMqBjJs);
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
