/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_app_immutable_assets_css: 10,410 bytes
 * ui_html: 3,900 bytes
 * ui_app_immutable_entry_js: 66,734 bytes
 * ui_app_immutable_nodes_js: 590 bytes
 * ui_svg: 456 bytes
 * Total: 82,090 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_app_immutable_assets_css.h"
#include "ui_html.h"
#include "ui_app_immutable_entry_js.h"
#include "ui_app_immutable_nodes_js.h"
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/app/immutable/assets/internal.VjTAGEOH.css", HTTP_GET, serveAppImmutableAssetsInternalVjTageohCss);
    server->on("/app/immutable/assets/start.Bt_nQDmA.css", HTTP_GET, serveAppImmutableAssetsStartBtNQDmACss);
    server->on("/app/immutable/entry/app.I3Sg2Hfk.js", HTTP_GET, serveAppImmutableEntryAppI3Sg2HfkJs);
    server->on("/app/immutable/entry/start.kLAQblfI.js", HTTP_GET, serveAppImmutableEntryStartKLaQblfIJs);
    server->on("/app/immutable/nodes/0.B3n7mqdf.js", HTTP_GET, serveAppImmutableNodes_0B3n7mqdfJs);
    server->on("/app/immutable/nodes/1.PTb36xMT.js", HTTP_GET, serveAppImmutableNodes_1PTb36xMtJs);
    server->on("/app/immutable/nodes/2.DoXpToi1.js", HTTP_GET, serveAppImmutableNodes_2DoXpToi1Js);
    server->on("/app/immutable/nodes/3.BWdnDKkF.js", HTTP_GET, serveAppImmutableNodes_3BWdnDKkFJs);
    server->on("/app/immutable/nodes/4.pmYetTg4.js", HTTP_GET, serveAppImmutableNodes_4PmYetTg4Js);
    server->on("/app/immutable/nodes/5.CaN1VPOw.js", HTTP_GET, serveAppImmutableNodes_5CaN1VpOwJs);
    server->on("/app/immutable/nodes/6.C4Vb3wee.js", HTTP_GET, serveAppImmutableNodes_6C4Vb3weeJs);
    server->on("/app/immutable/nodes/7.C186DEU-.js", HTTP_GET, serveAppImmutableNodes_7C186DeuJs);
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
