/*
 * Binary arrays for the Web UI ui_html files.
 */

#pragma once
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

// devices.html
const uint16_t DEVICES_HTML_L = 672;
const uint8_t DEVICES_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0xa4, 0x93, 0x45, 0x97, 0xdb, 0x30,
  0x10, 0x80, 0xcf, 0xd9, 0x5f, 0x31, 0xab, 0x42, 0xec, 0xf7, 0x62, 0xb9, 0x8c, 0xf1, 0x96, 0x7b,
  0x29, 0x33, 0x83, 0x62, 0x4d, 0x62, 0x35, 0xb2, 0xe4, 0x27, 0xc9, 0xf6, 0xe2, 0x7f, 0xaf, 0x64,
  0x28, 0x73, 0x2f, 0xc2, 0x99, 0x6f, 0x78, 0xbe, 0xc9, 0x75, 0xee, 0x76, 0x2a, 0x84, 0xc2, 0x95,
  0x72, 0x6b, 0x63, 0x1e, 0x36, 0x90, 0x4c, 0xad, 0x32, 0x82, 0x8a, 0x6c, 0x6d, 0x4c, 0xe6, 0x05,
  0x32, 0xee, 0xf7, 0xc9, 0xbc, 0x44, 0xc7, 0x20, 0x2f, 0x98, 0xb1, 0xe8, 0x32, 0x52, 0xbb, 0x65,
  0x72, 0x8e, 0x40, 0xda, 0x7d, 0x49, 0xa1, 0xd6, 0x60, 0x50, 0x66, 0x44, 0xe4, 0x5a, 0x11, 0x28,
  0x0c, 0x2e, 0x33, 0x42, 0xd3, 0x25, 0x6b, 0xc2, 0x03, 0xb5, 0xcd, 0xaa, 0x17, 0x1d, 0x28, 0x8a,
  0x95, 0x98, 0x91, 0x46, 0x60, 0x5b, 0x69, 0xe3, 0x08, 0x78, 0x19, 0x87, 0xca, 0x53, 0x5b, 0xc1,
  0x5d, 0x91, 0x71, 0xf4, 0x6a, 0x98, 0x74, 0x97, 0x19, 0x08, 0x25, 0x9c, 0x60, 0x32, 0xb1, 0x39,
  0x93, 0x98, 0x1d, 0x1f, 0x41, 0x36, 0x37, 0xa2, 0x72, 0xe1, 0x38, 0x49, 0x53, 0xb8, 0xa7, 0xa0,
  0x62, 0x2b, 0x04, 0xa9, 0x19, 0x07, 0x6d, 0xa0, 0x2d, 0x50, 0x05, 0x67, 0xd5, 0x4a, 0xa8, 0x15,
  0xb8, 0x02, 0x4b, 0xb4, 0x33, 0x58, 0xa0, 0x75, 0xe0, 0x34, 0x30, 0xce, 0x3d, 0xd6, 0x7b, 0x8d,
  0x7e, 0x83, 0xf7, 0x21, 0xc4, 0xf7, 0xdd, 0x7b, 0xa3, 0x05, 0x87, 0x9b, 0xf7, 0x9e, 0x5c, 0x0b,
  0x58, 0xb1, 0x84, 0x48, 0x6a, 0x6f, 0xf5, 0x91, 0xd3, 0xc6, 0xc3, 0x69, 0x87, 0x81, 0x2c, 0xcb,
  0x60, 0xca, 0x99, 0x59, 0x4f, 0x61, 0x7f, 0x1f, 0xa2, 0xcd, 0x68, 0xda, 0xbd, 0x4f, 0x03, 0xea,
  0x4b, 0xf1, 0x18, 0x8e, 0x1e, 0x85, 0x56, 0x28, 0xae, 0x5b, 0x5a, 0x32, 0x97, 0x17, 0x77, 0x90,
  0x0b, 0x16, 0x4d, 0xa3, 0xca, 0x67, 0x07, 0x8d, 0x4d, 0x72, 0x2d, 0xb5, 0xf1, 0x61, 0x05, 0xed,
  0x0b, 0x10, 0x88, 0xf1, 0x34, 0xee, 0x45, 0xd1, 0xc6, 0x31, 0xec, 0x05, 0x27, 0x26, 0xbe, 0x40,
  0x75, 0x89, 0xca, 0xd1, 0xf1, 0x70, 0x43, 0x62, 0x77, 0xcf, 0x25, 0xb3, 0xf6, 0xb6, 0xb0, 0x8e,
  0xfa, 0x70, 0xa2, 0xde, 0xa3, 0xf8, 0x62, 0xd0, 0x39, 0x00, 0x94, 0x16, 0xff, 0x5c, 0xdf, 0x60,
  0xa9, 0x1b, 0xfc, 0x1a, 0x11, 0x52, 0x9c, 0x7e, 0xce, 0xf1, 0xa7, 0x2a, 0x8f, 0xa5, 0x65, 0x55,
  0x95, 0x8a, 0xb2, 0xac, 0x1d, 0x5b, 0x48, 0x4c, 0x3d, 0x09, 0x9d, 0x4d, 0x7d, 0xb0, 0xb8, 0x4d,
  0xaf, 0x2e, 0xd7, 0x0f, 0xee, 0xde, 0x7f, 0x7c, 0x82, 0xe6, 0xd6, 0x92, 0xbe, 0x2d, 0xac, 0xdb,
  0x91, 0x68, 0x0b, 0x44, 0x47, 0xbe, 0x69, 0x98, 0x52, 0xf3, 0x5a, 0xa2, 0x4f, 0x49, 0xa8, 0x1c,
  0xf9, 0x09, 0x1e, 0x95, 0x33, 0x3b, 0xa9, 0x75, 0xcc, 0x38, 0x7a, 0xb5, 0xc6, 0x67, 0xad, 0x59,
  0x36, 0xf4, 0x83, 0xfd, 0x27, 0x56, 0x5e, 0xd4, 0x6a, 0x6d, 0xd3, 0x6b, 0xc7, 0x9e, 0xad, 0x1e,
  0x57, 0x2f, 0xce, 0x78, 0xcc, 0x7f, 0xb8, 0xe4, 0xdf, 0xe8, 0xd5, 0x87, 0x55, 0xfb, 0xfc, 0xe1,
  0xd3, 0x93, 0x03, 0x69, 0x9e, 0x0e, 0x13, 0x33, 0x5f, 0x68, 0xbe, 0x03, 0x9c, 0x39, 0x96, 0xd8,
  0x06, 0xa5, 0xc3, 0xb5, 0x70, 0xc9, 0x40, 0x4d, 0xc2, 0x73, 0x46, 0x0a, 0x9f, 0x76, 0xd3, 0x9b,
  0xe7, 0xa2, 0x81, 0x2e, 0x4b, 0x19, 0xe1, 0xc2, 0x56, 0x92, 0xed, 0x5c, 0x18, 0xc7, 0xa2, 0xc3,
  0x7e, 0xdd, 0xf1, 0x93, 0xbe, 0xb6, 0x93, 0x77, 0xef, 0x3e, 0xa1, 0xdf, 0x9d, 0x3a, 0x5d, 0x40,
  0x06, 0xc3, 0xc7, 0x64, 0xc1, 0xac, 0x6f, 0x2a, 0x85, 0x2d, 0x3c, 0x79, 0x78, 0x3b, 0x22, 0x94,
  0xcc, 0xba, 0xe6, 0x74, 0x42, 0xab, 0x98, 0x56, 0xcc, 0x15, 0x61, 0x06, 0xa9, 0x95, 0x22, 0xc7,
  0xe8, 0xd8, 0x0c, 0x92, 0xe3, 0x71, 0xaf, 0x78, 0x70, 0x71, 0xa3, 0x3f, 0x78, 0xe3, 0xd6, 0x01,
  0xf6, 0xcd, 0xe2, 0xc1, 0x9f, 0xfa, 0x28, 0xaf, 0x8d, 0xf1, 0xfb, 0xa3, 0xce, 0x1b, 0x8f, 0x32,
  0x9f, 0x7b, 0x6a, 0xd4, 0xbd, 0x6f, 0x74, 0x29, 0x2c, 0x52, 0x26, 0x65, 0xf4, 0x6a, 0x70, 0x48,
  0x94, 0x61, 0xd6, 0xa3, 0x3f, 0xae, 0x6d, 0x3c, 0xfb, 0xb5, 0xe2, 0x4f, 0x2a, 0x30, 0x84, 0xf1,
  0x26, 0x0e, 0xd3, 0xaa, 0xa2, 0xe8, 0x95, 0x4f, 0xcd, 0x0c, 0xbc, 0xd4, 0x9b, 0x18, 0xb2, 0xad,
  0x4f, 0xe9, 0xf1, 0xaf, 0xb4, 0xb3, 0x19, 0xf9, 0xaf, 0xd9, 0x18, 0x66, 0x7c, 0x71, 0x48, 0xc2,
  0x70, 0x08, 0x53, 0xf0, 0x71, 0x48, 0xd9, 0x00, 0xc4, 0x06, 0xc6, 0x13, 0x38, 0x92, 0x41, 0x91,
  0x0b, 0x2c, 0x2f, 0xf5, 0x21, 0xe5, 0x26, 0x00, 0xea, 0x94, 0xb6, 0xa2, 0x48, 0x05, 0x00, 0x00
};

