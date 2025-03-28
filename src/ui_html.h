/*
 * Binary arrays for the Web UI ui_html files.
 */

#pragma once
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

// devices.html
const uint16_t DEVICES_HTML_L = 672;
const uint8_t DEVICES_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x9c, 0x93, 0x65, 0x9f, 0xe3, 0x2c,
  0x10, 0xc0, 0x5f, 0x77, 0x3f, 0xc5, 0x2c, 0x8f, 0x34, 0xf9, 0xfd, 0x1a, 0xb2, 0xae, 0xcd, 0x3e,
  0x72, 0xee, 0xee, 0x4a, 0xc3, 0xb4, 0xe1, 0x4a, 0x20, 0x07, 0xb4, 0x5d, 0xfd, 0xee, 0x07, 0x91,
  0x73, 0x7f, 0x83, 0xce, 0xfc, 0xc7, 0x87, 0xcb, 0x5c, 0xe7, 0xee, 0xa8, 0x42, 0x28, 0x5c, 0x29,
  0x0f, 0x96, 0x86, 0x61, 0x03, 0xc9, 0xd4, 0x24, 0x23, 0xa8, 0xc8, 0xc1, 0x52, 0x6f, 0x58, 0x20,
  0xe3, 0x7e, 0xef, 0x0d, 0x4b, 0x74, 0x0c, 0xf2, 0x82, 0x19, 0x8b, 0x2e, 0x23, 0x33, 0x37, 0x4e,
  0x76, 0x08, 0xa4, 0xf5, 0x97, 0x14, 0x6a, 0x0a, 0x06, 0x65, 0x46, 0x44, 0xae, 0x15, 0x81, 0xc2,
  0xe0, 0x38, 0x23, 0x34, 0x1d, 0xb3, 0x79, 0x78, 0xa0, 0x76, 0x3e, 0x69, 0x45, 0x1b, 0x8a, 0x62,
  0x25, 0x66, 0x64, 0x2e, 0x70, 0x51, 0x69, 0xe3, 0x08, 0x78, 0x19, 0x87, 0xca, 0x53, 0x17, 0x82,
  0xbb, 0x22, 0xe3, 0xe8, 0xd5, 0x30, 0xa9, 0x2f, 0x03, 0x10, 0x4a, 0x38, 0xc1, 0x64, 0x62, 0x73,
  0x26, 0x31, 0x5b, 0xed, 0x40, 0x36, 0x37, 0xa2, 0x72, 0xe1, 0xd8, 0x4b, 0x53, 0xb8, 0xa5, 0xa0,
  0x62, 0x13, 0x04, 0xa9, 0x19, 0x07, 0x6d, 0x60, 0x51, 0xa0, 0x0a, 0xce, 0xaa, 0x89, 0x50, 0x13,
  0x70, 0x05, 0x96, 0x68, 0x07, 0x30, 0x42, 0xeb, 0xc0, 0x69, 0x60, 0x9c, 0x7b, 0xac, 0xf7, 0x1a,
  0xfd, 0x06, 0xaf, 0x43, 0x88, 0xaf, 0xeb, 0xf7, 0xb9, 0x16, 0x1c, 0x2e, 0xde, 0x7a, 0x70, 0x2e,
  0x60, 0xc5, 0x18, 0x22, 0xa9, 0xbd, 0xd5, 0x7b, 0x4e, 0x1b, 0x0f, 0xa7, 0x35, 0x06, 0xb2, 0x2c,
  0x83, 0x3e, 0x67, 0x66, 0xda, 0x87, 0xd3, 0x53, 0x88, 0x96, 0xa3, 0x7e, 0xfd, 0xde, 0x0f, 0xa8,
  0x8f, 0xc5, 0x63, 0xf8, 0xfb, 0x6f, 0x58, 0x08, 0xc5, 0xf5, 0x82, 0x96, 0xcc, 0xe5, 0xc5, 0x0d,
  0xe4, 0x82, 0x45, 0xfd, 0xa8, 0xf2, 0xd9, 0x41, 0x63, 0x93, 0x5c, 0x4b, 0x6d, 0x7c, 0x58, 0x41,
  0x7b, 0x0f, 0x02, 0x31, 0xee, 0xc7, 0x8d, 0x28, 0xda, 0x38, 0x86, 0x93, 0xe0, 0x44, 0xcf, 0x17,
  0x68, 0x56, 0xa2, 0x72, 0xb4, 0x3b, 0x5c, 0x90, 0x58, 0xdf, 0x73, 0xc9, 0xac, 0xbd, 0x2e, 0xac,
  0xa3, 0x3e, 0x9c, 0xa8, 0xf1, 0x28, 0xde, 0x0f, 0x3a, 0x67, 0x80, 0xd2, 0xe2, 0xcf, 0xeb, 0x1b,
  0x2c, 0xf5, 0x1c, 0x3f, 0x45, 0x84, 0x14, 0xa7, 0x1f, 0x72, 0xdc, 0x55, 0xf9, 0x7d, 0x69, 0x59,
  0x55, 0xa5, 0xa2, 0x2c, 0x67, 0x8e, 0x8d, 0x24, 0xa6, 0x9e, 0x84, 0xce, 0xa6, 0x3e, 0x58, 0x3c,
  0xa4, 0xff, 0x8f, 0xa7, 0x77, 0x6e, 0xde, 0xbe, 0xbf, 0x46, 0x73, 0x6b, 0x49, 0xd3, 0x16, 0xd6,
  0x1d, 0x49, 0xb4, 0x05, 0xa2, 0x23, 0x9f, 0x35, 0x4c, 0xa9, 0xf9, 0x4c, 0xa2, 0x4f, 0x49, 0xa8,
  0x1c, 0xf9, 0x06, 0x1e, 0x95, 0x33, 0x47, 0xa9, 0x75, 0xcc, 0x38, 0xba, 0xf5, 0xe0, 0xf8, 0xea,
  0x7a, 0xf9, 0xf8, 0x29, 0x7d, 0x63, 0x7f, 0x8b, 0x95, 0x17, 0x33, 0x35, 0xb5, 0x29, 0x1f, 0xbd,
  0x5d, 0x79, 0xb8, 0x3b, 0x9e, 0x06, 0xcc, 0xef, 0xbb, 0xe4, 0xdf, 0xa8, 0x3e, 0xbf, 0xb1, 0x3d,
  0xae, 0xf0, 0x7a, 0x4b, 0x1a, 0xa6, 0xed, 0xc4, 0x0c, 0x47, 0x9a, 0x1f, 0x01, 0x67, 0x8e, 0x25,
  0x76, 0x8e, 0xd2, 0xe1, 0x54, 0xb8, 0xa4, 0xa5, 0x26, 0xe1, 0x39, 0x23, 0x85, 0x4f, 0xbb, 0x69,
  0xcc, 0x73, 0x31, 0x87, 0x3a, 0x4b, 0x19, 0xe1, 0xc2, 0x56, 0x92, 0x1d, 0xed, 0x75, 0x63, 0x51,
  0x63, 0x3f, 0xed, 0xf8, 0x5e, 0x53, 0xdb, 0xde, 0xab, 0x57, 0xef, 0xd1, 0xaf, 0x36, 0x36, 0x0b,
  0xc8, 0xa0, 0xfd, 0xe8, 0x8d, 0x98, 0xf5, 0x4d, 0xa5, 0x70, 0x01, 0x0f, 0xee, 0x5e, 0x8f, 0x08,
  0x25, 0x83, 0xba, 0x39, 0x9d, 0xd0, 0x2a, 0xa6, 0x15, 0x73, 0x45, 0x98, 0x41, 0x6a, 0xa5, 0xc8,
  0x31, 0x5a, 0x19, 0x40, 0xb2, 0x1a, 0x37, 0x8a, 0x67, 0xfb, 0x4b, 0xcd, 0xc1, 0x1b, 0xb7, 0x0e,
  0xb0, 0x69, 0x16, 0x0f, 0x7e, 0xdf, 0x47, 0xf9, 0xcc, 0x18, 0xbf, 0xdf, 0xab, 0xbd, 0xf1, 0x28,
  0xf3, 0xa1, 0xa7, 0x3a, 0xdd, 0xdb, 0x46, 0x97, 0xc2, 0x22, 0x65, 0x52, 0x46, 0xcf, 0x5a, 0x87,
  0x44, 0x19, 0x66, 0x3d, 0xfa, 0xe9, 0xda, 0xc6, 0x83, 0x9f, 0x52, 0xfc, 0xbc, 0x02, 0x6d, 0x18,
  0x2f, 0xe2, 0x30, 0xad, 0x2a, 0x8a, 0x9e, 0xf9, 0xd4, 0x0c, 0xc0, 0x4b, 0xbd, 0x88, 0x21, 0x3b,
  0x78, 0x9f, 0x1e, 0xff, 0x4a, 0x6b, 0x9b, 0x91, 0xff, 0x1a, 0x74, 0x61, 0xc6, 0xfb, 0x6d, 0x12,
  0xc2, 0xa1, 0x9d, 0x82, 0x77, 0x43, 0xca, 0x06, 0x20, 0x36, 0x30, 0x9e, 0xc0, 0x91, 0x0c, 0x8a,
  0x5c, 0x60, 0x79, 0xa9, 0x0f, 0x29, 0x37, 0x01, 0x55, 0x6e, 0xe1, 0x88, 0x48, 0x05, 0x00, 0x00
};

