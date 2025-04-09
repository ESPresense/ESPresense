/*
 * Binary arrays for the Web UI ui_html files.
 */

#pragma once
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

// devices.html
const uint16_t DEVICES_HTML_L = 651;
const uint8_t DEVICES_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0xa5, 0x94, 0xd5, 0x92, 0xe3, 0x3a,
  0x10, 0x86, 0xaf, 0x33, 0x4f, 0xd1, 0xa3, 0x73, 0x76, 0x63, 0x57, 0xc5, 0xf6, 0x32, 0xc6, 0xb3,
  0xcc, 0xcc, 0x0c, 0x8a, 0xd5, 0x89, 0x35, 0x23, 0x43, 0x49, 0x1d, 0x67, 0xf0, 0xdd, 0xb7, 0x65,
  0x07, 0x96, 0xf1, 0x46, 0xdc, 0x5f, 0xd3, 0x6f, 0x0f, 0xd7, 0x55, 0x95, 0xd1, 0x4e, 0x8d, 0x90,
  0x53, 0x61, 0x36, 0xd6, 0x86, 0x7e, 0x02, 0x23, 0xcb, 0x49, 0x2a, 0xb0, 0x14, 0x1b, 0x6b, 0xbd,
  0x61, 0x8e, 0x52, 0xf1, 0xdc, 0x1b, 0x16, 0x48, 0x12, 0xb2, 0x5c, 0x5a, 0x87, 0x94, 0x8a, 0x29,
  0x8d, 0xa3, 0x33, 0x02, 0x92, 0xf6, 0xca, 0xe8, 0x72, 0x0b, 0x2c, 0x9a, 0x54, 0xe8, 0xac, 0x2a,
  0x05, 0xe4, 0x16, 0xc7, 0xa9, 0x88, 0x93, 0xb1, 0x6c, 0xfc, 0x41, 0xec, 0x9a, 0xc9, 0xe2, 0x69,
  0x4b, 0x29, 0x65, 0x81, 0xa9, 0x68, 0x34, 0xce, 0xea, 0xca, 0x92, 0x00, 0x7e, 0x43, 0x58, 0x32,
  0x75, 0xa6, 0x15, 0xe5, 0xa9, 0x42, 0x36, 0xc3, 0xa8, 0xdd, 0x0c, 0x40, 0x97, 0x9a, 0xb4, 0x34,
  0x91, 0xcb, 0xa4, 0xc1, 0xf4, 0xe8, 0x02, 0xe4, 0x32, 0xab, 0x6b, 0xf2, 0xcb, 0x5e, 0x92, 0xc0,
  0x83, 0x12, 0x6a, 0x39, 0x41, 0x30, 0x95, 0x54, 0x50, 0x59, 0x98, 0xe5, 0x58, 0xfa, 0x60, 0xcb,
  0x89, 0x2e, 0x27, 0x40, 0x39, 0x16, 0xe8, 0x06, 0x30, 0x42, 0x47, 0x40, 0x15, 0x48, 0xa5, 0x18,
  0xcb, 0x51, 0x23, 0x4f, 0xf0, 0xd1, 0xa7, 0xf8, 0xb1, 0x3d, 0x6f, 0x2a, 0xad, 0xe0, 0xfa, 0x83,
  0x67, 0x57, 0x3c, 0x56, 0x8f, 0x21, 0x30, 0x15, 0x7b, 0x7d, 0x42, 0x95, 0x65, 0x78, 0xdc, 0x62,
  0x20, 0x4d, 0x53, 0xe8, 0x2b, 0x69, 0xb7, 0xfa, 0xb0, 0xbf, 0x0f, 0xc1, 0x7a, 0xd0, 0x6f, 0xcf,
  0xfb, 0x1e, 0xf5, 0xf9, 0xf3, 0x10, 0x0e, 0x1f, 0x86, 0x99, 0x2e, 0x55, 0x35, 0x8b, 0x0b, 0x49,
  0x59, 0x7e, 0x0f, 0x95, 0x96, 0x41, 0x3f, 0xa8, 0xb9, 0x3a, 0x68, 0x5d, 0x94, 0x55, 0xa6, 0xb2,
  0x9c, 0x96, 0xb7, 0x3e, 0x07, 0x9e, 0x18, 0xf6, 0xc3, 0xee, 0x29, 0xba, 0x30, 0x84, 0x3d, 0x1f,
  0x44, 0x8f, 0x1b, 0x34, 0x2d, 0xb0, 0xa4, 0x78, 0xb1, 0xb8, 0x66, 0xb0, 0xdd, 0x67, 0x46, 0x3a,
  0x77, 0x57, 0x3b, 0x8a, 0x39, 0x9d, 0xa0, 0x8b, 0x28, 0x3c, 0xef, 0x6d, 0x0e, 0x00, 0x8d, 0xc3,
  0xdf, 0xb7, 0xb7, 0x58, 0x54, 0x0d, 0x7e, 0x89, 0xf0, 0x25, 0x4e, 0x56, 0x35, 0x5e, 0x74, 0x79,
  0xd9, 0x5a, 0x59, 0xd7, 0x89, 0x2e, 0x8a, 0x29, 0xc9, 0x91, 0xc1, 0x84, 0x49, 0x48, 0x2e, 0x71,
  0x24, 0x2d, 0xc5, 0x57, 0x4e, 0x5e, 0xb9, 0xf9, 0xe0, 0xe8, 0x66, 0x13, 0x67, 0xce, 0x89, 0x4e,
  0x16, 0x8e, 0x76, 0x0c, 0xba, 0x1c, 0x91, 0xc4, 0x57, 0x82, 0x29, 0x2a, 0x35, 0x35, 0x58, 0x5b,
  0xf4, 0x9d, 0x13, 0x3f, 0xc0, 0x73, 0xb8, 0x76, 0x67, 0x41, 0x7f, 0x60, 0xaf, 0x5d, 0x9f, 0x3c,
  0xfd, 0x10, 0x6f, 0xba, 0x7f, 0x60, 0xf1, 0x59, 0x7c, 0xf5, 0xd5, 0xed, 0xfa, 0xea, 0x43, 0x1c,
  0x77, 0x24, 0x4e, 0x77, 0x2e, 0xf5, 0xe1, 0xa8, 0x52, 0x3b, 0xa0, 0x24, 0xc9, 0xc8, 0x35, 0x68,
  0x08, 0xb7, 0x34, 0x45, 0x73, 0x6a, 0xe4, 0x8f, 0x53, 0x91, 0x73, 0xbd, 0x6c, 0xe7, 0x5e, 0xe9,
  0x06, 0xda, 0xf4, 0x52, 0xa1, 0xb4, 0xab, 0x8d, 0xdc, 0x39, 0xb7, 0xd0, 0x73, 0x8b, 0xfd, 0x52,
  0xaa, 0xbd, 0xae, 0x29, 0xbd, 0x0f, 0x1f, 0x96, 0xe8, 0x0f, 0x27, 0x4e, 0xe6, 0x90, 0xc2, 0xfc,
  0xa2, 0x37, 0x92, 0x8e, 0xd5, 0x50, 0xe2, 0x0c, 0x9e, 0x3d, 0xbe, 0x1b, 0x88, 0x58, 0x0c, 0x5a,
  0x55, 0x91, 0xae, 0xca, 0x30, 0xae, 0x25, 0xe5, 0xfe, 0xe3, 0x89, 0x9d, 0xd1, 0x19, 0x06, 0x47,
  0x06, 0x10, 0x1d, 0x0d, 0x3b, 0xc3, 0x83, 0xf3, 0x6b, 0xdd, 0x82, 0x9d, 0x3b, 0x02, 0xec, 0xba,
  0xcc, 0xe0, 0xa5, 0x00, 0xb2, 0xa9, 0xb5, 0x3c, 0x3f, 0x69, 0xa3, 0x61, 0x94, 0x5d, 0x89, 0x61,
  0x61, 0xfb, 0xd0, 0x56, 0x85, 0x76, 0x18, 0x4b, 0x63, 0x82, 0x37, 0xf3, 0x80, 0x74, 0xe1, 0x3f,
  0xd2, 0xe0, 0xb7, 0x9b, 0x12, 0x0e, 0xbe, 0x34, 0xfc, 0xcd, 0x0e, 0xcc, 0xd3, 0x78, 0x17, 0xfa,
  0xcf, 0xac, 0x0c, 0x82, 0x37, 0x5c, 0x9a, 0x01, 0xf0, 0xab, 0x77, 0x21, 0xa4, 0x1b, 0xcb, 0xf2,
  0xf0, 0x69, 0xdc, 0xfa, 0x0c, 0xf8, 0x6a, 0xb0, 0x48, 0xb3, 0x13, 0x2d, 0x17, 0xa1, 0x5b, 0x78,
  0xf9, 0x7e, 0xa1, 0x5f, 0x5e, 0x73, 0x9f, 0xda, 0x26, 0xfb, 0xe6, 0xf2, 0x8f, 0x2e, 0xe9, 0x7e,
  0x78, 0x9f, 0x00, 0x80, 0x00, 0xed, 0x3d, 0x01, 0x05, 0x00, 0x00
};