inline void serveDevicesHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", DEVICES_HTML, DEVICES_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// fingerprints.html
const uint16_t FINGERPRINTS_HTML_L = 672;
const uint8_t FINGERPRINTS_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0xa4, 0x93, 0x45, 0x97, 0xdb, 0x30,
  0x10, 0x80, 0xcf, 0xd9, 0x5f, 0x31, 0xab, 0x42, 0xec, 0xf7, 0x62, 0xb9, 0x8c, 0xf1, 0x96, 0x7b,
  0x29, 0x33, 0x83, 0x62, 0x4d, 0x62, 0x35, 0xb2, 0xe4, 0x27, 0xc9, 0xf6, 0xe2, 0x7f, 0xaf, 0x64,
  0x28, 0x73, 0x2f, 0xc2, 0x99, 0x6f, 0x78, 0xbe, 0xc9, 0x75, 0xee, 0x76, 0x2a, 0x84, 0xc2, 0x95,
  0x72, 0x6b, 0x63, 0x1e, 0x36, 0x90, 0x4c, 0xad, 0x32, 0x82, 0x8a, 0x6c, 0x6d, 0x4c, 0xe6, 0x05,
  0x32, 0xee, 0xf7, 0xc9, 0xbc, 0x44, 0xc7, 0x20, 0x2f, 0x98, 0xb1, 0xe8, 0x32, 0x52, 0xbb, 0x65,
  0x72, 0x8e, 0x40, 0xda, 0x7d, 0x49, 0xa1, 0xd6, 0x60, 0x50, 0x66, 0x44, 0xe4, 0x5a, 0x11, 0x28,
  0x0c, 0x2e, 0x33, 0x42, 0xd3, 0x25, 0x6b, 0xc2, 0x03, 0xb5, 0xcd, 0xaa, 0x17, 0x1d, 0x28, 0x8a,
  0x95, 0x98, 0x91, 0x46, 0x60, 0x5b, 0x69, 0xe3, 0x08, 0x78, 0x19, 0x87, 0xca, 0x53, 0x5b, 0xc1,
  0x5d, 0x91, 0x71, 0xf4, 0x6a, 0x98, 0x74, 0x97, 0x19, 0x08, 0x25, 0x9c, 0x60, 0x32, 0xb1, 0x39,
  0x93, 0x98, 0x1d, 0x1f, 0x41, 0x36, 0x37, 0xa2, 0x72, 0xe1, 0x38, 0x49, 0x53, 0xb8, 0xa7, 0xa0,
  0x62, 0x2b, 0x04, 0xa9, 0x19, 0x07, 0x6d, 0xa0, 0x2d, 0x50, 0x05, 0x67, 0xd5, 0x4a, 0xa8, 0x15,
  0xb8, 0x02, 0x4b, 0xb4, 0x33, 0x58, 0xa0, 0x75, 0xe0, 0x34, 0x30, 0xce, 0x3d, 0xd6, 0x7b, 0x8d,
  0x7e, 0x83, 0xf7, 0x21, 0xc4, 0xf7, 0xdd, 0x7b, 0xa3, 0x05, 0x87, 0x9b, 0xf7, 0x9e, 0x5c, 0x0b,
  0x58, 0xb1, 0x84, 0x48, 0x6a, 0x6f, 0xf5, 0x91, 0xd3, 0xc6, 0xc3, 0x69, 0x87, 0x81, 0x2c, 0xcb,
  0x60, 0xca, 0x99, 0x59, 0x4f, 0x61, 0x7f, 0x1f, 0xa2, 0xcd, 0x68, 0xda, 0xbd, 0x4f, 0x03, 0xea,
  0x4b, 0xf1, 0x18, 0x8e, 0x1e, 0x85, 0x56, 0x28, 0xae, 0x5b, 0x5a, 0x32, 0x97, 0x17, 0x77, 0x90,
  0x0b, 0x16, 0x4d, 0xa3, 0xca, 0x67, 0x07, 0x8d, 0x4d, 0x72, 0x2d, 0xb5, 0xf1, 0x61, 0x05, 0xed,
  0x0b, 0x10, 0x88, 0xf1, 0x34, 0xee, 0x45, 0xd1, 0xc6, 0x31, 0xec, 0x05, 0x27, 0x26, 0xbe, 0x40,
  0x75, 0x89, 0xca, 0xd1, 0xf1, 0x70, 0x43, 0x62, 0x77, 0xcf, 0x25, 0xb3, 0xf6, 0xb6, 0xb0, 0x8e,
  0xfa, 0x70, 0xa2, 0xde, 0xa3, 0xf8, 0x62, 0xd0, 0x39, 0x00, 0x94, 0x16, 0xff, 0x5c, 0xdf, 0x60,
  0xa9, 0x1b, 0xfc, 0x1a, 0x11, 0x52, 0x9c, 0x7e, 0xce, 0xf1, 0xa7, 0x2a, 0x8f, 0xa5, 0x65, 0x55,
  0x95, 0x8a, 0xb2, 0xac, 0x1d, 0x5b, 0x48, 0x4c, 0x3d, 0x09, 0x9d, 0x4d, 0x7d, 0xb0, 0xb8, 0x4d,
  0xaf, 0x2e, 0xd7, 0x0f, 0xee, 0xde, 0x7f, 0x7c, 0x82, 0xe6, 0xd6, 0x92, 0xbe, 0x2d, 0xac, 0xdb,
  0x91, 0x68, 0x0b, 0x44, 0x47, 0xbe, 0x69, 0x98, 0x52, 0xf3, 0x5a, 0xa2, 0x4f, 0x49, 0xa8, 0x1c,
  0xf9, 0x09, 0x1e, 0x95, 0x33, 0x3b, 0xa9, 0x75, 0xcc, 0x38, 0x7a, 0xb5, 0xc6, 0x67, 0xad, 0x59,
  0x36, 0xf4, 0x83, 0xfd, 0x27, 0x56, 0x5e, 0xd4, 0x6a, 0x6d, 0xd3, 0x6b, 0xc7, 0x9e, 0xad, 0x1e,
  0x57, 0x2f, 0xce, 0x78, 0xcc, 0x7f, 0xb8, 0xe4, 0xdf, 0xe8, 0xd5, 0x87, 0x55, 0xfb, 0xfc, 0xe1,
  0xd3, 0x93, 0x03, 0x69, 0x9e, 0x0e, 0x13, 0x33, 0x5f, 0x68, 0xbe, 0x03, 0x9c, 0x39, 0x96, 0xd8,
  0x06, 0xa5, 0xc3, 0xb5, 0x70, 0xc9, 0x40, 0x4d, 0xc2, 0x73, 0x46, 0x0a, 0x9f, 0x76, 0xd3, 0x9b,
  0xe7, 0xa2, 0x81, 0x2e, 0x4b, 0x19, 0xe1, 0xc2, 0x56, 0x92, 0xed, 0x5c, 0x18, 0xc7, 0xa2, 0xc3,
  0x7e, 0xdd, 0xf1, 0x93, 0xbe, 0xb6, 0x93, 0x77, 0xef, 0x3e, 0xa1, 0xdf, 0x9d, 0x3a, 0x5d, 0x40,
  0x06, 0xc3, 0xc7, 0x64, 0xc1, 0xac, 0x6f, 0x2a, 0x85, 0x2d, 0x3c, 0x79, 0x78, 0x3b, 0x22, 0x94,
  0xcc, 0xba, 0xe6, 0x74, 0x42, 0xab, 0x98, 0x56, 0xcc, 0x15, 0x61, 0x06, 0xa9, 0x95, 0x22, 0xc7,
  0xe8, 0xd8, 0x0c, 0x92, 0xe3, 0x71, 0xaf, 0x78, 0x70, 0x71, 0xa3, 0x3f, 0x78, 0xe3, 0xd6, 0x01,
  0xf6, 0xcd, 0xe2, 0xc1, 0x9f, 0xfa, 0x28, 0xaf, 0x8d, 0xf1, 0xfb, 0xa3, 0xce, 0x1b, 0x8f, 0x32,
  0x9f, 0x7b, 0x6a, 0xd4, 0xbd, 0x6f, 0x74, 0x29, 0x2c, 0x52, 0x26, 0x65, 0xf4, 0x6a, 0x70, 0x48,
  0x94, 0x61, 0xd6, 0xa3, 0x3f, 0xae, 0x6d, 0x3c, 0xfb, 0xb5, 0xe2, 0x4f, 0x2a, 0x30, 0x84, 0xf1,
  0x26, 0x0e, 0xd3, 0xaa, 0xa2, 0xe8, 0x95, 0x4f, 0xcd, 0x0c, 0xbc, 0xd4, 0x9b, 0x18, 0xb2, 0xad,
  0x4f, 0xe9, 0xf1, 0xaf, 0xb4, 0xb3, 0x19, 0xf9, 0xaf, 0xd9, 0x18, 0x66, 0x7c, 0x71, 0x48, 0xc2,
  0x70, 0x08, 0x53, 0xf0, 0x71, 0x48, 0xd9, 0x00, 0xc4, 0x06, 0xc6, 0x13, 0x38, 0x92, 0x41, 0x91,
  0x0b, 0x2c, 0x2f, 0xf5, 0x21, 0xe5, 0x26, 0x00, 0xea, 0x94, 0xb6, 0xa2, 0x48, 0x05, 0x00, 0x00
};

