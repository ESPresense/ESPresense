/*
 * Web UI Routes
 *
 * Compressed Size Summary:
 * ui_svg: 456 bytes
 * Total: 456 bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
#include "ui_svg.h"

inline void setupRoutes(AsyncWebServer* server) {
    server->on("/ui/favicon.svg", HTTP_GET, serveFaviconSvg);

    // HTML routes

}