/*
 * Binary arrays for the Web UI ui_app_immutable_entry_js files.
 */

#pragma once
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

// app/immutable/entry/app.CIMDWf5R.js
const uint16_t APP_IMMUTABLE_ENTRY_APP_CIMDWF5R_JS_L = 160;
const uint8_t APP_IMMUTABLE_ENTRY_APP_CIMDWF5R_JS[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x45, 0x4e, 0x35, 0x12, 0x02, 0x51,
  0x0c, 0xed, 0xb9, 0x04, 0x52, 0x67, 0xa5, 0xdf, 0x0a, 0xbd, 0x03, 0x15, 0xf3, 0x8d, 0x59, 0x0f,
  0x93, 0x7c, 0x1c, 0xee, 0x0e, 0xd9, 0x20, 0x55, 0xe4, 0x69, 0xd5, 0x1d, 0x90, 0xe2, 0x3d, 0x4c,
  0x0c, 0x4f, 0x10, 0xac, 0x8c, 0x00, 0x5e, 0x06, 0x81, 0x93, 0xe1, 0xa1, 0x94, 0xe1, 0xa0, 0x93,
  0x51, 0x42, 0x2f, 0x23, 0x02, 0xc9, 0xe8, 0xc0, 0xc8, 0xe8, 0x9f, 0x7b, 0xc2, 0x6e, 0x96, 0xa6,
  0x99, 0x2b, 0x8f, 0x7d, 0xc3, 0xd9, 0x72, 0x63, 0xf3, 0xed, 0x82, 0x97, 0x69, 0xcd, 0xb3, 0x22,
  0x5c, 0x86, 0x04, 0x1c, 0xcc, 0x82, 0x43, 0x1f, 0x20, 0xfc, 0x77, 0x62, 0xb5, 0xf2, 0x95, 0x8b,
  0x15, 0xf6, 0x86, 0xae, 0x9a, 0x5e, 0x1a, 0x2e, 0xb5, 0x40, 0x89, 0xd8, 0xb0, 0x96, 0xe8, 0x4c,
  0x74, 0xa5, 0x48, 0xe2, 0x10, 0xfb, 0x96, 0xb3, 0xd6, 0x22, 0xc4, 0xa8, 0xcd, 0x38, 0xd0, 0x29,
  0xd0, 0xae, 0x45, 0xe3, 0xf9, 0x59, 0x8c, 0x5e, 0x89, 0xee, 0xd4, 0xa6, 0xdf, 0x00, 0x00, 0x00
};

inline void serveAppImmutableEntryAppCimdWf5RJs(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", APP_IMMUTABLE_ENTRY_APP_CIMDWF5R_JS, APP_IMMUTABLE_ENTRY_APP_CIMDWF5R_JS_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// app/immutable/entry/start.7tE2sxOU.js
const uint16_t APP_IMMUTABLE_ENTRY_START_7TE2SXOU_JS_L = 83;
const uint8_t APP_IMMUTABLE_ENTRY_START_7TE2SXOU_JS[] PROGMEM = {
  0x69, 0x6d, 0x70, 0x6f, 0x72, 0x74, 0x7b, 0x6c, 0x20, 0x61, 0x73, 0x20, 0x6f, 0x2c, 0x73, 0x20,
  0x61, 0x73, 0x20, 0x72, 0x7d, 0x66, 0x72, 0x6f, 0x6d, 0x22, 0x2e, 0x2e, 0x2f, 0x63, 0x68, 0x75,
  0x6e, 0x6b, 0x73, 0x2f, 0x43, 0x46, 0x62, 0x30, 0x59, 0x42, 0x73, 0x43, 0x2e, 0x6a, 0x73, 0x22,
  0x3b, 0x65, 0x78, 0x70, 0x6f, 0x72, 0x74, 0x7b, 0x6f, 0x20, 0x61, 0x73, 0x20, 0x6c, 0x6f, 0x61,
  0x64, 0x5f, 0x63, 0x73, 0x73, 0x2c, 0x72, 0x20, 0x61, 0x73, 0x20, 0x73, 0x74, 0x61, 0x72, 0x74,
  0x7d, 0x3b, 0x0a
};

inline void serveAppImmutableEntryStart_7tE2sxOuJs(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", APP_IMMUTABLE_ENTRY_START_7TE2SXOU_JS, APP_IMMUTABLE_ENTRY_START_7TE2SXOU_JS_L);
  request->send(response);
}