inline void serveFingerprintsHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", FINGERPRINTS_HTML, FINGERPRINTS_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// index.html
const uint16_t INDEX_HTML_L = 672;
const uint8_t INDEX_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0xa4, 0x93, 0x45, 0x97, 0xdb, 0x30,
  0x10, 0x80, 0xcf, 0xd9, 0x5f, 0x31, 0xab, 0x42, 0xec, 0xf7, 0x62, 0xb9, 0x8c, 0xf1, 0x96, 0x7b,
  0x29, 0x33, 0x83, 0x62, 0x4d, 0x62, 0x35, 0xb2, 0xe4, 0x27, 0xc9, 0xf6, 0xe2, 0x7f, 0xaf, 0x64,
  0x28, 0x73, 0x2f, 0xc2, 0x99, 0x6f, 0x78, 0xbe, 0xc9, 0x75, 0xee, 0x76, 0x2a, 0x84, 0xc2, 0x95,
  0x72, 0x6b, 0x63, 0x1e, 0x36, 0x90, 0x4c, 0xad, 0x32, 0x82, 0x8a, 0x6c, 0x6d, 0x4c, 0xe6, 0x05,
  0x32, 0xee, 0xf7, 0xc9, 0xbc, 0x44, 0xc7, 0x20, 0x2f, 0x98, 0xb1, 0xe8, 0x32, 0x52, 0xbb, 0x65,
  0x72, 0x8e, 0x40, 0xda, 0x7d, 0x49, 0xa1, 0xd6, 0x60, 0x50, 0x66, 0x44, 0xe4, 0x5a, 0x11, 0x28,
  0x0c, 0x2e, 0x33, 0x42, 0xd3, 0x25, 0x6b, 0xc2, 0x03, 0xb5, 0xcd, 0xaa, 0x17, 0x1d, 0x28, 0x8a,
  0x95, 0x98, 0x91, 0x46, 0x60, 0x5b, 0x69, 0xe3, 0x08, 0x78, 0x19, 0x87, 0xca, 0x53, 0x5b, 0xc1,
  0x5d, 0x91, 0x71, 0xf4, 0x6a, 0x98, 0x74, 0x97, 0x19, 0x08, 0x25, 0x9c, 0x60, 0x32, 0xb1, 0x39,
  0x93, 0x98, 0x1d, 0x1f, 0x41, 0x36, 0x37, 0xa2, 0x72, 0xe1, 0x38, 0x49, 0x53, 0xb8, 0xa7, 0xa0,
  0x62, 0x2b, 0x04, 0xa9, 0x19, 0x07, 0x6d, 0xa0, 0x2d, 0x50, 0x05, 0x67, 0xd5, 0x4a, 0xa8, 0x15,
  0xb8, 0x02, 0x4b, 0xb4, 0x33, 0x58, 0xa0, 0x75, 0xe0, 0x34, 0x30, 0xce, 0x3d, 0xd6, 0x7b, 0x8d,
  0x7e, 0x83, 0xf7, 0x21, 0xc4, 0xf7, 0xdd, 0x7b, 0xa3, 0x05, 0x87, 0x9b, 0xf7, 0x9e, 0x5c, 0x0b,
  0x58, 0xb1, 0x84, 0x48, 0x6a, 0x6f, 0xf5, 0x91, 0xd3, 0xc6, 0xc3, 0x69, 0x87, 0x81, 0x2c, 0xcb,
  0x60, 0xca, 0x99, 0x59, 0x4f, 0x61, 0x7f, 0x1f, 0xa2, 0xcd, 0x68, 0xda, 0xbd, 0x4f, 0x03, 0xea,
  0x4b, 0xf1, 0x18, 0x8e, 0x1e, 0x85, 0x56, 0x28, 0xae, 0x5b, 0x5a, 0x32, 0x97, 0x17, 0x77, 0x90,
  0x0b, 0x16, 0x4d, 0xa3, 0xca, 0x67, 0x07, 0x8d, 0x4d, 0x72, 0x2d, 0xb5, 0xf1, 0x61, 0x05, 0xed,
  0x0b, 0x10, 0x88, 0xf1, 0x34, 0xee, 0x45, 0xd1, 0xc6, 0x31, 0xec, 0x05, 0x27, 0x26, 0xbe, 0x40,
  0x75, 0x89, 0xca, 0xd1, 0xf1, 0x70, 0x43, 0x62, 0x77, 0xcf, 0x25, 0xb3, 0xf6, 0xb6, 0xb0, 0x8e,
  0xfa, 0x70, 0xa2, 0xde, 0xa3, 0xf8, 0x62, 0xd0, 0x39, 0x00, 0x94, 0x16, 0xff, 0x5c, 0xdf, 0x60,
  0xa9, 0x1b, 0xfc, 0x1a, 0x11, 0x52, 0x9c, 0x7e, 0xce, 0xf1, 0xa7, 0x2a, 0x8f, 0xa5, 0x65, 0x55,
  0x95, 0x8a, 0xb2, 0xac, 0x1d, 0x5b, 0x48, 0x4c, 0x3d, 0x09, 0x9d, 0x4d, 0x7d, 0xb0, 0xb8, 0x4d,
  0xaf, 0x2e, 0xd7, 0x0f, 0xee, 0xde, 0x7f, 0x7c, 0x82, 0xe6, 0xd6, 0x92, 0xbe, 0x2d, 0xac, 0xdb,
  0x91, 0x68, 0x0b, 0x44, 0x47, 0xbe, 0x69, 0x98, 0x52, 0xf3, 0x5a, 0xa2, 0x4f, 0x49, 0xa8, 0x1c,
  0xf9, 0x09, 0x1e, 0x95, 0x33, 0x3b, 0xa9, 0x75, 0xcc, 0x38, 0x7a, 0xb5, 0xc6, 0x67, 0xad, 0x59,
  0x36, 0xf4, 0x83, 0xfd, 0x27, 0x56, 0x5e, 0xd4, 0x6a, 0x6d, 0xd3, 0x6b, 0xc7, 0x9e, 0xad, 0x1e,
  0x57, 0x2f, 0xce, 0x78, 0xcc, 0x7f, 0xb8, 0xe4, 0xdf, 0xe8, 0xd5, 0x87, 0x55, 0xfb, 0xfc, 0xe1,
  0xd3, 0x93, 0x03, 0x69, 0x9e, 0x0e, 0x13, 0x33, 0x5f, 0x68, 0xbe, 0x03, 0x9c, 0x39, 0x96, 0xd8,
  0x06, 0xa5, 0xc3, 0xb5, 0x70, 0xc9, 0x40, 0x4d, 0xc2, 0x73, 0x46, 0x0a, 0x9f, 0x76, 0xd3, 0x9b,
  0xe7, 0xa2, 0x81, 0x2e, 0x4b, 0x19, 0xe1, 0xc2, 0x56, 0x92, 0xed, 0x5c, 0x18, 0xc7, 0xa2, 0xc3,
  0x7e, 0xdd, 0xf1, 0x93, 0xbe, 0xb6, 0x93, 0x77, 0xef, 0x3e, 0xa1, 0xdf, 0x9d, 0x3a, 0x5d, 0x40,
  0x06, 0xc3, 0xc7, 0x64, 0xc1, 0xac, 0x6f, 0x2a, 0x85, 0x2d, 0x3c, 0x79, 0x78, 0x3b, 0x22, 0x94,
  0xcc, 0xba, 0xe6, 0x74, 0x42, 0xab, 0x98, 0x56, 0xcc, 0x15, 0x61, 0x06, 0xa9, 0x95, 0x22, 0xc7,
  0xe8, 0xd8, 0x0c, 0x92, 0xe3, 0x71, 0xaf, 0x78, 0x70, 0x71, 0xa3, 0x3f, 0x78, 0xe3, 0xd6, 0x01,
  0xf6, 0xcd, 0xe2, 0xc1, 0x9f, 0xfa, 0x28, 0xaf, 0x8d, 0xf1, 0xfb, 0xa3, 0xce, 0x1b, 0x8f, 0x32,
  0x9f, 0x7b, 0x6a, 0xd4, 0xbd, 0x6f, 0x74, 0x29, 0x2c, 0x52, 0x26, 0x65, 0xf4, 0x6a, 0x70, 0x48,
  0x94, 0x61, 0xd6, 0xa3, 0x3f, 0xae, 0x6d, 0x3c, 0xfb, 0xb5, 0xe2, 0x4f, 0x2a, 0x30, 0x84, 0xf1,
  0x26, 0x0e, 0xd3, 0xaa, 0xa2, 0xe8, 0x95, 0x4f, 0xcd, 0x0c, 0xbc, 0xd4, 0x9b, 0x18, 0xb2, 0xad,
  0x4f, 0xe9, 0xf1, 0xaf, 0xb4, 0xb3, 0x19, 0xf9, 0xaf, 0xd9, 0x18, 0x66, 0x7c, 0x71, 0x48, 0xc2,
  0x70, 0x08, 0x53, 0xf0, 0x71, 0x48, 0xd9, 0x00, 0xc4, 0x06, 0xc6, 0x13, 0x38, 0x92, 0x41, 0x91,
  0x0b, 0x2c, 0x2f, 0xf5, 0x21, 0xe5, 0x26, 0x00, 0xea, 0x94, 0xb6, 0xa2, 0x48, 0x05, 0x00, 0x00
};

