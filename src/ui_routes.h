/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 8,873 bytes
 * ui_html: 3,360 bytes
 * ui_app_immutable_chunks_js: 44,630 bytes
 * ui_app_immutable_entry_js: 243 bytes
 * ui_app_immutable_nodes_js: 484 bytes
 * ui_svg: 456 bytes
 * Total: 58,046 bytes
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
    server->on("/app/immutable/assets/index.DN2mWWgh.css", HTTP_GET, serveAppImmutableAssetsIndexDn2mWWghCss);
    server->on("/app/immutable/assets/index.BfkQNPT2.css", HTTP_GET, serveAppImmutableAssetsIndexBfkQnpt2Css);
    server->on("/app/immutable/chunks/CX-eomk7.js", HTTP_GET, serveAppImmutableChunksCxEomk7Js);
    server->on("/app/immutable/entry/app.C_7RhmCk.js", HTTP_GET, serveAppImmutableEntryAppC_7RhmCkJs);
    server->on("/app/immutable/entry/start.N1FTwjEN.js", HTTP_GET, serveAppImmutableEntryStartN1FTwjEnJs);
    server->on("/app/immutable/nodes/0.u7GdL-Ni.js", HTTP_GET, serveAppImmutableNodes_0U7GdLNiJs);
    server->on("/app/immutable/nodes/1.BY3yRJ0O.js", HTTP_GET, serveAppImmutableNodes_1By3yRj0OJs);
    server->on("/app/immutable/nodes/2.DxloyQ2Z.js", HTTP_GET, serveAppImmutableNodes_2DxloyQ2ZJs);
    server->on("/app/immutable/nodes/3.DjiChRum.js", HTTP_GET, serveAppImmutableNodes_3DjiChRumJs);
    server->on("/app/immutable/nodes/4.D95UqARn.js", HTTP_GET, serveAppImmutableNodes_4D95UqARnJs);
    server->on("/app/immutable/nodes/5.BFgJMIR4.js", HTTP_GET, serveAppImmutableNodes_5BFgJmir4Js);
    server->on("/app/immutable/nodes/6.DjwWMFkU.js", HTTP_GET, serveAppImmutableNodes_6DjwWmFkUJs);
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
