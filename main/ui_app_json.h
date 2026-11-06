/*
 * Binary arrays for the Web UI ui_app_json files.
 */

#pragma once
#include "HttpStatic.h"

// app/version.json
const uint32_t APP_VERSION_JSON_L = 26;
const uint8_t APP_VERSION_JSON[] = {
  0x7b, 0x22, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x22, 0x3a, 0x22, 0x34, 0x31, 0x38, 0x64,
  0x66, 0x39, 0x36, 0x62, 0x65, 0x61, 0x66, 0x33, 0x22, 0x7d
};

inline esp_err_t serveAppVersionJson(httpd_req_t* req) {
  return HttpWebServer::serveStatic(req, "application/json", APP_VERSION_JSON, APP_VERSION_JSON_L, false, false);
}

