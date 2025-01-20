/*
 * Binary arrays for the Web UI ui_app_immutable_entry_js files.
 */

#pragma once
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

// app/immutable/entry/start.B0eCyNH_.js
const uint16_t APP_IMMUTABLE_ENTRY_START_B0ECYNH__JS_L = 62;
const uint8_t APP_IMMUTABLE_ENTRY_START_B0ECYNH__JS[] PROGMEM = {
  0x69, 0x6d, 0x70, 0x6f, 0x72, 0x74, 0x7b, 0x73, 0x20, 0x61, 0x73, 0x20, 0x74, 0x7d, 0x66, 0x72,
  0x6f, 0x6d, 0x22, 0x2e, 0x2e, 0x2f, 0x63, 0x68, 0x75, 0x6e, 0x6b, 0x73, 0x2f, 0x42, 0x6f, 0x68,
  0x71, 0x41, 0x50, 0x75, 0x66, 0x2e, 0x6a, 0x73, 0x22, 0x3b, 0x65, 0x78, 0x70, 0x6f, 0x72, 0x74,
  0x7b, 0x74, 0x20, 0x61, 0x73, 0x20, 0x73, 0x74, 0x61, 0x72, 0x74, 0x7d, 0x3b, 0x0a
};

inline void serveAppImmutableEntryStartB0eCyNhJs(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", APP_IMMUTABLE_ENTRY_START_B0ECYNH__JS, APP_IMMUTABLE_ENTRY_START_B0ECYNH__JS_L);
  request->send(response);
}

// app/immutable/entry/app.CLf4gXvV.js
const uint16_t APP_IMMUTABLE_ENTRY_APP_CLF4GXVV_JS_L = 161;
const uint8_t APP_IMMUTABLE_ENTRY_APP_CLF4GXVV_JS[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x44, 0xce, 0xc1, 0x0d, 0xc2, 0x30,
  0x0c, 0x05, 0xd0, 0x55, 0xaa, 0x9e, 0xad, 0x76, 0x80, 0x9e, 0x60, 0x02, 0x36, 0x40, 0x21, 0x71,
  0xe5, 0x52, 0x12, 0x83, 0x9d, 0x22, 0x10, 0xea, 0xee, 0xe0, 0xf8, 0xc0, 0xe9, 0xf9, 0xe2, 0xff,
  0xff, 0x92, 0xef, 0x2c, 0xf5, 0x83, 0x5d, 0xd0, 0x8e, 0xe1, 0x62, 0x20, 0x24, 0x43, 0x20, 0x1a,
  0x09, 0xc8, 0x88, 0x90, 0x0d, 0x82, 0x62, 0x54, 0x10, 0x23, 0x43, 0x30, 0xca, 0x3e, 0x0b, 0xe7,
  0x7e, 0x18, 0xc6, 0x48, 0x5b, 0x59, 0x75, 0x3c, 0x32, 0x3d, 0x0e, 0xa7, 0x6d, 0x1e, 0xae, 0xda,
  0x4f, 0xf8, 0x6a, 0x0d, 0xdc, 0xc2, 0x30, 0x72, 0x42, 0xc0, 0xff, 0x2d, 0xea, 0x51, 0x69, 0x89,
  0x75, 0xe1, 0x12, 0xe4, 0xed, 0xed, 0x14, 0x94, 0x7c, 0x00, 0x31, 0xaf, 0xea, 0x23, 0x72, 0xa8,
  0x91, 0xec, 0xa5, 0xb6, 0xda, 0xdf, 0xbb, 0xfa, 0x2c, 0x61, 0xae, 0xbe, 0x4c, 0x51, 0x9e, 0x28,
  0xe7, 0x1b, 0x87, 0xa4, 0xfb, 0xf4, 0x1d, 0x17, 0x00, 0x92, 0x69, 0xca, 0x54, 0xdf, 0x00, 0x00,
  0x00
};

inline void serveAppImmutableEntryAppCLf4gXvVJs(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", APP_IMMUTABLE_ENTRY_APP_CLF4GXVV_JS, APP_IMMUTABLE_ENTRY_APP_CLF4GXVV_JS_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