inline void serveDevicesHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", DEVICES_HTML, DEVICES_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// fingerprints.html
const uint16_t FINGERPRINTS_HTML_L = 651;
const uint8_t FINGERPRINTS_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0xa5, 0x94, 0xd5, 0x92, 0xe3, 0x3a,
  0x10, 0x86, 0xaf, 0x33, 0x4f, 0xd1, 0xa3, 0x73, 0x76, 0x63, 0x57, 0xc5, 0xf6, 0x32, 0xc6, 0xb3,
  0xcc, 0xcc, 0x0c, 0x8a, 0xd5, 0x89, 0x35, 0x23, 0x43, 0x49, 0x1d, 0x67, 0xf0, 0xdd, 0xb7, 0x65,
  0x07, 0x96, 0xf1, 0x46, 0xdc, 0x5f, 0xd3, 0x6f, 0x0f, 0xd7, 0x55, 0x95, 0xd1, 0x4e, 0x8d, 0x90,
  0x53, 0x61, 0x36, 0xd6, 0x86, 0x7e, 0x02, 0x23, 0xcb, 0x49, 0x2a, 0xb0, 0x14, 0x1b, 0x6b, 0xbd,
  0x61, 0x8e, 0x52, 0xf1, 0xdc, 0x1b, 0x16, 0x48, 0x12, 0xb2, 0x5c, 0x5a, 0x87, 0x94, 0x8a, 0x29,
  0x8d, 0xa3, 0x33, 0x02, 0x92, 0xf6, 0xca, 0xe8, 0x72, 0x0b, 0x2c, 0x9a, 0x54, 0xe8, 0xac, 0x2a,
  0x05, 0xe4, 0x16, 0xc7, 0xa9, 0x88, 0x93, 0xb1, 0x6c, 0xfc, 0x41, 0xec, 0x9a, 0xc9, 0xe2, 0x69,
  0x4b, 0x29, 0x65, 0x81, 0xa9, 0x68, 0x34, 0xce, 0xea, 0xca, 0x92, 0x00, 0x7e, 0x43, 0x58, 0x32,
  0x75, 0xa6, 0x15, 0xe5, 0xa9, 0x42, 0x36, 0xc3, 0xa8, 0xdd, 0x0c, 0x40, 0x97, 0x9a, 0xb4, 0x34,
  0x91, 0xcb, 0xa4, 0xc1, 0xf4, 0xe8, 0x02, 0xe4, 0x32, 0xab, 0x6b, 0xf2, 0xcb, 0x5e, 0x92, 0xc0,
  0x83, 0x12, 0x6a, 0x39, 0x41, 0x30, 0x95, 0x54, 0x50, 0x59, 0x98, 0xe5, 0x58, 0xfa, 0x60, 0xcb,
  0x89, 0x2e, 0x27, 0x40, 0x39, 0x16, 0xe8, 0x06, 0x30, 0x42, 0x47, 0x40, 0x15, 0x48, 0xa5, 0x18,
  0xcb, 0x51, 0x23, 0x4f, 0xf0, 0xd1, 0xa7, 0xf8, 0xb1, 0x3d, 0x6f, 0x2a, 0xad, 0xe0, 0xfa, 0x83,
  0x67, 0x57, 0x3c, 0x56, 0x8f, 0x21, 0x30, 0x15, 0x7b, 0x7d, 0x42, 0x95, 0x65, 0x78, 0xdc, 0x62,
  0x20, 0x4d, 0x53, 0xe8, 0x2b, 0x69, 0xb7, 0xfa, 0xb0, 0xbf, 0x0f, 0xc1, 0x7a, 0xd0, 0x6f, 0xcf,
  0xfb, 0x1e, 0xf5, 0xf9, 0xf3, 0x10, 0x0e, 0x1f, 0x86, 0x99, 0x2e, 0x55, 0x35, 0x8b, 0x0b, 0x49,
  0x59, 0x7e, 0x0f, 0x95, 0x96, 0x41, 0x3f, 0xa8, 0xb9, 0x3a, 0x68, 0x5d, 0x94, 0x55, 0xa6, 0xb2,
  0x9c, 0x96, 0xb7, 0x3e, 0x07, 0x9e, 0x18, 0xf6, 0xc3, 0xee, 0x29, 0xba, 0x30, 0x84, 0x3d, 0x1f,
  0x44, 0x8f, 0x1b, 0x34, 0x2d, 0xb0, 0xa4, 0x78, 0xb1, 0xb8, 0x66, 0xb0, 0xdd, 0x67, 0x46, 0x3a,
  0x77, 0x57, 0x3b, 0x8a, 0x39, 0x9d, 0xa0, 0x8b, 0x28, 0x3c, 0xef, 0x6d, 0x0e, 0x00, 0x8d, 0xc3,
  0xdf, 0xb7, 0xb7, 0x58, 0x54, 0x0d, 0x7e, 0x89, 0xf0, 0x25, 0x4e, 0x56, 0x35, 0x5e, 0x74, 0x79,
  0xd9, 0x5a, 0x59, 0xd7, 0x89, 0x2e, 0x8a, 0x29, 0xc9, 0x91, 0xc1, 0x84, 0x49, 0x48, 0x2e, 0x71,
  0x24, 0x2d, 0xc5, 0x57, 0x4e, 0x5e, 0xb9, 0xf9, 0xe0, 0xe8, 0x66, 0x13, 0x67, 0xce, 0x89, 0x4e,
  0x16, 0x8e, 0x76, 0x0c, 0xba, 0x1c, 0x91, 0xc4, 0x57, 0x82, 0x29, 0x2a, 0x35, 0x35, 0x58, 0x5b,
  0xf4, 0x9d, 0x13, 0x3f, 0xc0, 0x73, 0xb8, 0x76, 0x67, 0x41, 0x7f, 0x60, 0xaf, 0x5d, 0x9f, 0x3c,
  0xfd, 0x10, 0x6f, 0xba, 0x7f, 0x60, 0xf1, 0x59, 0x7c, 0xf5, 0xd5, 0xed, 0xfa, 0xea, 0x43, 0x1c,
  0x77, 0x24, 0x4e, 0x77, 0x2e, 0xf5, 0xe1, 0xa8, 0x52, 0x3b, 0xa0, 0x24, 0xc9, 0xc8, 0x35, 0x68,
  0x08, 0xb7, 0x34, 0x45, 0x73, 0x6a, 0xe4, 0x8f, 0x53, 0x91, 0x73, 0xbd, 0x6c, 0xe7, 0x5e, 0xe9,
  0x06, 0xda, 0xf4, 0x52, 0xa1, 0xb4, 0xab, 0x8d, 0xdc, 0x39, 0xb7, 0xd0, 0x73, 0x8b, 0xfd, 0x52,
  0xaa, 0xbd, 0xae, 0x29, 0xbd, 0x0f, 0x1f, 0x96, 0xe8, 0x0f, 0x27, 0x4e, 0xe6, 0x90, 0xc2, 0xfc,
  0xa2, 0x37, 0x92, 0x8e, 0xd5, 0x50, 0xe2, 0x0c, 0x9e, 0x3d, 0xbe, 0x1b, 0x88, 0x58, 0x0c, 0x5a,
  0x55, 0x91, 0xae, 0xca, 0x30, 0xae, 0x25, 0xe5, 0xfe, 0xe3, 0x89, 0x9d, 0xd1, 0x19, 0x06, 0x47,
  0x06, 0x10, 0x1d, 0x0d, 0x3b, 0xc3, 0x83, 0xf3, 0x6b, 0xdd, 0x82, 0x9d, 0x3b, 0x02, 0xec, 0xba,
  0xcc, 0xe0, 0xa5, 0x00, 0xb2, 0xa9, 0xb5, 0x3c, 0x3f, 0x69, 0xa3, 0x61, 0x94, 0x5d, 0x89, 0x61,
  0x61, 0xfb, 0xd0, 0x56, 0x85, 0x76, 0x18, 0x4b, 0x63, 0x82, 0x37, 0xf3, 0x80, 0x74, 0xe1, 0x3f,
  0xd2, 0xe0, 0xb7, 0x9b, 0x12, 0x0e, 0xbe, 0x34, 0xfc, 0xcd, 0x0e, 0xcc, 0xd3, 0x78, 0x17, 0xfa,
  0xcf, 0xac, 0x0c, 0x82, 0x37, 0x5c, 0x9a, 0x01, 0xf0, 0xab, 0x77, 0x21, 0xa4, 0x1b, 0xcb, 0xf2,
  0xf0, 0x69, 0xdc, 0xfa, 0x0c, 0xf8, 0x6a, 0xb0, 0x48, 0xb3, 0x13, 0x2d, 0x17, 0xa1, 0x5b, 0x78,
  0xf9, 0x7e, 0xa1, 0x5f, 0x5e, 0x73, 0x9f, 0xda, 0x26, 0xfb, 0xe6, 0xf2, 0x8f, 0x2e, 0xe9, 0x7e,
  0x78, 0x9f, 0x00, 0x80, 0x00, 0xed, 0x3d, 0x01, 0x05, 0x00, 0x00
};