inline void serveIndexHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", INDEX_HTML, INDEX_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// network.html
const uint16_t NETWORK_HTML_L = 672;
const uint8_t NETWORK_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0xa4, 0x93, 0x45, 0x97, 0xdb, 0x30,
  0x10, 0x80, 0xcf, 0xd9, 0x5f, 0x31, 0xab, 0x42, 0xec, 0xf7, 0x62, 0xb9, 0x8c, 0xf1, 0x96, 0x7b,
  0x29, 0x33, 0x83, 0x62, 0x4d, 0x62, 0x35, 0xb2, 0xe4, 0x27, 0xc9, 0xf6, 0xe2, 0x7f, 0xaf, 0x64,
  0x28, 0x73, 0x2f, 0xc2, 0x99, 0x6f, 0x78, 0xbe, 0xc9, 0x75, 0xee, 0x76, 0x2a, 0x84, 0xc2, 0x95,
  0x72, 0x6b, 0x63, 0x1e, 0x36, 0x90, 0x4c, 0xad, 0x32, 0x82, 0x8a, 0x6c, 0x6d, 0x4c, 0xe6, 0x05,
  0x32, 0xee, 0xf7, 0xc9, 0xbc, 0x44, 0xc7, 0x20, 0x2f, 0x98, 0xb1, 0xe8, 0x32, 0x52, 0xbb, 0x65,
  0x72, 0x8e, 0x40, 0xda, 0x7d, 0x49, 0xa1, 0xd6, 0x60, 0x50, 0x66, 0x44, 0xe4, 0x5a, 0x11, 0x28,
  0x0c, 0x2e, 0x33, 0x42, 0xd3, 0x25, 0x6b, 0xc2, 0x03, 0xb5, 0xcd, 0xaa, 0x17, 0x1d, 0x28, 0x8a,
  0x95, 0x98, 0x91, 0x46, 0x60, 0x5b, 0x69, 0xe3, 0x08, 0x78, 0x19, 0x87, 0xca, 0x53, 0x5b, 0xc1,
  0x5d, 0x91, 0x71, 0xf4, 0x6a, 0x98, 0x74, 0x97, 0x19, 0x08, 0x25, 0x9c, 0x60, 0x32, 0xb1, 0x39,
  0x93, 0x98, 0x1d, 0x1f, 0x41, 0x36, 0x37, 0xa2, 0x72, 0xe1, 0x38, 0x49, 0x53, 0xb8, 0xa7, 0xa0,
  0x62, 0x2b, 0x04, 0xa9, 0x19, 0x07, 0x6d, 0xa0, 0x2d, 0x50, 0x05, 0x67, 0xd5, 0x4a, 0xa8, 0x15,
  0xb8, 0x02, 0x4b, 0xb4, 0x33, 0x58, 0xa0, 0x75, 0xe0, 0x34, 0x30, 0xce, 0x3d, 0xd6, 0x7b, 0x8d,
  0x7e, 0x83, 0xf7, 0x21, 0xc4, 0xf7, 0xdd, 0x7b, 0xa3, 0x05, 0x87, 0x9b, 0xf7, 0x9e, 0x5c, 0x0b,
  0x58, 0xb1, 0x84, 0x48, 0x6a, 0x6f, 0xf5, 0x91, 0xd3, 0xc6, 0xc3, 0x69, 0x87, 0x81, 0x2c, 0xcb,
  0x60, 0xca, 0x99, 0x59, 0x4f, 0x61, 0x7f, 0x1f, 0xa2, 0xcd, 0x68, 0xda, 0xbd, 0x4f, 0x03, 0xea,
  0x4b, 0xf1, 0x18, 0x8e, 0x1e, 0x85, 0x56, 0x28, 0xae, 0x5b, 0x5a, 0x32, 0x97, 0x17, 0x77, 0x90,
  0x0b, 0x16, 0x4d, 0xa3, 0xca, 0x67, 0x07, 0x8d, 0x4d, 0x72, 0x2d, 0xb5, 0xf1, 0x61, 0x05, 0xed,
  0x0b, 0x10, 0x88, 0xf1, 0x34, 0xee, 0x45, 0xd1, 0xc6, 0x31, 0xec, 0x05, 0x27, 0x26, 0xbe, 0x40,
  0x75, 0x89, 0xca, 0xd1, 0xf1, 0x70, 0x43, 0x62, 0x77, 0xcf, 0x25, 0xb3, 0xf6, 0xb6, 0xb0, 0x8e,
  0xfa, 0x70, 0xa2, 0xde, 0xa3, 0xf8, 0x62, 0xd0, 0x39, 0x00, 0x94, 0x16, 0xff, 0x5c, 0xdf, 0x60,
  0xa9, 0x1b, 0xfc, 0x1a, 0x11, 0x52, 0x9c, 0x7e, 0xce, 0xf1, 0xa7, 0x2a, 0x8f, 0xa5, 0x65, 0x55,
  0x95, 0x8a, 0xb2, 0xac, 0x1d, 0x5b, 0x48, 0x4c, 0x3d, 0x09, 0x9d, 0x4d, 0x7d, 0xb0, 0xb8, 0x4d,
  0xaf, 0x2e, 0xd7, 0x0f, 0xee, 0xde, 0x7f, 0x7c, 0x82, 0xe6, 0xd6, 0x92, 0xbe, 0x2d, 0xac, 0xdb,
  0x91, 0x68, 0x0b, 0x44, 0x47, 0xbe, 0x69, 0x98, 0x52, 0xf3, 0x5a, 0xa2, 0x4f, 0x49, 0xa8, 0x1c,
  0xf9, 0x09, 0x1e, 0x95, 0x33, 0x3b, 0xa9, 0x75, 0xcc, 0x38, 0x7a, 0xb5, 0xc6, 0x67, 0xad, 0x59,
  0x36, 0xf4, 0x83, 0xfd, 0x27, 0x56, 0x5e, 0xd4, 0x6a, 0x6d, 0xd3, 0x6b, 0xc7, 0x9e, 0xad, 0x1e,
  0x57, 0x2f, 0xce, 0x78, 0xcc, 0x7f, 0xb8, 0xe4, 0xdf, 0xe8, 0xd5, 0x87, 0x55, 0xfb, 0xfc, 0xe1,
  0xd3, 0x93, 0x03, 0x69, 0x9e, 0x0e, 0x13, 0x33, 0x5f, 0x68, 0xbe, 0x03, 0x9c, 0x39, 0x96, 0xd8,
  0x06, 0xa5, 0xc3, 0xb5, 0x70, 0xc9, 0x40, 0x4d, 0xc2, 0x73, 0x46, 0x0a, 0x9f, 0x76, 0xd3, 0x9b,
  0xe7, 0xa2, 0x81, 0x2e, 0x4b, 0x19, 0xe1, 0xc2, 0x56, 0x92, 0xed, 0x5c, 0x18, 0xc7, 0xa2, 0xc3,
  0x7e, 0xdd, 0xf1, 0x93, 0xbe, 0xb6, 0x93, 0x77, 0xef, 0x3e, 0xa1, 0xdf, 0x9d, 0x3a, 0x5d, 0x40,
  0x06, 0xc3, 0xc7, 0x64, 0xc1, 0xac, 0x6f, 0x2a, 0x85, 0x2d, 0x3c, 0x79, 0x78, 0x3b, 0x22, 0x94,
  0xcc, 0xba, 0xe6, 0x74, 0x42, 0xab, 0x98, 0x56, 0xcc, 0x15, 0x61, 0x06, 0xa9, 0x95, 0x22, 0xc7,
  0xe8, 0xd8, 0x0c, 0x92, 0xe3, 0x71, 0xaf, 0x78, 0x70, 0x71, 0xa3, 0x3f, 0x78, 0xe3, 0xd6, 0x01,
  0xf6, 0xcd, 0xe2, 0xc1, 0x9f, 0xfa, 0x28, 0xaf, 0x8d, 0xf1, 0xfb, 0xa3, 0xce, 0x1b, 0x8f, 0x32,
  0x9f, 0x7b, 0x6a, 0xd4, 0xbd, 0x6f, 0x74, 0x29, 0x2c, 0x52, 0x26, 0x65, 0xf4, 0x6a, 0x70, 0x48,
  0x94, 0x61, 0xd6, 0xa3, 0x3f, 0xae, 0x6d, 0x3c, 0xfb, 0xb5, 0xe2, 0x4f, 0x2a, 0x30, 0x84, 0xf1,
  0x26, 0x0e, 0xd3, 0xaa, 0xa2, 0xe8, 0x95, 0x4f, 0xcd, 0x0c, 0xbc, 0xd4, 0x9b, 0x18, 0xb2, 0xad,
  0x4f, 0xe9, 0xf1, 0xaf, 0xb4, 0xb3, 0x19, 0xf9, 0xaf, 0xd9, 0x18, 0x66, 0x7c, 0x71, 0x48, 0xc2,
  0x70, 0x08, 0x53, 0xf0, 0x71, 0x48, 0xd9, 0x00, 0xc4, 0x06, 0xc6, 0x13, 0x38, 0x92, 0x41, 0x91,
  0x0b, 0x2c, 0x2f, 0xf5, 0x21, 0xe5, 0x26, 0x00, 0xea, 0x94, 0xb6, 0xa2, 0x48, 0x05, 0x00, 0x00
};

