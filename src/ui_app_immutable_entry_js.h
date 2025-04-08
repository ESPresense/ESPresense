/*
 * Binary arrays for the Web UI ui_app_immutable_entry_js files.
 */

#pragma once
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

// app/immutable/entry/app.B3OJyQIt.js
const uint16_t APP_IMMUTABLE_ENTRY_APP_B3OJYQIT_JS_L = 162;
const uint8_t APP_IMMUTABLE_ENTRY_APP_B3OJYQIT_JS[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x44, 0x4e, 0x35, 0x16, 0x02, 0x41,
  0x0c, 0x3d, 0x0a, 0x52, 0x07, 0xab, 0xe9, 0xf1, 0x03, 0xd0, 0xf1, 0xc6, 0x78, 0xeb, 0x61, 0x26,
  0xc1, 0xe1, 0xee, 0x90, 0x0d, 0x52, 0x45, 0xbe, 0xe6, 0xf5, 0x01, 0x13, 0xdf, 0x43, 0xd7, 0x50,
  0x17, 0xc1, 0xca, 0x08, 0xe0, 0x65, 0x24, 0x70, 0x32, 0x3c, 0x64, 0x32, 0x1c, 0xd4, 0x32, 0x32,
  0x68, 0x64, 0x30, 0x24, 0x19, 0x35, 0x18, 0x19, 0xcd, 0x73, 0x9f, 0xb0, 0xee, 0x0f, 0x87, 0x23,
  0x97, 0x1d, 0x9b, 0x92, 0x46, 0x8b, 0x0d, 0x2f, 0x6f, 0x93, 0xf9, 0x76, 0x58, 0x50, 0x7f, 0x1a,
  0x2e, 0x6d, 0x02, 0xb6, 0x66, 0xc1, 0xa1, 0x0f, 0x10, 0xfe, 0x7b, 0x22, 0xb5, 0xf2, 0xb9, 0xe3,
  0x1c, 0x1b, 0x93, 0xae, 0x9a, 0x9e, 0x19, 0xca, 0xb4, 0x40, 0x86, 0x58, 0x92, 0x96, 0xa8, 0x0d,
  0xbb, 0x4c, 0x24, 0xdc, 0xc6, 0xbe, 0xe5, 0xa4, 0xb5, 0x12, 0x22, 0x6b, 0x33, 0x0a, 0xe9, 0x14,
  0xd2, 0xae, 0x42, 0xe3, 0xe9, 0xf9, 0x9a, 0x35, 0x17, 0x00, 0xf2, 0xd9, 0x53, 0x39, 0xdf, 0x00,
  0x00, 0x00
};

inline void serveAppImmutableEntryAppB3OJyQItJs(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", APP_IMMUTABLE_ENTRY_APP_B3OJYQIT_JS, APP_IMMUTABLE_ENTRY_APP_B3OJYQIT_JS_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// app/immutable/entry/start._Bm_RwaV.js
const uint16_t APP_IMMUTABLE_ENTRY_START__BM_RWAV_JS_L = 83;
const uint8_t APP_IMMUTABLE_ENTRY_START__BM_RWAV_JS[] PROGMEM = {
  0x69, 0x6d, 0x70, 0x6f, 0x72, 0x74, 0x7b, 0x6c, 0x20, 0x61, 0x73, 0x20, 0x6f, 0x2c, 0x73, 0x20,
  0x61, 0x73, 0x20, 0x72, 0x7d, 0x66, 0x72, 0x6f, 0x6d, 0x22, 0x2e, 0x2e, 0x2f, 0x63, 0x68, 0x75,
  0x6e, 0x6b, 0x73, 0x2f, 0x48, 0x4c, 0x74, 0x49, 0x7a, 0x31, 0x47, 0x58, 0x2e, 0x6a, 0x73, 0x22,
  0x3b, 0x65, 0x78, 0x70, 0x6f, 0x72, 0x74, 0x7b, 0x6f, 0x20, 0x61, 0x73, 0x20, 0x6c, 0x6f, 0x61,
  0x64, 0x5f, 0x63, 0x73, 0x73, 0x2c, 0x72, 0x20, 0x61, 0x73, 0x20, 0x73, 0x74, 0x61, 0x72, 0x74,
  0x7d, 0x3b, 0x0a
};

inline void serveAppImmutableEntryStartBmRwaVJs(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", APP_IMMUTABLE_ENTRY_START__BM_RWAV_JS, APP_IMMUTABLE_ENTRY_START__BM_RWAV_JS_L);
  request->send(response);
}

