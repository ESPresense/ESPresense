/*
 * Binary arrays for the Web UI ui_app_immutable_entry_js files.
 */

#pragma once
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

// app/immutable/entry/app.C2sOnQk-.js
const uint16_t APP_IMMUTABLE_ENTRY_APP_C2SONQK__JS_L = 161;
const uint8_t APP_IMMUTABLE_ENTRY_APP_C2SONQK__JS[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x45, 0xce, 0x31, 0x0e, 0xc2, 0x30,
  0x0c, 0x05, 0xd0, 0x9d, 0x53, 0x54, 0x9d, 0xad, 0x76, 0xae, 0x3a, 0x21, 0x6e, 0xc1, 0x82, 0x42,
  0x62, 0xe4, 0x50, 0x12, 0x23, 0x3b, 0x20, 0x10, 0xea, 0xdd, 0xc1, 0xf5, 0xc0, 0xf4, 0xbc, 0xf8,
  0xff, 0x9f, 0xcb, 0x9d, 0xa5, 0x7d, 0xb0, 0x0b, 0xda, 0x31, 0x9c, 0x0d, 0x84, 0x64, 0x08, 0x44,
  0x23, 0x01, 0x19, 0x11, 0x8a, 0x41, 0x50, 0x8d, 0x06, 0x62, 0x14, 0x08, 0x46, 0x5d, 0x2f, 0xc2,
  0xa5, 0x1f, 0x86, 0x31, 0xd2, 0xa3, 0x2e, 0x3a, 0x1e, 0x8e, 0x79, 0x4a, 0xd3, 0x3e, 0x0f, 0x57,
  0xed, 0x67, 0x7c, 0x6d, 0x0d, 0xbc, 0x85, 0x61, 0xe4, 0x84, 0x80, 0xff, 0x5b, 0xd4, 0xa3, 0x52,
  0x8e, 0x2d, 0x73, 0x0d, 0xf2, 0xf6, 0x76, 0x0a, 0x4a, 0x3e, 0x80, 0x98, 0x17, 0xf5, 0x11, 0x25,
  0xb4, 0x48, 0xf6, 0xd2, 0xb6, 0xda, 0xdf, 0xbb, 0xfa, 0x2c, 0x61, 0x6e, 0xbe, 0x4c, 0x51, 0x9e,
  0x28, 0xa7, 0x1b, 0x87, 0xa4, 0xeb, 0xbc, 0xfb, 0x02, 0x42, 0xfa, 0x94, 0x9e, 0xdf, 0x00, 0x00,
  0x00
};

inline void serveAppImmutableEntryAppC2sOnQkJs(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", APP_IMMUTABLE_ENTRY_APP_C2SONQK__JS, APP_IMMUTABLE_ENTRY_APP_C2SONQK__JS_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// app/immutable/entry/start.BMo8JMNl.js
const uint16_t APP_IMMUTABLE_ENTRY_START_BMO8JMNL_JS_L = 83;
const uint8_t APP_IMMUTABLE_ENTRY_START_BMO8JMNL_JS[] PROGMEM = {
  0x69, 0x6d, 0x70, 0x6f, 0x72, 0x74, 0x7b, 0x6c, 0x20, 0x61, 0x73, 0x20, 0x6f, 0x2c, 0x73, 0x20,
  0x61, 0x73, 0x20, 0x72, 0x7d, 0x66, 0x72, 0x6f, 0x6d, 0x22, 0x2e, 0x2e, 0x2f, 0x63, 0x68, 0x75,
  0x6e, 0x6b, 0x73, 0x2f, 0x43, 0x5a, 0x69, 0x39, 0x64, 0x39, 0x41, 0x69, 0x2e, 0x6a, 0x73, 0x22,
  0x3b, 0x65, 0x78, 0x70, 0x6f, 0x72, 0x74, 0x7b, 0x6f, 0x20, 0x61, 0x73, 0x20, 0x6c, 0x6f, 0x61,
  0x64, 0x5f, 0x63, 0x73, 0x73, 0x2c, 0x72, 0x20, 0x61, 0x73, 0x20, 0x73, 0x74, 0x61, 0x72, 0x74,
  0x7d, 0x3b, 0x0a
};

inline void serveAppImmutableEntryStartBMo8JmNlJs(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", APP_IMMUTABLE_ENTRY_START_BMO8JMNL_JS, APP_IMMUTABLE_ENTRY_START_BMO8JMNL_JS_L);
  request->send(response);
}