inline void serveDevicesHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", DEVICES_HTML, DEVICES_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// fingerprints.html
const uint16_t FINGERPRINTS_HTML_L = 672;
const uint8_t FINGERPRINTS_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x9c, 0x93, 0x65, 0x9f, 0xe3, 0x2c,
  0x10, 0xc0, 0x5f, 0x77, 0x3f, 0xc5, 0x2c, 0x8f, 0x34, 0xf9, 0xfd, 0x1a, 0xb2, 0xae, 0xcd, 0x3e,
  0x72, 0xee, 0xee, 0x4a, 0xc3, 0xb4, 0xe1, 0x4a, 0x20, 0x07, 0xb4, 0x5d, 0xfd, 0xee, 0x07, 0x91,
  0x73, 0x7f, 0x83, 0xce, 0xfc, 0xc7, 0x87, 0xcb, 0x5c, 0xe7, 0xee, 0xa8, 0x42, 0x28, 0x5c, 0x29,
  0x0f, 0x96, 0x86, 0x61, 0x03, 0xc9, 0xd4, 0x24, 0x23, 0xa8, 0xc8, 0xc1, 0x52, 0x6f, 0x58, 0x20,
  0xe3, 0x7e, 0xef, 0x0d, 0x4b, 0x74, 0x0c, 0xf2, 0x82, 0x19, 0x8b, 0x2e, 0x23, 0x33, 0x37, 0x4e,
  0x76, 0x08, 0xa4, 0xf5, 0x97, 0x14, 0x6a, 0x0a, 0x06, 0x65, 0x46, 0x44, 0xae, 0x15, 0x81, 0xc2,
  0xe0, 0x38, 0x23, 0x34, 0x1d, 0xb3, 0x79, 0x78, 0xa0, 0x76, 0x3e, 0x69, 0x45, 0x1b, 0x8a, 0x62,
  0x25, 0x66, 0x64, 0x2e, 0x70, 0x51, 0x69, 0xe3, 0x08, 0x78, 0x19, 0x87, 0xca, 0x53, 0x17, 0x82,
  0xbb, 0x22, 0xe3, 0xe8, 0xd5, 0x30, 0xa9, 0x2f, 0x03, 0x10, 0x4a, 0x38, 0xc1, 0x64, 0x62, 0x73,
  0x26, 0x31, 0x5b, 0xed, 0x40, 0x36, 0x37, 0xa2, 0x72, 0xe1, 0xd8, 0x4b, 0x53, 0xb8, 0xa5, 0xa0,
  0x62, 0x13, 0x04, 0xa9, 0x19, 0x07, 0x6d, 0x60, 0x51, 0xa0, 0x0a, 0xce, 0xaa, 0x89, 0x50, 0x13,
  0x70, 0x05, 0x96, 0x68, 0x07, 0x30, 0x42, 0xeb, 0xc0, 0x69, 0x60, 0x9c, 0x7b, 0xac, 0xf7, 0x1a,
  0xfd, 0x06, 0xaf, 0x43, 0x88, 0xaf, 0xeb, 0xf7, 0xb9, 0x16, 0x1c, 0x2e, 0xde, 0x7a, 0x70, 0x2e,
  0x60, 0xc5, 0x18, 0x22, 0xa9, 0xbd, 0xd5, 0x7b, 0x4e, 0x1b, 0x0f, 0xa7, 0x35, 0x06, 0xb2, 0x2c,
  0x83, 0x3e, 0x67, 0x66, 0xda, 0x87, 0xd3, 0x53, 0x88, 0x96, 0xa3, 0x7e, 0xfd, 0xde, 0x0f, 0xa8,
  0x8f, 0xc5, 0x63, 0xf8, 0xfb, 0x6f, 0x58, 0x08, 0xc5, 0xf5, 0x82, 0x96, 0xcc, 0xe5, 0xc5, 0x0d,
  0xe4, 0x82, 0x45, 0xfd, 0xa8, 0xf2, 0xd9, 0x41, 0x63, 0x93, 0x5c, 0x4b, 0x6d, 0x7c, 0x58, 0x41,
  0x7b, 0x0f, 0x02, 0x31, 0xee, 0xc7, 0x8d, 0x28, 0xda, 0x38, 0x86, 0x93, 0xe0, 0x44, 0xcf, 0x17,
  0x68, 0x56, 0xa2, 0x72, 0xb4, 0x3b, 0x5c, 0x90, 0x58, 0xdf, 0x73, 0xc9, 0xac, 0xbd, 0x2e, 0xac,
  0xa3, 0x3e, 0x9c, 0xa8, 0xf1, 0x28, 0xde, 0x0f, 0x3a, 0x67, 0x80, 0xd2, 0xe2, 0xcf, 0xeb, 0x1b,
  0x2c, 0xf5, 0x1c, 0x3f, 0x45, 0x84, 0x14, 0xa7, 0x1f, 0x72, 0xdc, 0x55, 0xf9, 0x7d, 0x69, 0x59,
  0x55, 0xa5, 0xa2, 0x2c, 0x67, 0x8e, 0x8d, 0x24, 0xa6, 0x9e, 0x84, 0xce, 0xa6, 0x3e, 0x58, 0x3c,
  0xa4, 0xff, 0x8f, 0xa7, 0x77, 0x6e, 0xde, 0xbe, 0xbf, 0x46, 0x73, 0x6b, 0x49, 0xd3, 0x16, 0xd6,
  0x1d, 0x49, 0xb4, 0x05, 0xa2, 0x23, 0x9f, 0x35, 0x4c, 0xa9, 0xf9, 0x4c, 0xa2, 0x4f, 0x49, 0xa8,
  0x1c, 0xf9, 0x06, 0x1e, 0x95, 0x33, 0x47, 0xa9, 0x75, 0xcc, 0x38, 0xba, 0xf5, 0xe0, 0xf8, 0xea,
  0x7a, 0xf9, 0xf8, 0x29, 0x7d, 0x63, 0x7f, 0x8b, 0x95, 0x17, 0x33, 0x35, 0xb5, 0x29, 0x1f, 0xbd,
  0x5d, 0x79, 0xb8, 0x3b, 0x9e, 0x06, 0xcc, 0xef, 0xbb, 0xe4, 0xdf, 0xa8, 0x3e, 0xbf, 0xb1, 0x3d,
  0xae, 0xf0, 0x7a, 0x4b, 0x1a, 0xa6, 0xed, 0xc4, 0x0c, 0x47, 0x9a, 0x1f, 0x01, 0x67, 0x8e, 0x25,
  0x76, 0x8e, 0xd2, 0xe1, 0x54, 0xb8, 0xa4, 0xa5, 0x26, 0xe1, 0x39, 0x23, 0x85, 0x4f, 0xbb, 0x69,
  0xcc, 0x73, 0x31, 0x87, 0x3a, 0x4b, 0x19, 0xe1, 0xc2, 0x56, 0x92, 0x1d, 0xed, 0x75, 0x63, 0x51,
  0x63, 0x3f, 0xed, 0xf8, 0x5e, 0x53, 0xdb, 0xde, 0xab, 0x57, 0xef, 0xd1, 0xaf, 0x36, 0x36, 0x0b,
  0xc8, 0xa0, 0xfd, 0xe8, 0x8d, 0x98, 0xf5, 0x4d, 0xa5, 0x70, 0x01, 0x0f, 0xee, 0x5e, 0x8f, 0x08,
  0x25, 0x83, 0xba, 0x39, 0x9d, 0xd0, 0x2a, 0xa6, 0x15, 0x73, 0x45, 0x98, 0x41, 0x6a, 0xa5, 0xc8,
  0x31, 0x5a, 0x19, 0x40, 0xb2, 0x1a, 0x37, 0x8a, 0x67, 0xfb, 0x4b, 0xcd, 0xc1, 0x1b, 0xb7, 0x0e,
  0xb0, 0x69, 0x16, 0x0f, 0x7e, 0xdf, 0x47, 0xf9, 0xcc, 0x18, 0xbf, 0xdf, 0xab, 0xbd, 0xf1, 0x28,
  0xf3, 0xa1, 0xa7, 0x3a, 0xdd, 0xdb, 0x46, 0x97, 0xc2, 0x22, 0x65, 0x52, 0x46, 0xcf, 0x5a, 0x87,
  0x44, 0x19, 0x66, 0x3d, 0xfa, 0xe9, 0xda, 0xc6, 0x83, 0x9f, 0x52, 0xfc, 0xbc, 0x02, 0x6d, 0x18,
  0x2f, 0xe2, 0x30, 0xad, 0x2a, 0x8a, 0x9e, 0xf9, 0xd4, 0x0c, 0xc0, 0x4b, 0xbd, 0x88, 0x21, 0x3b,
  0x78, 0x9f, 0x1e, 0xff, 0x4a, 0x6b, 0x9b, 0x91, 0xff, 0x1a, 0x74, 0x61, 0xc6, 0xfb, 0x6d, 0x12,
  0xc2, 0xa1, 0x9d, 0x82, 0x77, 0x43, 0xca, 0x06, 0x20, 0x36, 0x30, 0x9e, 0xc0, 0x91, 0x0c, 0x8a,
  0x5c, 0x60, 0x79, 0xa9, 0x0f, 0x29, 0x37, 0x01, 0x55, 0x6e, 0xe1, 0x88, 0x48, 0x05, 0x00, 0x00
};

