/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,210 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 45,336 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 59,766 bytes
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
    server->on("/app/immutable/entry/app.ftxQMfzn.js", HTTP_GET, serveAppImmutableEntryAppFtxQMfznJs);
    server->on("/app/immutable/entry/start.DGHtOsDr.js", HTTP_GET, serveAppImmutableEntryStartDgHtOsDrJs);
    server->on("/app/immutable/nodes/0.D5IeBcFq.js", HTTP_GET, serveAppImmutableNodes_0D5IeBcFqJs);
    server->on("/app/immutable/nodes/1.BsZUBqFo.js", HTTP_GET, serveAppImmutableNodes_1BsZuBqFoJs);
    server->on("/app/immutable/nodes/2.EnRog6wJ.js", HTTP_GET, serveAppImmutableNodes_2EnRog6wJJs);
    server->on("/app/immutable/nodes/3.C7eAHwXo.js", HTTP_GET, serveAppImmutableNodes_3C7eAHwXoJs);
    server->on("/app/immutable/nodes/4.DrK65Isw.js", HTTP_GET, serveAppImmutableNodes_4DrK65IswJs);
    server->on("/app/immutable/nodes/5.TszwZ4T3.js", HTTP_GET, serveAppImmutableNodes_5TszwZ4T3Js);
    server->on("/app/immutable/nodes/6.DfyAYEG_.js", HTTP_GET, serveAppImmutableNodes_6DfyAyegJs);
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