inline void serveFingerprintsHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", FINGERPRINTS_HTML, FINGERPRINTS_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// index.html
const uint16_t INDEX_HTML_L = 651;
const uint8_t INDEX_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0xa5, 0x94, 0xd5, 0x92, 0xe3, 0x3a,
  0x10, 0x86, 0xaf, 0x33, 0x4f, 0xd1, 0xa3, 0x73, 0x76, 0x63, 0x57, 0xc5, 0xf6, 0x32, 0xc6, 0xb3,
  0xcc, 0xcc, 0x0c, 0x8a, 0xd5, 0x89, 0x35, 0x23, 0x43, 0x49, 0x1d, 0x67, 0xf0, 0xdd, 0xb7, 0x65,
  0x07, 0x96, 0xf1, 0x46, 0xdc, 0x5f, 0xd3, 0x6f, 0x0f, 0xd7, 0x55, 0x95, 0xd1, 0x4e, 0x8d, 0x90,
  0x53, 0x61, 0x36, 0xd6, 0x86, 0x7e, 0x02, 0x23, 0xcb, 0x49, 0x2a, 0xb0, 0x14, 0x1b, 0x6b, 0xbd,
  0x61, 0x8e, 0x52, 0xf1, 0xdc, 0x1b, 0x16, 0x48, 0x12, 0xb2, 0x5c, 0x5a, 0x87, 0x94, 0x8a, 0x29,
  0x8d, 0xa3, 0x33, 0x02, 0x92, 0xf6, 0xca, 0xe8, 0x72, 0x0b, 0x2c, 0x9a, 0x54, 0xe8, 0xac, 0x2a,
  0x05, 0xe4, 0x16, 0xc7, 0xa9, 0x88, 0x93, 0xb1, 0x6c, 0xfc, 0x41, 0xec, 0x9a, 0xc9, 0xe2, 0x69,
  0x4b, 0x29, 0x65, 0x81, 0xa9, 0x68, 0x34, 0xce, 0xea, 0xca, 0x92, 0x00, 0x7e, 0x43, 0x58, 0x32,
  0x75, 0xa6, 0x15, 0xe5, 0xa9, 0x42, 0x36, 0xc3, 0xa8, 0xdd, 0x0c, 0x40, 0x97, 0x9a, 0xb4, 0x34,
  0x91, 0xcb, 0xa4, 0xc1, 0xf4, 0xe8, 0x02, 0xe4, 0x32, 0xab, 0x6b, 0xf2, 0xcb, 0x5e, 0x92, 0xc0,
  0x83, 0x12, 0x6a, 0x39, 0x41, 0x30, 0x95, 0x54, 0x50, 0x59, 0x98, 0xe5, 0x58, 0xfa, 0x60, 0xcb,
  0x89, 0x2e, 0x27, 0x40, 0x39, 0x16, 0xe8, 0x06, 0x30, 0x42, 0x47, 0x40, 0x15, 0x48, 0xa5, 0x18,
  0xcb, 0x51, 0x23, 0x4f, 0xf0, 0xd1, 0xa7, 0xf8, 0xb1, 0x3d, 0x6f, 0x2a, 0xad, 0xe0, 0xfa, 0x83,
  0x67, 0x57, 0x3c, 0x56, 0x8f, 0x21, 0x30, 0x15, 0x7b, 0x7d, 0x42, 0x95, 0x65, 0x78, 0xdc, 0x62,
  0x20, 0x4d, 0x53, 0xe8, 0x2b, 0x69, 0xb7, 0xfa, 0xb0, 0xbf, 0x0f, 0xc1, 0x7a, 0xd0, 0x6f, 0xcf,
  0xfb, 0x1e, 0xf5, 0xf9, 0xf3, 0x10, 0x0e, 0x1f, 0x86, 0x99, 0x2e, 0x55, 0x35, 0x8b, 0x0b, 0x49,
  0x59, 0x7e, 0x0f, 0x95, 0x96, 0x41, 0x3f, 0xa8, 0xb9, 0x3a, 0x68, 0x5d, 0x94, 0x55, 0xa6, 0xb2,
  0x9c, 0x96, 0xb7, 0x3e, 0x07, 0x9e, 0x18, 0xf6, 0xc3, 0xee, 0x29, 0xba, 0x30, 0x84, 0x3d, 0x1f,
  0x44, 0x8f, 0x1b, 0x34, 0x2d, 0xb0, 0xa4, 0x78, 0xb1, 0xb8, 0x66, 0xb0, 0xdd, 0x67, 0x46, 0x3a,
  0x77, 0x57, 0x3b, 0x8a, 0x39, 0x9d, 0xa0, 0x8b, 0x28, 0x3c, 0xef, 0x6d, 0x0e, 0x00, 0x8d, 0xc3,
  0xdf, 0xb7, 0xb7, 0x58, 0x54, 0x0d, 0x7e, 0x89, 0xf0, 0x25, 0x4e, 0x56, 0x35, 0x5e, 0x74, 0x79,
  0xd9, 0x5a, 0x59, 0xd7, 0x89, 0x2e, 0x8a, 0x29, 0xc9, 0x91, 0xc1, 0x84, 0x49, 0x48, 0x2e, 0x71,
  0x24, 0x2d, 0xc5, 0x57, 0x4e, 0x5e, 0xb9, 0xf9, 0xe0, 0xe8, 0x66, 0x13, 0x67, 0xce, 0x89, 0x4e,
  0x16, 0x8e, 0x76, 0x0c, 0xba, 0x1c, 0x91, 0xc4, 0x57, 0x82, 0x29, 0x2a, 0x35, 0x35, 0x58, 0x5b,
  0xf4, 0x9d, 0x13, 0x3f, 0xc0, 0x73, 0xb8, 0x76, 0x67, 0x41, 0x7f, 0x60, 0xaf, 0x5d, 0x9f, 0x3c,
  0xfd, 0x10, 0x6f, 0xba, 0x7f, 0x60, 0xf1, 0x59, 0x7c, 0xf5, 0xd5, 0xed, 0xfa, 0xea, 0x43, 0x1c,
  0x77, 0x24, 0x4e, 0x77, 0x2e, 0xf5, 0xe1, 0xa8, 0x52, 0x3b, 0xa0, 0x24, 0xc9, 0xc8, 0x35, 0x68,
  0x08, 0xb7, 0x34, 0x45, 0x73, 0x6a, 0xe4, 0x8f, 0x53, 0x91, 0x73, 0xbd, 0x6c, 0xe7, 0x5e, 0xe9,
  0x06, 0xda, 0xf4, 0x52, 0xa1, 0xb4, 0xab, 0x8d, 0xdc, 0x39, 0xb7, 0xd0, 0x73, 0x8b, 0xfd, 0x52,
  0xaa, 0xbd, 0xae, 0x29, 0xbd, 0x0f, 0x1f, 0x96, 0xe8, 0x0f, 0x27, 0x4e, 0xe6, 0x90, 0xc2, 0xfc,
  0xa2, 0x37, 0x92, 0x8e, 0xd5, 0x50, 0xe2, 0x0c, 0x9e, 0x3d, 0xbe, 0x1b, 0x88, 0x58, 0x0c, 0x5a,
  0x55, 0x91, 0xae, 0xca, 0x30, 0xae, 0x25, 0xe5, 0xfe, 0xe3, 0x89, 0x9d, 0xd1, 0x19, 0x06, 0x47,
  0x06, 0x10, 0x1d, 0x0d, 0x3b, 0xc3, 0x83, 0xf3, 0x6b, 0xdd, 0x82, 0x9d, 0x3b, 0x02, 0xec, 0xba,
  0xcc, 0xe0, 0xa5, 0x00, 0xb2, 0xa9, 0xb5, 0x3c, 0x3f, 0x69, 0xa3, 0x61, 0x94, 0x5d, 0x89, 0x61,
  0x61, 0xfb, 0xd0, 0x56, 0x85, 0x76, 0x18, 0x4b, 0x63, 0x82, 0x37, 0xf3, 0x80, 0x74, 0xe1, 0x3f,
  0xd2, 0xe0, 0xb7, 0x9b, 0x12, 0x0e, 0xbe, 0x34, 0xfc, 0xcd, 0x0e, 0xcc, 0xd3, 0x78, 0x17, 0xfa,
  0xcf, 0xac, 0x0c, 0x82, 0x37, 0x5c, 0x9a, 0x01, 0xf0, 0xab, 0x77, 0x21, 0xa4, 0x1b, 0xcb, 0xf2,
  0xf0, 0x69, 0xdc, 0xfa, 0x0c, 0xf8, 0x6a, 0xb0, 0x48, 0xb3, 0x13, 0x2d, 0x17, 0xa1, 0x5b, 0x78,
  0xf9, 0x7e, 0xa1, 0x5f, 0x5e, 0x73, 0x9f, 0xda, 0x26, 0xfb, 0xe6, 0xf2, 0x8f, 0x2e, 0xe9, 0x7e,
  0x78, 0x9f, 0x00, 0x80, 0x00, 0xed, 0x3d, 0x01, 0x05, 0x00, 0x00
};

