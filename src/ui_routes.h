#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_app_immutable_chunks_js.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_html.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/ui/app/immutable/assets/index.DUGlLD2F.css", HTTP_GET, serveAppImmutableAssetsIndexDuGlLd2FCss);
    server->on("/ui/app/immutable/chunks/index.mFKdhD8C.js", HTTP_GET, serveAppImmutableChunksIndexMFKdhD8CJs);
    server->on("/ui/app/immutable/entry/start.DzDNF7x0.js", HTTP_GET, serveAppImmutableEntryStartDzDnf7x0Js);
    server->on("/ui/app/immutable/entry/app.B9Y0-_K7.js", HTTP_GET, serveAppImmutableEntryAppB9Y0K7Js);
    server->on("/ui/app/immutable/nodes/0.BUpjl-S3.js", HTTP_GET, serveAppImmutableNodes_0BUpjlS3Js);
    server->on("/ui/app/immutable/nodes/1.KgnaUiKZ.js", HTTP_GET, serveAppImmutableNodes_1KgnaUiKzJs);
    server->on("/ui/app/immutable/nodes/2.DRTs-OeU.js", HTTP_GET, serveAppImmutableNodes_2DrTsOeUJs);
    server->on("/ui/app/immutable/nodes/3.DAisRa9z.js", HTTP_GET, serveAppImmutableNodes_3DAisRa9zJs);
    server->on("/ui/app/immutable/nodes/4.pU4Xl9Mg.js", HTTP_GET, serveAppImmutableNodes_4PU4Xl9MgJs);
    server->on("/ui/app/immutable/nodes/5.9ec-vGtq.js", HTTP_GET, serveAppImmutableNodes_5_9ecVGtqJs);
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