inline void serveNetworkHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", NETWORK_HTML, NETWORK_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// settings.html
const uint16_t SETTINGS_HTML_L = 672;
const uint8_t SETTINGS_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0xa4, 0x93, 0x45, 0x97, 0xdb, 0x30,
  0x10, 0x80, 0xcf, 0xd9, 0x5f, 0x31, 0xab, 0x42, 0xec, 0xf7, 0x62, 0xb9, 0x8c, 0xf1, 0x96, 0x7b,
  0x29, 0x33, 0x83, 0x62, 0x4d, 0x62, 0x35, 0xb2, 0xe4, 0x27, 0xc9, 0xf6, 0xe2, 0x7f, 0xaf, 0x64,
  0x28, 0x73, 0x2f, 0xc2, 0x99, 0x6f, 0x78, 0xbe, 0xc9, 0x75, 0xee, 0x76, 0x2a, 0x84, 0xc2, 0x95,
  0x72, 0x6b, 0x63, 0x1e, 0x36, 0x90, 0x4c, 0xad, 0x32, 0x82, 0x8a, 0x6c, 0x6d, 0x4c, 0xe6, 0x05,
  0x32, 0xee, 0xf7, 0xc9, 0xbc, 0x44, 0xc7, 0x20, 0x2f, 0x98, 0xb1, 0xe8, 0x32, 0x52, 0xbb, 0x65,
  0x72, 0x8e, 0x40, 0xda, 0x7d, 0x49, 0xa1, 0xd6, 0x60, 0x50, 0x66, 0x44, 0xe4, 0x5a, 0x11, 0x28,
  0x0c, 0x2e, 0x33, 0x42, 0xd3, 0x25, 0x6b, 0xc2, 0x03, 0xb5, 0xcd, 0xaa, 0x17, 0x1d, 0x28, 0x8a,
  0x95, 0x98, 0x91, 0x46, 0x60, 0x5b, 0x69, 0xe3, 0x08, 0x78, 0x19, 0x87, 0xca, 0x53, 0x5b, 0xc1,
  0x5d, 0x91, 0x71, 0xf4, 0x6a, 0x98, 0x74, 0x97, 0x19, 0x08, 0x25, 0x9c, 0x60, 0x32, 0xb1, 0x39,
  0x93, 0x98, 0x1d, 0x1f, 0x41, 0x36, 0x37, 0xa2, 0x72, 0xe1, 0x38, 0x49, 0x53, 0xb8, 0xa7, 0xa0,
  0x62, 0x2b, 0x04, 0xa9, 0x19, 0x07, 0x6d, 0xa0, 0x2d, 0x50, 0x05, 0x67, 0xd5, 0x4a, 0xa8, 0x15,
  0xb8, 0x02, 0x4b, 0xb4, 0x33, 0x58, 0xa0, 0x75, 0xe0, 0x34, 0x30, 0xce, 0x3d, 0xd6, 0x7b, 0x8d,
  0x7e, 0x83, 0xf7, 0x21, 0xc4, 0xf7, 0xdd, 0x7b, 0xa3, 0x05, 0x87, 0x9b, 0xf7, 0x9e, 0x5c, 0x0b,
  0x58, 0xb1, 0x84, 0x48, 0x6a, 0x6f, 0xf5, 0x91, 0xd3, 0xc6, 0xc3, 0x69, 0x87, 0x81, 0x2c, 0xcb,
  0x60, 0xca, 0x99, 0x59, 0x4f, 0x61, 0x7f, 0x1f, 0xa2, 0xcd, 0x68, 0xda, 0xbd, 0x4f, 0x03, 0xea,
  0x4b, 0xf1, 0x18, 0x8e, 0x1e, 0x85, 0x56, 0x28, 0xae, 0x5b, 0x5a, 0x32, 0x97, 0x17, 0x77, 0x90,
  0x0b, 0x16, 0x4d, 0xa3, 0xca, 0x67, 0x07, 0x8d, 0x4d, 0x72, 0x2d, 0xb5, 0xf1, 0x61, 0x05, 0xed,
  0x0b, 0x10, 0x88, 0xf1, 0x34, 0xee, 0x45, 0xd1, 0xc6, 0x31, 0xec, 0x05, 0x27, 0x26, 0xbe, 0x40,
  0x75, 0x89, 0xca, 0xd1, 0xf1, 0x70, 0x43, 0x62, 0x77, 0xcf, 0x25, 0xb3, 0xf6, 0xb6, 0xb0, 0x8e,
  0xfa, 0x70, 0xa2, 0xde, 0xa3, 0xf8, 0x62, 0xd0, 0x39, 0x00, 0x94, 0x16, 0xff, 0x5c, 0xdf, 0x60,
  0xa9, 0x1b, 0xfc, 0x1a, 0x11, 0x52, 0x9c, 0x7e, 0xce, 0xf1, 0xa7, 0x2a, 0x8f, 0xa5, 0x65, 0x55,
  0x95, 0x8a, 0xb2, 0xac, 0x1d, 0x5b, 0x48, 0x4c, 0x3d, 0x09, 0x9d, 0x4d, 0x7d, 0xb0, 0xb8, 0x4d,
  0xaf, 0x2e, 0xd7, 0x0f, 0xee, 0xde, 0x7f, 0x7c, 0x82, 0xe6, 0xd6, 0x92, 0xbe, 0x2d, 0xac, 0xdb,
  0x91, 0x68, 0x0b, 0x44, 0x47, 0xbe, 0x69, 0x98, 0x52, 0xf3, 0x5a, 0xa2, 0x4f, 0x49, 0xa8, 0x1c,
  0xf9, 0x09, 0x1e, 0x95, 0x33, 0x3b, 0xa9, 0x75, 0xcc, 0x38, 0x7a, 0xb5, 0xc6, 0x67, 0xad, 0x59,
  0x36, 0xf4, 0x83, 0xfd, 0x27, 0x56, 0x5e, 0xd4, 0x6a, 0x6d, 0xd3, 0x6b, 0xc7, 0x9e, 0xad, 0x1e,
  0x57, 0x2f, 0xce, 0x78, 0xcc, 0x7f, 0xb8, 0xe4, 0xdf, 0xe8, 0xd5, 0x87, 0x55, 0xfb, 0xfc, 0xe1,
  0xd3, 0x93, 0x03, 0x69, 0x9e, 0x0e, 0x13, 0x33, 0x5f, 0x68, 0xbe, 0x03, 0x9c, 0x39, 0x96, 0xd8,
  0x06, 0xa5, 0xc3, 0xb5, 0x70, 0xc9, 0x40, 0x4d, 0xc2, 0x73, 0x46, 0x0a, 0x9f, 0x76, 0xd3, 0x9b,
  0xe7, 0xa2, 0x81, 0x2e, 0x4b, 0x19, 0xe1, 0xc2, 0x56, 0x92, 0xed, 0x5c, 0x18, 0xc7, 0xa2, 0xc3,
  0x7e, 0xdd, 0xf1, 0x93, 0xbe, 0xb6, 0x93, 0x77, 0xef, 0x3e, 0xa1, 0xdf, 0x9d, 0x3a, 0x5d, 0x40,
  0x06, 0xc3, 0xc7, 0x64, 0xc1, 0xac, 0x6f, 0x2a, 0x85, 0x2d, 0x3c, 0x79, 0x78, 0x3b, 0x22, 0x94,
  0xcc, 0xba, 0xe6, 0x74, 0x42, 0xab, 0x98, 0x56, 0xcc, 0x15, 0x61, 0x06, 0xa9, 0x95, 0x22, 0xc7,
  0xe8, 0xd8, 0x0c, 0x92, 0xe3, 0x71, 0xaf, 0x78, 0x70, 0x71, 0xa3, 0x3f, 0x78, 0xe3, 0xd6, 0x01,
  0xf6, 0xcd, 0xe2, 0xc1, 0x9f, 0xfa, 0x28, 0xaf, 0x8d, 0xf1, 0xfb, 0xa3, 0xce, 0x1b, 0x8f, 0x32,
  0x9f, 0x7b, 0x6a, 0xd4, 0xbd, 0x6f, 0x74, 0x29, 0x2c, 0x52, 0x26, 0x65, 0xf4, 0x6a, 0x70, 0x48,
  0x94, 0x61, 0xd6, 0xa3, 0x3f, 0xae, 0x6d, 0x3c, 0xfb, 0xb5, 0xe2, 0x4f, 0x2a, 0x30, 0x84, 0xf1,
  0x26, 0x0e, 0xd3, 0xaa, 0xa2, 0xe8, 0x95, 0x4f, 0xcd, 0x0c, 0xbc, 0xd4, 0x9b, 0x18, 0xb2, 0xad,
  0x4f, 0xe9, 0xf1, 0xaf, 0xb4, 0xb3, 0x19, 0xf9, 0xaf, 0xd9, 0x18, 0x66, 0x7c, 0x71, 0x48, 0xc2,
  0x70, 0x08, 0x53, 0xf0, 0x71, 0x48, 0xd9, 0x00, 0xc4, 0x06, 0xc6, 0x13, 0x38, 0x92, 0x41, 0x91,
  0x0b, 0x2c, 0x2f, 0xf5, 0x21, 0xe5, 0x26, 0x00, 0xea, 0x94, 0xb6, 0xa2, 0x48, 0x05, 0x00, 0x00
};

inline void serveSettingsHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", SETTINGS_HTML, SETTINGS_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