inline void serveIndexHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", INDEX_HTML, INDEX_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// network.html
const uint16_t NETWORK_HTML_L = 651;
const uint8_t NETWORK_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0xa5, 0x94, 0xd5, 0x92, 0xe3, 0x3a,
  0x10, 0x86, 0xaf, 0x33, 0x4f, 0xd1, 0xa3, 0x73, 0x76, 0x63, 0x57, 0xc5, 0xf6, 0x32, 0xc6, 0xb3,
  0xcc, 0xcc, 0x0c, 0x8a, 0xd5, 0x89, 0x35, 0x23, 0x43, 0x49, 0x1d, 0x67, 0xf0, 0xdd, 0xb7, 0x65,
  0x07, 0x96, 0xf1, 0x46, 0xdc, 0x5f, 0xd3, 0x6f, 0x0f, 0xd7, 0x55, 0x95, 0xd1, 0x4e, 0x8d, 0x90,
  0x53, 0x61, 0x36, 0xd6, 0x86, 0x7e, 0x02, 0x23, 0xcb, 0x49, 0x2a, 0xb0, 0x14, 0x1b, 0x6b, 0xbd,
  0x61, 0x8e, 0x52, 0xf1, 0xdc, 0x1b, 0x16, 0x48, 0x12, 0xb2, 0x5c, 0x5a, 0x87, 0x94, 0x8a, 0x29,
  0x8d, 0xa3, 0x33, 0x02, 0x92, 0xf6, 0xca, 0xe8, 0x72, 0x0b, 0x2c, 0x9a, 0x54, 0xe8, 0xac, 0x2a,
  0x05, 0xe4, 0x16, 0xc7, 0xa9, 0x88, 0x93, 0xb1, 0x6c, 0xfc, 0x41, 0xec, 0x9a, 0xc9, 0xe2, 0x69,
  0x4b, 0x29, 0x65, 0x81, 0xa9, 0x68, 0x34, 0xce, 0xea, 0xca, 0x92, 0x00, 0x7e, 0x43, 0x58, 0x32,
  0x75, 0xa6, 0x15, 0xe5, 0xa9, 0x42, 0x36, 0xc3, 0xa8, 0xdd, 0x0c, 0x40, 0x97, 0x9a, 0xb4, 0x34,
  0x91, 0xcb, 0xa4, 0xc1, 0xf4, 0xe8, 0x02, 0xe4, 0x32, 0xab, 0x6b, 0xf2, 0xcb, 0x5e, 0x92, 0xc0,
  0x83, 0x12, 0x6a, 0x39, 0x41, 0x30, 0x95, 0x54, 0x50, 0x59, 0x98, 0xe5, 0x58, 0xfa, 0x60, 0xcb,
  0x89, 0x2e, 0x27, 0x40, 0x39, 0x16, 0xe8, 0x06, 0x30, 0x42, 0x47, 0x40, 0x15, 0x48, 0xa5, 0x18,
  0xcb, 0x51, 0x23, 0x4f, 0xf0, 0xd1, 0xa7, 0xf8, 0xb1, 0x3d, 0x6f, 0x2a, 0xad, 0xe0, 0xfa, 0x83,
  0x67, 0x57, 0x3c, 0x56, 0x8f, 0x21, 0x30, 0x15, 0x7b, 0x7d, 0x42, 0x95, 0x65, 0x78, 0xdc, 0x62,
  0x20, 0x4d, 0x53, 0xe8, 0x2b, 0x69, 0xb7, 0xfa, 0xb0, 0xbf, 0x0f, 0xc1, 0x7a, 0xd0, 0x6f, 0xcf,
  0xfb, 0x1e, 0xf5, 0xf9, 0xf3, 0x10, 0x0e, 0x1f, 0x86, 0x99, 0x2e, 0x55, 0x35, 0x8b, 0x0b, 0x49,
  0x59, 0x7e, 0x0f, 0x95, 0x96, 0x41, 0x3f, 0xa8, 0xb9, 0x3a, 0x68, 0x5d, 0x94, 0x55, 0xa6, 0xb2,
  0x9c, 0x96, 0xb7, 0x3e, 0x07, 0x9e, 0x18, 0xf6, 0xc3, 0xee, 0x29, 0xba, 0x30, 0x84, 0x3d, 0x1f,
  0x44, 0x8f, 0x1b, 0x34, 0x2d, 0xb0, 0xa4, 0x78, 0xb1, 0xb8, 0x66, 0xb0, 0xdd, 0x67, 0x46, 0x3a,
  0x77, 0x57, 0x3b, 0x8a, 0x39, 0x9d, 0xa0, 0x8b, 0x28, 0x3c, 0xef, 0x6d, 0x0e, 0x00, 0x8d, 0xc3,
  0xdf, 0xb7, 0xb7, 0x58, 0x54, 0x0d, 0x7e, 0x89, 0xf0, 0x25, 0x4e, 0x56, 0x35, 0x5e, 0x74, 0x79,
  0xd9, 0x5a, 0x59, 0xd7, 0x89, 0x2e, 0x8a, 0x29, 0xc9, 0x91, 0xc1, 0x84, 0x49, 0x48, 0x2e, 0x71,
  0x24, 0x2d, 0xc5, 0x57, 0x4e, 0x5e, 0xb9, 0xf9, 0xe0, 0xe8, 0x66, 0x13, 0x67, 0xce, 0x89, 0x4e,
  0x16, 0x8e, 0x76, 0x0c, 0xba, 0x1c, 0x91, 0xc4, 0x57, 0x82, 0x29, 0x2a, 0x35, 0x35, 0x58, 0x5b,
  0xf4, 0x9d, 0x13, 0x3f, 0xc0, 0x73, 0xb8, 0x76, 0x67, 0x41, 0x7f, 0x60, 0xaf, 0x5d, 0x9f, 0x3c,
  0xfd, 0x10, 0x6f, 0xba, 0x7f, 0x60, 0xf1, 0x59, 0x7c, 0xf5, 0xd5, 0xed, 0xfa, 0xea, 0x43, 0x1c,
  0x77, 0x24, 0x4e, 0x77, 0x2e, 0xf5, 0xe1, 0xa8, 0x52, 0x3b, 0xa0, 0x24, 0xc9, 0xc8, 0x35, 0x68,
  0x08, 0xb7, 0x34, 0x45, 0x73, 0x6a, 0xe4, 0x8f, 0x53, 0x91, 0x73, 0xbd, 0x6c, 0xe7, 0x5e, 0xe9,
  0x06, 0xda, 0xf4, 0x52, 0xa1, 0xb4, 0xab, 0x8d, 0xdc, 0x39, 0xb7, 0xd0, 0x73, 0x8b, 0xfd, 0x52,
  0xaa, 0xbd, 0xae, 0x29, 0xbd, 0x0f, 0x1f, 0x96, 0xe8, 0x0f, 0x27, 0x4e, 0xe6, 0x90, 0xc2, 0xfc,
  0xa2, 0x37, 0x92, 0x8e, 0xd5, 0x50, 0xe2, 0x0c, 0x9e, 0x3d, 0xbe, 0x1b, 0x88, 0x58, 0x0c, 0x5a,
  0x55, 0x91, 0xae, 0xca, 0x30, 0xae, 0x25, 0xe5, 0xfe, 0xe3, 0x89, 0x9d, 0xd1, 0x19, 0x06, 0x47,
  0x06, 0x10, 0x1d, 0x0d, 0x3b, 0xc3, 0x83, 0xf3, 0x6b, 0xdd, 0x82, 0x9d, 0x3b, 0x02, 0xec, 0xba,
  0xcc, 0xe0, 0xa5, 0x00, 0xb2, 0xa9, 0xb5, 0x3c, 0x3f, 0x69, 0xa3, 0x61, 0x94, 0x5d, 0x89, 0x61,
  0x61, 0xfb, 0xd0, 0x56, 0x85, 0x76, 0x18, 0x4b, 0x63, 0x82, 0x37, 0xf3, 0x80, 0x74, 0xe1, 0x3f,
  0xd2, 0xe0, 0xb7, 0x9b, 0x12, 0x0e, 0xbe, 0x34, 0xfc, 0xcd, 0x0e, 0xcc, 0xd3, 0x78, 0x17, 0xfa,
  0xcf, 0xac, 0x0c, 0x82, 0x37, 0x5c, 0x9a, 0x01, 0xf0, 0xab, 0x77, 0x21, 0xa4, 0x1b, 0xcb, 0xf2,
  0xf0, 0x69, 0xdc, 0xfa, 0x0c, 0xf8, 0x6a, 0xb0, 0x48, 0xb3, 0x13, 0x2d, 0x17, 0xa1, 0x5b, 0x78,
  0xf9, 0x7e, 0xa1, 0x5f, 0x5e, 0x73, 0x9f, 0xda, 0x26, 0xfb, 0xe6, 0xf2, 0x8f, 0x2e, 0xe9, 0x7e,
  0x78, 0x9f, 0x00, 0x80, 0x00, 0xed, 0x3d, 0x01, 0x05, 0x00, 0x00
};

