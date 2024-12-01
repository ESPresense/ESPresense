/*
 * Binary arrays for the Web UI ui_app_immutable_nodes_js files.
 * Uses Zopfli compression for optimal size and improved speed.
 */

#pragma once
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

// app/immutable/nodes/0.BUpjl-S3.js
const uint16_t APP_IMMUTABLE_NODES_0_BUPJL_S3_JS_L = 103;
const uint8_t APP_IMMUTABLE_NODES_0_BUPJL_S3_JS[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0xcb, 0xcc, 0x2d, 0xc8, 0x2f, 0x2a,
  0xa9, 0x4e, 0x52, 0x48, 0x2c, 0x56, 0x48, 0xd5, 0x89, 0x07, 0x51, 0x79, 0xb5, 0x69, 0x45, 0xf9,
  0xb9, 0x4a, 0x7a, 0x7a, 0xfa, 0xc9, 0x19, 0xa5, 0x79, 0xd9, 0xc5, 0xfa, 0x99, 0x79, 0x29, 0xa9,
  0x15, 0x7a, 0xb9, 0x6e, 0xde, 0x29, 0x19, 0x2e, 0x16, 0xce, 0x7a, 0x59, 0xc5, 0x4a, 0xd6, 0xa9,
  0x15, 0x60, 0x6d, 0xa9, 0x20, 0xf5, 0xc9, 0xf9, 0x40, 0x33, 0xf2, 0x52, 0xf3, 0x4a, 0x74, 0xf2,
  0x40, 0xdc, 0xd2, 0xbc, 0xcc, 0xb2, 0xd4, 0xa2, 0xe2, 0xc4, 0x9c, 0x5a, 0x6b, 0x2e, 0x00, 0x84,
  0xf2, 0x80, 0xe5, 0x5e, 0x00, 0x00, 0x00
};

