/*
 * Binary arrays for the Web UI ui_app_immutable_entry_js files.
 */

#pragma once
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

// app/immutable/entry/app.CAE7mPt8.js
const uint16_t APP_IMMUTABLE_ENTRY_APP_CAE7MPT8_JS_L = 163;
const uint8_t APP_IMMUTABLE_ENTRY_APP_CAE7MPT8_JS[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x45, 0xce, 0x31, 0x0e, 0xc2, 0x30,
  0x0c, 0x05, 0xd0, 0x9d, 0x53, 0x54, 0x9d, 0xad, 0x56, 0x2c, 0x2c, 0x1d, 0x61, 0xe0, 0x0c, 0x2c,
  0xc8, 0x24, 0xae, 0x5c, 0x4a, 0x62, 0x64, 0x07, 0x04, 0x42, 0xbd, 0x3b, 0xb8, 0x19, 0x98, 0x9e,
  0x17, 0xff, 0xff, 0xa7, 0x74, 0x17, 0x2d, 0x1f, 0x6a, 0xd0, 0x1a, 0x81, 0x8b, 0x43, 0x10, 0x1d,
  0x85, 0xe0, 0x44, 0x60, 0x27, 0x40, 0x72, 0x18, 0xb2, 0x53, 0x40, 0x9d, 0x04, 0xe8, 0xe4, 0x65,
  0x54, 0x49, 0x6d, 0xd7, 0xf5, 0x81, 0x1f, 0x79, 0xb6, 0x7e, 0x8f, 0xc7, 0xc3, 0x76, 0x77, 0x1a,
  0xbb, 0xab, 0xb5, 0x03, 0xbd, 0xd6, 0x06, 0x59, 0xc3, 0x28, 0x48, 0x24, 0xa0, 0xff, 0xad, 0x56,
  0xa3, 0xe2, 0x14, 0xca, 0x24, 0x19, 0xf5, 0x5d, 0xdb, 0x19, 0x8d, 0xeb, 0x00, 0x16, 0x99, 0xad,
  0x8e, 0x48, 0x58, 0x02, 0xfb, 0x4b, 0x59, 0x6b, 0x7f, 0xef, 0x56, 0x67, 0xa9, 0x48, 0xa9, 0xcb,
  0x8c, 0xf4, 0x49, 0x7a, 0xbe, 0x09, 0x46, 0x5b, 0x86, 0xcd, 0x17, 0x5a, 0x4d, 0xfb, 0x21, 0xdf,
  0x00, 0x00, 0x00
};

inline void serveAppImmutableEntryAppCae7mPt8Js(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", APP_IMMUTABLE_ENTRY_APP_CAE7MPT8_JS, APP_IMMUTABLE_ENTRY_APP_CAE7MPT8_JS_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// app/immutable/entry/start.jOu6qxvN.js
const uint16_t APP_IMMUTABLE_ENTRY_START_JOU6QXVN_JS_L = 83;
const uint8_t APP_IMMUTABLE_ENTRY_START_JOU6QXVN_JS[] PROGMEM = {
  0x69, 0x6d, 0x70, 0x6f, 0x72, 0x74, 0x7b, 0x6c, 0x20, 0x61, 0x73, 0x20, 0x6f, 0x2c, 0x73, 0x20,
  0x61, 0x73, 0x20, 0x72, 0x7d, 0x66, 0x72, 0x6f, 0x6d, 0x22, 0x2e, 0x2e, 0x2f, 0x63, 0x68, 0x75,
  0x6e, 0x6b, 0x73, 0x2f, 0x43, 0x61, 0x48, 0x44, 0x31, 0x36, 0x5a, 0x66, 0x2e, 0x6a, 0x73, 0x22,
  0x3b, 0x65, 0x78, 0x70, 0x6f, 0x72, 0x74, 0x7b, 0x6f, 0x20, 0x61, 0x73, 0x20, 0x6c, 0x6f, 0x61,
  0x64, 0x5f, 0x63, 0x73, 0x73, 0x2c, 0x72, 0x20, 0x61, 0x73, 0x20, 0x73, 0x74, 0x61, 0x72, 0x74,
  0x7d, 0x3b, 0x0a
};

inline void serveAppImmutableEntryStartJOu6qxvNJs(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", APP_IMMUTABLE_ENTRY_START_JOU6QXVN_JS, APP_IMMUTABLE_ENTRY_START_JOU6QXVN_JS_L);
  request->send(response);
}

