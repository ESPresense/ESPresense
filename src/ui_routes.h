/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,210 bytes
 * ui_html: 3,245 bytes
 * ui_app_immutable_entry_js: 44,977 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 59,402 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.R2nWuKlp.css", HTTP_GET, serveAppImmutableAssetsInternalR2nWuKlpCss);
    server->on("/app/immutable/assets/start.CquYjHqP.css", HTTP_GET, serveAppImmutableAssetsStartCquYjHqPCss);
    server->on("/app/immutable/entry/app.B9amyh02.js", HTTP_GET, serveAppImmutableEntryAppB9amyh02Js);
    server->on("/app/immutable/entry/start.tjxdw8Su.js", HTTP_GET, serveAppImmutableEntryStartTjxdw8SuJs);
    server->on("/app/immutable/nodes/0.DMUJYMmc.js", HTTP_GET, serveAppImmutableNodes_0DmujyMmcJs);
    server->on("/app/immutable/nodes/1.2TePMf_t.js", HTTP_GET, serveAppImmutableNodes_1_2TePMfTJs);
    server->on("/app/immutable/nodes/2.BWFJlq8Y.js", HTTP_GET, serveAppImmutableNodes_2BwfJlq8YJs);
    server->on("/app/immutable/nodes/3.Bh-vACw8.js", HTTP_GET, serveAppImmutableNodes_3BhVACw8Js);
    server->on("/app/immutable/nodes/4.q-7LWJzR.js", HTTP_GET, serveAppImmutableNodes_4Q_7LwJzRJs);
    server->on("/app/immutable/nodes/5.CRIJdgRV.js", HTTP_GET, serveAppImmutableNodes_5CriJdgRvJs);
    server->on("/app/immutable/nodes/6.D5kISpR9.js", HTTP_GET, serveAppImmutableNodes_6D5kISpR9Js);
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