inline void serveAppImmutableNodes_0BUpjlS3Js(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", APP_IMMUTABLE_NODES_0_BUPJL_S3_JS, APP_IMMUTABLE_NODES_0_BUPJL_S3_JS_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// app/immutable/nodes/1.KgnaUiKZ.js
const uint16_t APP_IMMUTABLE_NODES_1_KGNAUIKZ_JS_L = 86;
const uint8_t APP_IMMUTABLE_NODES_1_KGNAUIKZ_JS[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0xcb, 0xcc, 0x2d, 0xc8, 0x2f, 0x2a,
  0xa9, 0x76, 0x55, 0x48, 0x2c, 0x56, 0xc8, 0xad, 0x4d, 0x2b, 0xca, 0xcf, 0x55, 0xd2, 0xd3, 0xd3,
  0x4f, 0xce, 0x28, 0xcd, 0xcb, 0x2e, 0xd6, 0xcf, 0xcc, 0x4b, 0x49, 0xad, 0xd0, 0xcb, 0x75, 0xf3,
  0x4e, 0xc9, 0x70, 0xb1, 0x70, 0xd6, 0xcb, 0x2a, 0x56, 0xb2, 0x4e, 0xad, 0x00, 0xab, 0xcf, 0x05,
  0xa9, 0x4f, 0xce, 0x07, 0x6a, 0xce, 0x4b, 0xcd, 0x2b, 0xa9, 0xb5, 0xe6, 0x02, 0x00, 0xcb, 0x0c,
  0xb8, 0xa2, 0x48, 0x00, 0x00, 0x00
};

inline void serveAppImmutableNodes_1KgnaUiKzJs(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", APP_IMMUTABLE_NODES_1_KGNAUIKZ_JS, APP_IMMUTABLE_NODES_1_KGNAUIKZ_JS_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// app/immutable/nodes/2.DRTs-OeU.js
const uint16_t APP_IMMUTABLE_NODES_2_DRTS_OEU_JS_L = 86;
const uint8_t APP_IMMUTABLE_NODES_2_DRTS_OEU_JS[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0xcb, 0xcc, 0x2d, 0xc8, 0x2f, 0x2a,
  0xa9, 0x4e, 0x56, 0x48, 0x2c, 0x56, 0x48, 0xad, 0x4d, 0x2b, 0xca, 0xcf, 0x55, 0xd2, 0xd3, 0xd3,
  0x4f, 0xce, 0x28, 0xcd, 0xcb, 0x2e, 0xd6, 0xcf, 0xcc, 0x4b, 0x49, 0xad, 0xd0, 0xcb, 0x75, 0xf3,
  0x4e, 0xc9, 0x70, 0xb1, 0x70, 0xd6, 0xcb, 0x2a, 0x56, 0xb2, 0x4e, 0xad, 0x00, 0xab, 0x4f, 0x05,
  0xa9, 0x4f, 0xce, 0x07, 0x6a, 0xce, 0x4b, 0xcd, 0x2b, 0xa9, 0xb5, 0xe6, 0x02, 0x00, 0xe4, 0xcc,
  0x2e, 0xd7, 0x48, 0x00, 0x00, 0x00
};

inline void serveAppImmutableNodes_2DrTsOeUJs(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", APP_IMMUTABLE_NODES_2_DRTS_OEU_JS, APP_IMMUTABLE_NODES_2_DRTS_OEU_JS_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// app/immutable/nodes/3.DAisRa9z.js
const uint16_t APP_IMMUTABLE_NODES_3_DAISRA9Z_JS_L = 86;
const uint8_t APP_IMMUTABLE_NODES_3_DAISRA9Z_JS[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0xcb, 0xcc, 0x2d, 0xc8, 0x2f, 0x2a,
  0xa9, 0x4e, 0x55, 0x48, 0x2c, 0x56, 0xc8, 0xad, 0x4d, 0x2b, 0xca, 0xcf, 0x55, 0xd2, 0xd3, 0xd3,
  0x4f, 0xce, 0x28, 0xcd, 0xcb, 0x2e, 0xd6, 0xcf, 0xcc, 0x4b, 0x49, 0xad, 0xd0, 0xcb, 0x75, 0xf3,
  0x4e, 0xc9, 0x70, 0xb1, 0x70, 0xd6, 0xcb, 0x2a, 0x56, 0xb2, 0x4e, 0xad, 0x00, 0xab, 0xcf, 0x05,
  0xa9, 0x4f, 0xce, 0x07, 0x6a, 0xce, 0x4b, 0xcd, 0x2b, 0xa9, 0xb5, 0xe6, 0x02, 0x00, 0x91, 0x0a,
  0x88, 0x65, 0x48, 0x00, 0x00, 0x00
};

inline void serveAppImmutableNodes_3DAisRa9zJs(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", APP_IMMUTABLE_NODES_3_DAISRA9Z_JS, APP_IMMUTABLE_NODES_3_DAISRA9Z_JS_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// app/immutable/nodes/4.pU4Xl9Mg.js
const uint16_t APP_IMMUTABLE_NODES_4_PU4XL9MG_JS_L = 82;
const uint8_t APP_IMMUTABLE_NODES_4_PU4XL9MG_JS[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0xcb, 0xcc, 0x2d, 0xc8, 0x2f, 0x2a,
  0xa9, 0x4e, 0xab, 0x4d, 0x2b, 0xca, 0xcf, 0x55, 0xd2, 0xd3, 0xd3, 0x4f, 0xce, 0x28, 0xcd, 0xcb,
  0x2e, 0xd6, 0xcf, 0xcc, 0x4b, 0x49, 0xad, 0xd0, 0xcb, 0x75, 0xf3, 0x4e, 0xc9, 0x70, 0xb1, 0x70,
  0xd6, 0xcb, 0x2a, 0x56, 0xb2, 0x4e, 0xad, 0x80, 0x28, 0x55, 0x48, 0x2c, 0x56, 0x48, 0xce, 0x07,
  0xea, 0xcb, 0x4b, 0xcd, 0x2b, 0xa9, 0xb5, 0xe6, 0x02, 0x00, 0xf1, 0xfb, 0x4f, 0x22, 0x43, 0x00,
  0x00, 0x00
};

inline void serveAppImmutableNodes_4PU4Xl9MgJs(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", APP_IMMUTABLE_NODES_4_PU4XL9MG_JS, APP_IMMUTABLE_NODES_4_PU4XL9MG_JS_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// app/immutable/nodes/5.9ec-vGtq.js
const uint16_t APP_IMMUTABLE_NODES_5_9EC_VGTQ_JS_L = 86;
const uint8_t APP_IMMUTABLE_NODES_5_9EC_VGTQ_JS[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0xcb, 0xcc, 0x2d, 0xc8, 0x2f, 0x2a,
  0xa9, 0x4e, 0x57, 0x48, 0x2c, 0x56, 0xc8, 0xad, 0x4d, 0x2b, 0xca, 0xcf, 0x55, 0xd2, 0xd3, 0xd3,
  0x4f, 0xce, 0x28, 0xcd, 0xcb, 0x2e, 0xd6, 0xcf, 0xcc, 0x4b, 0x49, 0xad, 0xd0, 0xcb, 0x75, 0xf3,
  0x4e, 0xc9, 0x70, 0xb1, 0x70, 0xd6, 0xcb, 0x2a, 0x56, 0xb2, 0x4e, 0xad, 0x00, 0xab, 0xcf, 0x05,
  0xa9, 0x4f, 0xce, 0x07, 0x6a, 0xce, 0x4b, 0xcd, 0x2b, 0xa9, 0xb5, 0xe6, 0x02, 0x00, 0x1c, 0xa9,
  0x2d, 0xbf, 0x48, 0x00, 0x00, 0x00
};

inline void serveAppImmutableNodes_5_9ecVGtqJs(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", APP_IMMUTABLE_NODES_5_9EC_VGTQ_JS, APP_IMMUTABLE_NODES_5_9EC_VGTQ_JS_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

