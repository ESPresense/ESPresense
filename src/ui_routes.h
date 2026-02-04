/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 24,202 bytes
 * ui_html: 3,882 bytes
 * ui_app_immutable_entry_js: 62,539 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 91,669 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.BRhgnC6i.css", HTTP_GET, serveAppImmutableAssetsInternalBRhgnC6iCss);
    server->on("/app/immutable/assets/start.BRhgnC6i.css", HTTP_GET, serveAppImmutableAssetsStartBRhgnC6iCss);
    server->on("/app/immutable/entry/app.BMktPJYp.js", HTTP_GET, serveAppImmutableEntryAppBMktPjYpJs);
    server->on("/app/immutable/entry/start.BNdAIZ_m.js", HTTP_GET, serveAppImmutableEntryStartBNdAizMJs);
    server->on("/app/immutable/nodes/0.B8jHbqFZ.js", HTTP_GET, serveAppImmutableNodes_0B8jHbqFzJs);
    server->on("/app/immutable/nodes/1.C08gdRHO.js", HTTP_GET, serveAppImmutableNodes_1C08gdRhoJs);
    server->on("/app/immutable/nodes/2.oiemako7.js", HTTP_GET, serveAppImmutableNodes_2Oiemako7Js);
    server->on("/app/immutable/nodes/3.B-g-EUCT.js", HTTP_GET, serveAppImmutableNodes_3BGEuctJs);
    server->on("/app/immutable/nodes/4.Tbo6EU61.js", HTTP_GET, serveAppImmutableNodes_4Tbo6Eu61Js);
    server->on("/app/immutable/nodes/5.BGFTmaVb.js", HTTP_GET, serveAppImmutableNodes_5BgfTmaVbJs);
    server->on("/app/immutable/nodes/6.mzrJkenK.js", HTTP_GET, serveAppImmutableNodes_6MzrJkenKJs);
    server->on("/app/immutable/nodes/7.CCqSW1tO.js", HTTP_GET, serveAppImmutableNodes_7CCqSw1tOJs);
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