inline void serveFingerprintsHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", FINGERPRINTS_HTML, FINGERPRINTS_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// index.html
const uint16_t INDEX_HTML_L = 672;
const uint8_t INDEX_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x9c, 0x93, 0x65, 0x9f, 0xe3, 0x2c,
  0x10, 0xc0, 0x5f, 0x77, 0x3f, 0xc5, 0x2c, 0x8f, 0x34, 0xf9, 0xfd, 0x1a, 0xb2, 0xae, 0xcd, 0x3e,
  0x72, 0xee, 0xee, 0x4a, 0xc3, 0xb4, 0xe1, 0x4a, 0x20, 0x07, 0xb4, 0x5d, 0xfd, 0xee, 0x07, 0x91,
  0x73, 0x7f, 0x83, 0xce, 0xfc, 0xc7, 0x87, 0xcb, 0x5c, 0xe7, 0xee, 0xa8, 0x42, 0x28, 0x5c, 0x29,
  0x0f, 0x96, 0x86, 0x61, 0x03, 0xc9, 0xd4, 0x24, 0x23, 0xa8, 0xc8, 0xc1, 0x52, 0x6f, 0x58, 0x20,
  0xe3, 0x7e, 0xef, 0x0d, 0x4b, 0x74, 0x0c, 0xf2, 0x82, 0x19, 0x8b, 0x2e, 0x23, 0x33, 0x37, 0x4e,
  0x76, 0x08, 0xa4, 0xf5, 0x97, 0x14, 0x6a, 0x0a, 0x06, 0x65, 0x46, 0x44, 0xae, 0x15, 0x81, 0xc2,
  0xe0, 0x38, 0x23, 0x34, 0x1d, 0xb3, 0x79, 0x78, 0xa0, 0x76, 0x3e, 0x69, 0x45, 0x1b, 0x8a, 0x62,
  0x25, 0x66, 0x64, 0x2e, 0x70, 0x51, 0x69, 0xe3, 0x08, 0x78, 0x19, 0x87, 0xca, 0x53, 0x17, 0x82,
  0xbb, 0x22, 0xe3, 0xe8, 0xd5, 0x30, 0xa9, 0x2f, 0x03, 0x10, 0x4a, 0x38, 0xc1, 0x64, 0x62, 0x73,
  0x26, 0x31, 0x5b, 0xed, 0x40, 0x36, 0x37, 0xa2, 0x72, 0xe1, 0xd8, 0x4b, 0x53, 0xb8, 0xa5, 0xa0,
  0x62, 0x13, 0x04, 0xa9, 0x19, 0x07, 0x6d, 0x60, 0x51, 0xa0, 0x0a, 0xce, 0xaa, 0x89, 0x50, 0x13,
  0x70, 0x05, 0x96, 0x68, 0x07, 0x30, 0x42, 0xeb, 0xc0, 0x69, 0x60, 0x9c, 0x7b, 0xac, 0xf7, 0x1a,
  0xfd, 0x06, 0xaf, 0x43, 0x88, 0xaf, 0xeb, 0xf7, 0xb9, 0x16, 0x1c, 0x2e, 0xde, 0x7a, 0x70, 0x2e,
  0x60, 0xc5, 0x18, 0x22, 0xa9, 0xbd, 0xd5, 0x7b, 0x4e, 0x1b, 0x0f, 0xa7, 0x35, 0x06, 0xb2, 0x2c,
  0x83, 0x3e, 0x67, 0x66, 0xda, 0x87, 0xd3, 0x53, 0x88, 0x96, 0xa3, 0x7e, 0xfd, 0xde, 0x0f, 0xa8,
  0x8f, 0xc5, 0x63, 0xf8, 0xfb, 0x6f, 0x58, 0x08, 0xc5, 0xf5, 0x82, 0x96, 0xcc, 0xe5, 0xc5, 0x0d,
  0xe4, 0x82, 0x45, 0xfd, 0xa8, 0xf2, 0xd9, 0x41, 0x63, 0x93, 0x5c, 0x4b, 0x6d, 0x7c, 0x58, 0x41,
  0x7b, 0x0f, 0x02, 0x31, 0xee, 0xc7, 0x8d, 0x28, 0xda, 0x38, 0x86, 0x93, 0xe0, 0x44, 0xcf, 0x17,
  0x68, 0x56, 0xa2, 0x72, 0xb4, 0x3b, 0x5c, 0x90, 0x58, 0xdf, 0x73, 0xc9, 0xac, 0xbd, 0x2e, 0xac,
  0xa3, 0x3e, 0x9c, 0xa8, 0xf1, 0x28, 0xde, 0x0f, 0x3a, 0x67, 0x80, 0xd2, 0xe2, 0xcf, 0xeb, 0x1b,
  0x2c, 0xf5, 0x1c, 0x3f, 0x45, 0x84, 0x14, 0xa7, 0x1f, 0x72, 0xdc, 0x55, 0xf9, 0x7d, 0x69, 0x59,
  0x55, 0xa5, 0xa2, 0x2c, 0x67, 0x8e, 0x8d, 0x24, 0xa6, 0x9e, 0x84, 0xce, 0xa6, 0x3e, 0x58, 0x3c,
  0xa4, 0xff, 0x8f, 0xa7, 0x77, 0x6e, 0xde, 0xbe, 0xbf, 0x46, 0x73, 0x6b, 0x49, 0xd3, 0x16, 0xd6,
  0x1d, 0x49, 0xb4, 0x05, 0xa2, 0x23, 0x9f, 0x35, 0x4c, 0xa9, 0xf9, 0x4c, 0xa2, 0x4f, 0x49, 0xa8,
  0x1c, 0xf9, 0x06, 0x1e, 0x95, 0x33, 0x47, 0xa9, 0x75, 0xcc, 0x38, 0xba, 0xf5, 0xe0, 0xf8, 0xea,
  0x7a, 0xf9, 0xf8, 0x29, 0x7d, 0x63, 0x7f, 0x8b, 0x95, 0x17, 0x33, 0x35, 0xb5, 0x29, 0x1f, 0xbd,
  0x5d, 0x79, 0xb8, 0x3b, 0x9e, 0x06, 0xcc, 0xef, 0xbb, 0xe4, 0xdf, 0xa8, 0x3e, 0xbf, 0xb1, 0x3d,
  0xae, 0xf0, 0x7a, 0x4b, 0x1a, 0xa6, 0xed, 0xc4, 0x0c, 0x47, 0x9a, 0x1f, 0x01, 0x67, 0x8e, 0x25,
  0x76, 0x8e, 0xd2, 0xe1, 0x54, 0xb8, 0xa4, 0xa5, 0x26, 0xe1, 0x39, 0x23, 0x85, 0x4f, 0xbb, 0x69,
  0xcc, 0x73, 0x31, 0x87, 0x3a, 0x4b, 0x19, 0xe1, 0xc2, 0x56, 0x92, 0x1d, 0xed, 0x75, 0x63, 0x51,
  0x63, 0x3f, 0xed, 0xf8, 0x5e, 0x53, 0xdb, 0xde, 0xab, 0x57, 0xef, 0xd1, 0xaf, 0x36, 0x36, 0x0b,
  0xc8, 0xa0, 0xfd, 0xe8, 0x8d, 0x98, 0xf5, 0x4d, 0xa5, 0x70, 0x01, 0x0f, 0xee, 0x5e, 0x8f, 0x08,
  0x25, 0x83, 0xba, 0x39, 0x9d, 0xd0, 0x2a, 0xa6, 0x15, 0x73, 0x45, 0x98, 0x41, 0x6a, 0xa5, 0xc8,
  0x31, 0x5a, 0x19, 0x40, 0xb2, 0x1a, 0x37, 0x8a, 0x67, 0xfb, 0x4b, 0xcd, 0xc1, 0x1b, 0xb7, 0x0e,
  0xb0, 0x69, 0x16, 0x0f, 0x7e, 0xdf, 0x47, 0xf9, 0xcc, 0x18, 0xbf, 0xdf, 0xab, 0xbd, 0xf1, 0x28,
  0xf3, 0xa1, 0xa7, 0x3a, 0xdd, 0xdb, 0x46, 0x97, 0xc2, 0x22, 0x65, 0x52, 0x46, 0xcf, 0x5a, 0x87,
  0x44, 0x19, 0x66, 0x3d, 0xfa, 0xe9, 0xda, 0xc6, 0x83, 0x9f, 0x52, 0xfc, 0xbc, 0x02, 0x6d, 0x18,
  0x2f, 0xe2, 0x30, 0xad, 0x2a, 0x8a, 0x9e, 0xf9, 0xd4, 0x0c, 0xc0, 0x4b, 0xbd, 0x88, 0x21, 0x3b,
  0x78, 0x9f, 0x1e, 0xff, 0x4a, 0x6b, 0x9b, 0x91, 0xff, 0x1a, 0x74, 0x61, 0xc6, 0xfb, 0x6d, 0x12,
  0xc2, 0xa1, 0x9d, 0x82, 0x77, 0x43, 0xca, 0x06, 0x20, 0x36, 0x30, 0x9e, 0xc0, 0x91, 0x0c, 0x8a,
  0x5c, 0x60, 0x79, 0xa9, 0x0f, 0x29, 0x37, 0x01, 0x55, 0x6e, 0xe1, 0x88, 0x48, 0x05, 0x00, 0x00
};