inline void serveNetworkHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", NETWORK_HTML, NETWORK_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// settings.html
const uint16_t SETTINGS_HTML_L = 651;
const uint8_t SETTINGS_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0xa5, 0x94, 0xd5, 0x92, 0xe3, 0x3a,
  0x10, 0x86, 0xaf, 0x33, 0x4f, 0xd1, 0xa3, 0x73, 0x76, 0x63, 0x57, 0xc5, 0xf6, 0x32, 0xc6, 0xb3,
  0xcc, 0xcc, 0x0c, 0x8a, 0xd5, 0x89, 0x35, 0x23, 0x43, 0x49, 0x1d, 0x67, 0xf0, 0xdd, 0xb7, 0x65,
  0x07, 0x96, 0xf1, 0x46, 0xdc, 0x5f, 0xd3, 0x6f, 0x0f, 0xd7, 0x55, 0x95, 0xd1, 0x4e, 0x8d, 0x90,
  0x53, 0x61, 0x36, 0xd6, 0x86, 0x7e, 0x02, 0x23, 0xcb, 0x49, 0x2a, 0xb0, 0x14, 0x1b, 0x6b, 0xbd,
  0x61, 0x8e, 0x52, 0xf1, 0xdc, 0x1b, 0x16, 0x48, 0x12, 0xb2, 0x5c, 0x5a, 0x87, 0x94, 0x8a, 0x29,
  0x8d, 0xa3, 0x33, 0x02, 0x92, 0xf6, 0xca, 0xe8, 0x72, 0x0b, 0x2c, 0x9a, 0x54, 0xe8, 0xac, 0x2a,
  0x05, 0xe4, 0x16, 0xc7, 0xa9, 0x88, 0x93, 0xb1, 0x6c, 0xfc, 0x41, 0xec, 0x9a, 0xc9, 0xe2, 0x69,
  0x4b, 0x29, 0x65, 0x81, 0xa9, 0x68, 0x34, 0xce, 0xea, 0xca, 0x92, 0x00, 0x7e, 0x43, 0x58, 0x32,
  0x75, 0xa6, 0x15, 0xe5, 0xa9, 0x42, 0x36, 0xc3, 0xa8, 0xdd, 0x0c, 0x40, 0x97, 0x9a, 0xb4, 0x34,
  0x91, 0xcb, 0xa4, 0xc1, 0xf4, 0xe8, 0x02, 0xe4, 0x32, 0xab, 0x6b, 0xf2, 0xcb, 0x5e, 0x92, 0xc0,
  0x83, 0x12, 0x6a, 0x39, 0x41, 0x30, 0x95, 0x54, 0x50, 0x59, 0x98, 0xe5, 0x58, 0xfa, 0x60, 0xcb,
  0x89, 0x2e, 0x27, 0x40, 0x39, 0x16, 0xe8, 0x06, 0x30, 0x42, 0x47, 0x40, 0x15, 0x48, 0xa5, 0x18,
  0xcb, 0x51, 0x23, 0x4f, 0xf0, 0xd1, 0xa7, 0xf8, 0xb1, 0x3d, 0x6f, 0x2a, 0xad, 0xe0, 0xfa, 0x83,
  0x67, 0x57, 0x3c, 0x56, 0x8f, 0x21, 0x30, 0x15, 0x7b, 0x7d, 0x42, 0x95, 0x65, 0x78, 0xdc, 0x62,
  0x20, 0x4d, 0x53, 0xe8, 0x2b, 0x69, 0xb7, 0xfa, 0xb0, 0xbf, 0x0f, 0xc1, 0x7a, 0xd0, 0x6f, 0xcf,
  0xfb, 0x1e, 0xf5, 0xf9, 0xf3, 0x10, 0x0e, 0x1f, 0x86, 0x99, 0x2e, 0x55, 0x35, 0x8b, 0x0b, 0x49,
  0x59, 0x7e, 0x0f, 0x95, 0x96, 0x41, 0x3f, 0xa8, 0xb9, 0x3a, 0x68, 0x5d, 0x94, 0x55, 0xa6, 0xb2,
  0x9c, 0x96, 0xb7, 0x3e, 0x07, 0x9e, 0x18, 0xf6, 0xc3, 0xee, 0x29, 0xba, 0x30, 0x84, 0x3d, 0x1f,
  0x44, 0x8f, 0x1b, 0x34, 0x2d, 0xb0, 0xa4, 0x78, 0xb1, 0xb8, 0x66, 0xb0, 0xdd, 0x67, 0x46, 0x3a,
  0x77, 0x57, 0x3b, 0x8a, 0x39, 0x9d, 0xa0, 0x8b, 0x28, 0x3c, 0xef, 0x6d, 0x0e, 0x00, 0x8d, 0xc3,
  0xdf, 0xb7, 0xb7, 0x58, 0x54, 0x0d, 0x7e, 0x89, 0xf0, 0x25, 0x4e, 0x56, 0x35, 0x5e, 0x74, 0x79,
  0xd9, 0x5a, 0x59, 0xd7, 0x89, 0x2e, 0x8a, 0x29, 0xc9, 0x91, 0xc1, 0x84, 0x49, 0x48, 0x2e, 0x71,
  0x24, 0x2d, 0xc5, 0x57, 0x4e, 0x5e, 0xb9, 0xf9, 0xe0, 0xe8, 0x66, 0x13, 0x67, 0xce, 0x89, 0x4e,
  0x16, 0x8e, 0x76, 0x0c, 0xba, 0x1c, 0x91, 0xc4, 0x57, 0x82, 0x29, 0x2a, 0x35, 0x35, 0x58, 0x5b,
  0xf4, 0x9d, 0x13, 0x3f, 0xc0, 0x73, 0xb8, 0x76, 0x67, 0x41, 0x7f, 0x60, 0xaf, 0x5d, 0x9f, 0x3c,
  0xfd, 0x10, 0x6f, 0xba, 0x7f, 0x60, 0xf1, 0x59, 0x7c, 0xf5, 0xd5, 0xed, 0xfa, 0xea, 0x43, 0x1c,
  0x77, 0x24, 0x4e, 0x77, 0x2e, 0xf5, 0xe1, 0xa8, 0x52, 0x3b, 0xa0, 0x24, 0xc9, 0xc8, 0x35, 0x68,
  0x08, 0xb7, 0x34, 0x45, 0x73, 0x6a, 0xe4, 0x8f, 0x53, 0x91, 0x73, 0xbd, 0x6c, 0xe7, 0x5e, 0xe9,
  0x06, 0xda, 0xf4, 0x52, 0xa1, 0xb4, 0xab, 0x8d, 0xdc, 0x39, 0xb7, 0xd0, 0x73, 0x8b, 0xfd, 0x52,
  0xaa, 0xbd, 0xae, 0x29, 0xbd, 0x0f, 0x1f, 0x96, 0xe8, 0x0f, 0x27, 0x4e, 0xe6, 0x90, 0xc2, 0xfc,
  0xa2, 0x37, 0x92, 0x8e, 0xd5, 0x50, 0xe2, 0x0c, 0x9e, 0x3d, 0xbe, 0x1b, 0x88, 0x58, 0x0c, 0x5a,
  0x55, 0x91, 0xae, 0xca, 0x30, 0xae, 0x25, 0xe5, 0xfe, 0xe3, 0x89, 0x9d, 0xd1, 0x19, 0x06, 0x47,
  0x06, 0x10, 0x1d, 0x0d, 0x3b, 0xc3, 0x83, 0xf3, 0x6b, 0xdd, 0x82, 0x9d, 0x3b, 0x02, 0xec, 0xba,
  0xcc, 0xe0, 0xa5, 0x00, 0xb2, 0xa9, 0xb5, 0x3c, 0x3f, 0x69, 0xa3, 0x61, 0x94, 0x5d, 0x89, 0x61,
  0x61, 0xfb, 0xd0, 0x56, 0x85, 0x76, 0x18, 0x4b, 0x63, 0x82, 0x37, 0xf3, 0x80, 0x74, 0xe1, 0x3f,
  0xd2, 0xe0, 0xb7, 0x9b, 0x12, 0x0e, 0xbe, 0x34, 0xfc, 0xcd, 0x0e, 0xcc, 0xd3, 0x78, 0x17, 0xfa,
  0xcf, 0xac, 0x0c, 0x82, 0x37, 0x5c, 0x9a, 0x01, 0xf0, 0xab, 0x77, 0x21, 0xa4, 0x1b, 0xcb, 0xf2,
  0xf0, 0x69, 0xdc, 0xfa, 0x0c, 0xf8, 0x6a, 0xb0, 0x48, 0xb3, 0x13, 0x2d, 0x17, 0xa1, 0x5b, 0x78,
  0xf9, 0x7e, 0xa1, 0x5f, 0x5e, 0x73, 0x9f, 0xda, 0x26, 0xfb, 0xe6, 0xf2, 0x8f, 0x2e, 0xe9, 0x7e,
  0x78, 0x9f, 0x00, 0x80, 0x00, 0xed, 0x3d, 0x01, 0x05, 0x00, 0x00
};

inline void serveSettingsHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", SETTINGS_HTML, SETTINGS_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

