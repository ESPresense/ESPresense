/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,373 bytes
 * ui_html: 3,250 bytes
 * ui_app_immutable_entry_js: 48,491 bytes
 * ui_app_immutable_nodes_js: 514 bytes
 * ui_svg: 456 bytes
 * Total: 63,084 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.DIdbUeTM.css", HTTP_GET, serveAppImmutableAssetsInternalDIdbUeTmCss);
    server->on("/app/immutable/assets/start.CtRfqZfF.css", HTTP_GET, serveAppImmutableAssetsStartCtRfqZfFCss);
    server->on("/app/immutable/entry/app.XkFQzAtR.js", HTTP_GET, serveAppImmutableEntryAppXkFQzAtRJs);
    server->on("/app/immutable/entry/start.gaOvcZcu.js", HTTP_GET, serveAppImmutableEntryStartGaOvcZcuJs);
    server->on("/app/immutable/nodes/0.D7TObsub.js", HTTP_GET, serveAppImmutableNodes_0D7TObsubJs);
    server->on("/app/immutable/nodes/1.BuWyvcNG.js", HTTP_GET, serveAppImmutableNodes_1BuWyvcNgJs);
    server->on("/app/immutable/nodes/2.B_M66uA-.js", HTTP_GET, serveAppImmutableNodes_2BM66uAJs);
    server->on("/app/immutable/nodes/3.BzO43ys4.js", HTTP_GET, serveAppImmutableNodes_3BzO43ys4Js);
    server->on("/app/immutable/nodes/4.C08CM-AP.js", HTTP_GET, serveAppImmutableNodes_4C08CmApJs);
    server->on("/app/immutable/nodes/5.CNAMgstq.js", HTTP_GET, serveAppImmutableNodes_5CnaMgstqJs);
    server->on("/app/immutable/nodes/6.DhUtc4XE.js", HTTP_GET, serveAppImmutableNodes_6DhUtc4XeJs);
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