inline void serveIndexHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", INDEX_HTML, INDEX_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// network.html
const uint16_t NETWORK_HTML_L = 672;
const uint8_t NETWORK_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x9c, 0x93, 0x65, 0x9f, 0xe3, 0x2c,
  0x10, 0xc0, 0x5f, 0x77, 0x3f, 0xc5, 0x2c, 0x8f, 0x34, 0xf9, 0xfd, 0x1a, 0xb2, 0xae, 0xcd, 0x3e,
  0x72, 0xee, 0xee, 0x4a, 0xc3, 0xb4, 0xe1, 0x4a, 0x20, 0x07, 0xb4, 0x5d, 0xfd, 0xee, 0x07, 0x91,
  0x73, 0x7f, 0x83, 0xce, 0xfc, 0xc7, 0x87, 0xcb, 0x5c, 0xe7, 0xee, 0xa8, 0x42, 0x28, 0x5c, 0x29,
  0x0f, 0x96, 0x86, 0x61, 0x03, 0xc9, 0xd4, 0x24, 0x23, 0xa8, 0xc8, 0xc1, 0x52, 0x6f, 0x58, 0x20,
  0xe3, 0x7e, 0xef, 0x0d, 0x4b, 0x74, 0x0c, 0xf2, 0x82, 0x19, 0x8b, 0x2e, 0x23, 0x33, 0x37, 0x4e,
  0x76, 0x08, 0xa4, 0xf5, 0x97, 0x14, 0x6a, 0x0a, 0x06, 0x65, 0x46, 0x44, 0xae, 0x15, 0x81, 0xc2,
  0xe0, 0x38, 0x23, 0x34, 0x1d, 0xb3, 0x79, 0x78, 0xa0, 0x76, 0x3e, 0x69, 0x45, 0x1b, 0x8a, 0x62,
  0x25, 0x66, 0x64, 0x2e, 0x70, 0x51, 0x69, 0xe3, 0x08, 0x78, 0x19, 0x87, 0xca, 0x53, 0x17, 0x82,
  0xbb, 0x22, 0xe3, 0xe8, 0xd5, 0x30, 0xa9, 0x2f, 0x03, 0x10, 0x4a, 0x38, 0xc1, 0x64, 0x62, 0x73,
  0x26, 0x31, 0x5b, 0xed, 0x40, 0x36, 0x37, 0xa2, 0x72, 0xe1, 0xd8, 0x4b, 0x53, 0xb8, 0xa5, 0xa0,
  0x62, 0x13, 0x04, 0xa9, 0x19, 0x07, 0x6d, 0x60, 0x51, 0xa0, 0x0a, 0xce, 0xaa, 0x89, 0x50, 0x13,
  0x70, 0x05, 0x96, 0x68, 0x07, 0x30, 0x42, 0xeb, 0xc0, 0x69, 0x60, 0x9c, 0x7b, 0xac, 0xf7, 0x1a,
  0xfd, 0x06, 0xaf, 0x43, 0x88, 0xaf, 0xeb, 0xf7, 0xb9, 0x16, 0x1c, 0x2e, 0xde, 0x7a, 0x70, 0x2e,
  0x60, 0xc5, 0x18, 0x22, 0xa9, 0xbd, 0xd5, 0x7b, 0x4e, 0x1b, 0x0f, 0xa7, 0x35, 0x06, 0xb2, 0x2c,
  0x83, 0x3e, 0x67, 0x66, 0xda, 0x87, 0xd3, 0x53, 0x88, 0x96, 0xa3, 0x7e, 0xfd, 0xde, 0x0f, 0xa8,
  0x8f, 0xc5, 0x63, 0xf8, 0xfb, 0x6f, 0x58, 0x08, 0xc5, 0xf5, 0x82, 0x96, 0xcc, 0xe5, 0xc5, 0x0d,
  0xe4, 0x82, 0x45, 0xfd, 0xa8, 0xf2, 0xd9, 0x41, 0x63, 0x93, 0x5c, 0x4b, 0x6d, 0x7c, 0x58, 0x41,
  0x7b, 0x0f, 0x02, 0x31, 0xee, 0xc7, 0x8d, 0x28, 0xda, 0x38, 0x86, 0x93, 0xe0, 0x44, 0xcf, 0x17,
  0x68, 0x56, 0xa2, 0x72, 0xb4, 0x3b, 0x5c, 0x90, 0x58, 0xdf, 0x73, 0xc9, 0xac, 0xbd, 0x2e, 0xac,
  0xa3, 0x3e, 0x9c, 0xa8, 0xf1, 0x28, 0xde, 0x0f, 0x3a, 0x67, 0x80, 0xd2, 0xe2, 0xcf, 0xeb, 0x1b,
  0x2c, 0xf5, 0x1c, 0x3f, 0x45, 0x84, 0x14, 0xa7, 0x1f, 0x72, 0xdc, 0x55, 0xf9, 0x7d, 0x69, 0x59,
  0x55, 0xa5, 0xa2, 0x2c, 0x67, 0x8e, 0x8d, 0x24, 0xa6, 0x9e, 0x84, 0xce, 0xa6, 0x3e, 0x58, 0x3c,
  0xa4, 0xff, 0x8f, 0xa7, 0x77, 0x6e, 0xde, 0xbe, 0xbf, 0x46, 0x73, 0x6b, 0x49, 0xd3, 0x16, 0xd6,
  0x1d, 0x49, 0xb4, 0x05, 0xa2, 0x23, 0x9f, 0x35, 0x4c, 0xa9, 0xf9, 0x4c, 0xa2, 0x4f, 0x49, 0xa8,
  0x1c, 0xf9, 0x06, 0x1e, 0x95, 0x33, 0x47, 0xa9, 0x75, 0xcc, 0x38, 0xba, 0xf5, 0xe0, 0xf8, 0xea,
  0x7a, 0xf9, 0xf8, 0x29, 0x7d, 0x63, 0x7f, 0x8b, 0x95, 0x17, 0x33, 0x35, 0xb5, 0x29, 0x1f, 0xbd,
  0x5d, 0x79, 0xb8, 0x3b, 0x9e, 0x06, 0xcc, 0xef, 0xbb, 0xe4, 0xdf, 0xa8, 0x3e, 0xbf, 0xb1, 0x3d,
  0xae, 0xf0, 0x7a, 0x4b, 0x1a, 0xa6, 0xed, 0xc4, 0x0c, 0x47, 0x9a, 0x1f, 0x01, 0x67, 0x8e, 0x25,
  0x76, 0x8e, 0xd2, 0xe1, 0x54, 0xb8, 0xa4, 0xa5, 0x26, 0xe1, 0x39, 0x23, 0x85, 0x4f, 0xbb, 0x69,
  0xcc, 0x73, 0x31, 0x87, 0x3a, 0x4b, 0x19, 0xe1, 0xc2, 0x56, 0x92, 0x1d, 0xed, 0x75, 0x63, 0x51,
  0x63, 0x3f, 0xed, 0xf8, 0x5e, 0x53, 0xdb, 0xde, 0xab, 0x57, 0xef, 0xd1, 0xaf, 0x36, 0x36, 0x0b,
  0xc8, 0xa0, 0xfd, 0xe8, 0x8d, 0x98, 0xf5, 0x4d, 0xa5, 0x70, 0x01, 0x0f, 0xee, 0x5e, 0x8f, 0x08,
  0x25, 0x83, 0xba, 0x39, 0x9d, 0xd0, 0x2a, 0xa6, 0x15, 0x73, 0x45, 0x98, 0x41, 0x6a, 0xa5, 0xc8,
  0x31, 0x5a, 0x19, 0x40, 0xb2, 0x1a, 0x37, 0x8a, 0x67, 0xfb, 0x4b, 0xcd, 0xc1, 0x1b, 0xb7, 0x0e,
  0xb0, 0x69, 0x16, 0x0f, 0x7e, 0xdf, 0x47, 0xf9, 0xcc, 0x18, 0xbf, 0xdf, 0xab, 0xbd, 0xf1, 0x28,
  0xf3, 0xa1, 0xa7, 0x3a, 0xdd, 0xdb, 0x46, 0x97, 0xc2, 0x22, 0x65, 0x52, 0x46, 0xcf, 0x5a, 0x87,
  0x44, 0x19, 0x66, 0x3d, 0xfa, 0xe9, 0xda, 0xc6, 0x83, 0x9f, 0x52, 0xfc, 0xbc, 0x02, 0x6d, 0x18,
  0x2f, 0xe2, 0x30, 0xad, 0x2a, 0x8a, 0x9e, 0xf9, 0xd4, 0x0c, 0xc0, 0x4b, 0xbd, 0x88, 0x21, 0x3b,
  0x78, 0x9f, 0x1e, 0xff, 0x4a, 0x6b, 0x9b, 0x91, 0xff, 0x1a, 0x74, 0x61, 0xc6, 0xfb, 0x6d, 0x12,
  0xc2, 0xa1, 0x9d, 0x82, 0x77, 0x43, 0xca, 0x06, 0x20, 0x36, 0x30, 0x9e, 0xc0, 0x91, 0x0c, 0x8a,
  0x5c, 0x60, 0x79, 0xa9, 0x0f, 0x29, 0x37, 0x01, 0x55, 0x6e, 0xe1, 0x88, 0x48, 0x05, 0x00, 0x00
};

inline void serveNetworkHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", NETWORK_HTML, NETWORK_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// settings.html
const uint16_t SETTINGS_HTML_L = 672;
const uint8_t SETTINGS_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x9c, 0x93, 0x65, 0x9f, 0xe3, 0x2c,
  0x10, 0xc0, 0x5f, 0x77, 0x3f, 0xc5, 0x2c, 0x8f, 0x34, 0xf9, 0xfd, 0x1a, 0xb2, 0xae, 0xcd, 0x3e,
  0x72, 0xee, 0xee, 0x4a, 0xc3, 0xb4, 0xe1, 0x4a, 0x20, 0x07, 0xb4, 0x5d, 0xfd, 0xee, 0x07, 0x91,
  0x73, 0x7f, 0x83, 0xce, 0xfc, 0xc7, 0x87, 0xcb, 0x5c, 0xe7, 0xee, 0xa8, 0x42, 0x28, 0x5c, 0x29,
  0x0f, 0x96, 0x86, 0x61, 0x03, 0xc9, 0xd4, 0x24, 0x23, 0xa8, 0xc8, 0xc1, 0x52, 0x6f, 0x58, 0x20,
  0xe3, 0x7e, 0xef, 0x0d, 0x4b, 0x74, 0x0c, 0xf2, 0x82, 0x19, 0x8b, 0x2e, 0x23, 0x33, 0x37, 0x4e,
  0x76, 0x08, 0xa4, 0xf5, 0x97, 0x14, 0x6a, 0x0a, 0x06, 0x65, 0x46, 0x44, 0xae, 0x15, 0x81, 0xc2,
  0xe0, 0x38, 0x23, 0x34, 0x1d, 0xb3, 0x79, 0x78, 0xa0, 0x76, 0x3e, 0x69, 0x45, 0x1b, 0x8a, 0x62,
  0x25, 0x66, 0x64, 0x2e, 0x70, 0x51, 0x69, 0xe3, 0x08, 0x78, 0x19, 0x87, 0xca, 0x53, 0x17, 0x82,
  0xbb, 0x22, 0xe3, 0xe8, 0xd5, 0x30, 0xa9, 0x2f, 0x03, 0x10, 0x4a, 0x38, 0xc1, 0x64, 0x62, 0x73,
  0x26, 0x31, 0x5b, 0xed, 0x40, 0x36, 0x37, 0xa2, 0x72, 0xe1, 0xd8, 0x4b, 0x53, 0xb8, 0xa5, 0xa0,
  0x62, 0x13, 0x04, 0xa9, 0x19, 0x07, 0x6d, 0x60, 0x51, 0xa0, 0x0a, 0xce, 0xaa, 0x89, 0x50, 0x13,
  0x70, 0x05, 0x96, 0x68, 0x07, 0x30, 0x42, 0xeb, 0xc0, 0x69, 0x60, 0x9c, 0x7b, 0xac, 0xf7, 0x1a,
  0xfd, 0x06, 0xaf, 0x43, 0x88, 0xaf, 0xeb, 0xf7, 0xb9, 0x16, 0x1c, 0x2e, 0xde, 0x7a, 0x70, 0x2e,
  0x60, 0xc5, 0x18, 0x22, 0xa9, 0xbd, 0xd5, 0x7b, 0x4e, 0x1b, 0x0f, 0xa7, 0x35, 0x06, 0xb2, 0x2c,
  0x83, 0x3e, 0x67, 0x66, 0xda, 0x87, 0xd3, 0x53, 0x88, 0x96, 0xa3, 0x7e, 0xfd, 0xde, 0x0f, 0xa8,
  0x8f, 0xc5, 0x63, 0xf8, 0xfb, 0x6f, 0x58, 0x08, 0xc5, 0xf5, 0x82, 0x96, 0xcc, 0xe5, 0xc5, 0x0d,
  0xe4, 0x82, 0x45, 0xfd, 0xa8, 0xf2, 0xd9, 0x41, 0x63, 0x93, 0x5c, 0x4b, 0x6d, 0x7c, 0x58, 0x41,
  0x7b, 0x0f, 0x02, 0x31, 0xee, 0xc7, 0x8d, 0x28, 0xda, 0x38, 0x86, 0x93, 0xe0, 0x44, 0xcf, 0x17,
  0x68, 0x56, 0xa2, 0x72, 0xb4, 0x3b, 0x5c, 0x90, 0x58, 0xdf, 0x73, 0xc9, 0xac, 0xbd, 0x2e, 0xac,
  0xa3, 0x3e, 0x9c, 0xa8, 0xf1, 0x28, 0xde, 0x0f, 0x3a, 0x67, 0x80, 0xd2, 0xe2, 0xcf, 0xeb, 0x1b,
  0x2c, 0xf5, 0x1c, 0x3f, 0x45, 0x84, 0x14, 0xa7, 0x1f, 0x72, 0xdc, 0x55, 0xf9, 0x7d, 0x69, 0x59,
  0x55, 0xa5, 0xa2, 0x2c, 0x67, 0x8e, 0x8d, 0x24, 0xa6, 0x9e, 0x84, 0xce, 0xa6, 0x3e, 0x58, 0x3c,
  0xa4, 0xff, 0x8f, 0xa7, 0x77, 0x6e, 0xde, 0xbe, 0xbf, 0x46, 0x73, 0x6b, 0x49, 0xd3, 0x16, 0xd6,
  0x1d, 0x49, 0xb4, 0x05, 0xa2, 0x23, 0x9f, 0x35, 0x4c, 0xa9, 0xf9, 0x4c, 0xa2, 0x4f, 0x49, 0xa8,
  0x1c, 0xf9, 0x06, 0x1e, 0x95, 0x33, 0x47, 0xa9, 0x75, 0xcc, 0x38, 0xba, 0xf5, 0xe0, 0xf8, 0xea,
  0x7a, 0xf9, 0xf8, 0x29, 0x7d, 0x63, 0x7f, 0x8b, 0x95, 0x17, 0x33, 0x35, 0xb5, 0x29, 0x1f, 0xbd,
  0x5d, 0x79, 0xb8, 0x3b, 0x9e, 0x06, 0xcc, 0xef, 0xbb, 0xe4, 0xdf, 0xa8, 0x3e, 0xbf, 0xb1, 0x3d,
  0xae, 0xf0, 0x7a, 0x4b, 0x1a, 0xa6, 0xed, 0xc4, 0x0c, 0x47, 0x9a, 0x1f, 0x01, 0x67, 0x8e, 0x25,
  0x76, 0x8e, 0xd2, 0xe1, 0x54, 0xb8, 0xa4, 0xa5, 0x26, 0xe1, 0x39, 0x23, 0x85, 0x4f, 0xbb, 0x69,
  0xcc, 0x73, 0x31, 0x87, 0x3a, 0x4b, 0x19, 0xe1, 0xc2, 0x56, 0x92, 0x1d, 0xed, 0x75, 0x63, 0x51,
  0x63, 0x3f, 0xed, 0xf8, 0x5e, 0x53, 0xdb, 0xde, 0xab, 0x57, 0xef, 0xd1, 0xaf, 0x36, 0x36, 0x0b,
  0xc8, 0xa0, 0xfd, 0xe8, 0x8d, 0x98, 0xf5, 0x4d, 0xa5, 0x70, 0x01, 0x0f, 0xee, 0x5e, 0x8f, 0x08,
  0x25, 0x83, 0xba, 0x39, 0x9d, 0xd0, 0x2a, 0xa6, 0x15, 0x73, 0x45, 0x98, 0x41, 0x6a, 0xa5, 0xc8,
  0x31, 0x5a, 0x19, 0x40, 0xb2, 0x1a, 0x37, 0x8a, 0x67, 0xfb, 0x4b, 0xcd, 0xc1, 0x1b, 0xb7, 0x0e,
  0xb0, 0x69, 0x16, 0x0f, 0x7e, 0xdf, 0x47, 0xf9, 0xcc, 0x18, 0xbf, 0xdf, 0xab, 0xbd, 0xf1, 0x28,
  0xf3, 0xa1, 0xa7, 0x3a, 0xdd, 0xdb, 0x46, 0x97, 0xc2, 0x22, 0x65, 0x52, 0x46, 0xcf, 0x5a, 0x87,
  0x44, 0x19, 0x66, 0x3d, 0xfa, 0xe9, 0xda, 0xc6, 0x83, 0x9f, 0x52, 0xfc, 0xbc, 0x02, 0x6d, 0x18,
  0x2f, 0xe2, 0x30, 0xad, 0x2a, 0x8a, 0x9e, 0xf9, 0xd4, 0x0c, 0xc0, 0x4b, 0xbd, 0x88, 0x21, 0x3b,
  0x78, 0x9f, 0x1e, 0xff, 0x4a, 0x6b, 0x9b, 0x91, 0xff, 0x1a, 0x74, 0x61, 0xc6, 0xfb, 0x6d, 0x12,
  0xc2, 0xa1, 0x9d, 0x82, 0x77, 0x43, 0xca, 0x06, 0x20, 0x36, 0x30, 0x9e, 0xc0, 0x91, 0x0c, 0x8a,
  0x5c, 0x60, 0x79, 0xa9, 0x0f, 0x29, 0x37, 0x01, 0x55, 0x6e, 0xe1, 0x88, 0x48, 0x05, 0x00, 0x00
};

inline void serveSettingsHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", SETTINGS_HTML, SETTINGS_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

