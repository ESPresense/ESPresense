/*
 * Binary arrays for the Web UI ui_html files.
 */

#pragma once
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

// devices.html
const uint16_t DEVICES_HTML_L = 673;
const uint8_t DEVICES_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x9c, 0x93, 0xe5, 0x96, 0xe3, 0x3a,
  0x0c, 0x80, 0x7f, 0x77, 0x9e, 0x42, 0xe3, 0x0b, 0x4d, 0xce, 0x69, 0x9c, 0xcb, 0xd8, 0xcc, 0xc5,
  0x65, 0x66, 0x06, 0x37, 0x56, 0x1a, 0x6f, 0x1d, 0x3b, 0xc7, 0x56, 0xdb, 0xc1, 0x77, 0x5f, 0x3b,
  0xb0, 0xcc, 0x7f, 0x8c, 0xd2, 0x27, 0x9e, 0xef, 0x4a, 0x5b, 0xd2, 0x41, 0x8b, 0x50, 0x53, 0xa3,
  0xf7, 0x76, 0xe6, 0x71, 0x03, 0x2d, 0xcc, 0xb2, 0x60, 0x68, 0xd8, 0xde, 0xce, 0x64, 0x5e, 0xa3,
  0x90, 0x61, 0x9f, 0xcc, 0x1b, 0x24, 0x01, 0x65, 0x2d, 0x9c, 0x47, 0x2a, 0xd8, 0x9a, 0xaa, 0xec,
  0x37, 0x06, 0x79, 0xf7, 0xa5, 0x95, 0x59, 0x81, 0x43, 0x5d, 0x30, 0x55, 0x5a, 0xc3, 0xa0, 0x76,
  0x58, 0x15, 0x8c, 0xe7, 0x95, 0xd8, 0xc4, 0x07, 0xee, 0x37, 0xcb, 0x41, 0xb4, 0xa7, 0x18, 0xd1,
  0x60, 0xc1, 0x36, 0x0a, 0xb7, 0xad, 0x75, 0xc4, 0x20, 0xc8, 0x10, 0x9a, 0x40, 0xdd, 0x2a, 0x49,
  0x75, 0x21, 0x31, 0xa8, 0x61, 0xd6, 0x5d, 0x66, 0xa0, 0x8c, 0x22, 0x25, 0x74, 0xe6, 0x4b, 0xa1,
  0xb1, 0xf8, 0x7e, 0x04, 0xf9, 0xd2, 0xa9, 0x96, 0xe2, 0x71, 0x92, 0xe7, 0x70, 0xc5, 0x40, 0x2b,
  0x96, 0x08, 0xda, 0x0a, 0x09, 0xd6, 0xc1, 0xb6, 0x46, 0x13, 0x9d, 0x35, 0x4b, 0x65, 0x96, 0x40,
  0x35, 0x36, 0xe8, 0x67, 0xb0, 0x40, 0x4f, 0x40, 0x16, 0x84, 0x94, 0x01, 0x1b, 0xbc, 0xc6, 0xb0,
  0xc1, 0xd3, 0x18, 0xe2, 0xd3, 0xee, 0x7d, 0x63, 0x95, 0x84, 0xd3, 0x57, 0x6e, 0xfd, 0x17, 0xb1,
  0xaa, 0x82, 0x44, 0xdb, 0x60, 0xf5, 0x06, 0x59, 0x17, 0xe0, 0xbc, 0xc3, 0x40, 0x51, 0x14, 0x30,
  0x95, 0xc2, 0xad, 0xa6, 0x70, 0x7c, 0x0c, 0xc9, 0x6e, 0x32, 0xed, 0xde, 0xa7, 0x11, 0xf5, 0xaa,
  0x78, 0x0a, 0xdf, 0x7e, 0x0b, 0x5b, 0x65, 0xa4, 0xdd, 0xf2, 0x46, 0x50, 0x59, 0x5f, 0x42, 0xa9,
  0x44, 0x32, 0x4d, 0xda, 0x90, 0x1d, 0x74, 0x3e, 0x2b, 0xad, 0xb6, 0x2e, 0x84, 0x15, 0xb5, 0xff,
  0x80, 0x48, 0x4c, 0xa7, 0x69, 0x2f, 0x8a, 0x3e, 0x4d, 0xe1, 0x28, 0x3a, 0x31, 0x09, 0x05, 0x5a,
  0x37, 0x68, 0x88, 0x8f, 0x87, 0x53, 0x1a, 0xbb, 0x7b, 0xa9, 0x85, 0xf7, 0x17, 0x95, 0x27, 0x1e,
  0xc2, 0x49, 0x7a, 0x8f, 0xd2, 0x3f, 0xa3, 0xce, 0x09, 0xa0, 0xf6, 0xf8, 0xe9, 0xfa, 0x0e, 0x1b,
  0xbb, 0xc1, 0xd7, 0x11, 0x31, 0xc5, 0xf9, 0xcb, 0x1c, 0x8f, 0x55, 0x7e, 0x51, 0x5a, 0xd1, 0xb6,
  0xb9, 0x6a, 0x9a, 0x35, 0x89, 0x85, 0xc6, 0x3c, 0x90, 0x90, 0x7c, 0x1e, 0x82, 0xc5, 0x7d, 0xfe,
  0x6f, 0xb5, 0xba, 0x76, 0xf9, 0xea, 0xcd, 0x1f, 0x78, 0xe9, 0x3d, 0xeb, 0xdb, 0xc2, 0xd3, 0x81,
  0x46, 0x5f, 0x23, 0x12, 0x7b, 0xa3, 0x61, 0x1a, 0x2b, 0xd7, 0x1a, 0x43, 0x4a, 0x62, 0xe5, 0xd8,
  0x7b, 0xf0, 0x68, 0xc8, 0x1d, 0xe4, 0x9e, 0x84, 0x23, 0xfe, 0xff, 0x9d, 0x5b, 0xd5, 0xfa, 0xa2,
  0x5e, 0xf1, 0x67, 0xfe, 0x8b, 0x58, 0x65, 0xbd, 0x36, 0x2b, 0x9f, 0xff, 0x5f, 0x5d, 0xb8, 0xd4,
  0x5e, 0xbe, 0xf4, 0x63, 0xc4, 0x7c, 0xb9, 0x4b, 0xe1, 0x8d, 0x57, 0xdf, 0xef, 0xff, 0xf3, 0x1b,
  0xdd, 0xfb, 0x77, 0x20, 0xcd, 0xf3, 0x61, 0x62, 0xe6, 0x0b, 0x2b, 0x0f, 0x40, 0x0a, 0x12, 0x99,
  0xdf, 0xa0, 0x26, 0x5c, 0x29, 0xca, 0x06, 0x6a, 0x16, 0x9f, 0x0b, 0x56, 0x87, 0xb4, 0xbb, 0xde,
  0xbc, 0x54, 0x1b, 0xe8, 0xb2, 0x54, 0x30, 0xa9, 0x7c, 0xab, 0xc5, 0xc1, 0x1f, 0xe3, 0x58, 0x74,
  0xd8, 0xd7, 0x3b, 0x7e, 0xd2, 0xd7, 0x76, 0xf2, 0xe4, 0xc9, 0x0b, 0xf4, 0x93, 0x9f, 0x7e, 0xae,
  0xa1, 0x80, 0xe1, 0x63, 0xb2, 0x10, 0x3e, 0x34, 0x95, 0xc1, 0x2d, 0xdc, 0xba, 0x7e, 0x31, 0x61,
  0x9c, 0xcd, 0xba, 0xe6, 0x24, 0x65, 0x4d, 0xca, 0x5b, 0x41, 0x75, 0x9c, 0x41, 0xee, 0xb5, 0x2a,
  0x31, 0xf9, 0x6e, 0x06, 0xd9, 0xf7, 0x69, 0xaf, 0x78, 0xf2, 0xe7, 0x4e, 0x7f, 0x08, 0xc6, 0x3d,
  0x01, 0xf6, 0xcd, 0x12, 0xc0, 0x2f, 0xfa, 0xa8, 0x5c, 0x3b, 0x17, 0xf6, 0x1b, 0x9d, 0x37, 0x01,
  0xe5, 0x5e, 0xf6, 0xd4, 0xa8, 0x7b, 0xd5, 0xd9, 0x46, 0x79, 0xe4, 0x42, 0xeb, 0xe4, 0xc1, 0xe0,
  0x90, 0x6a, 0xe2, 0xac, 0x27, 0x9f, 0x5c, 0xdb, 0x74, 0xf6, 0x49, 0x8a, 0x6f, 0x56, 0x60, 0x08,
  0xe3, 0x51, 0x1a, 0xa7, 0xd5, 0x24, 0xc9, 0x83, 0x90, 0x9a, 0x19, 0x04, 0xa9, 0x47, 0x29, 0x14,
  0x7b, 0x2f, 0xd2, 0x13, 0x5e, 0x79, 0x67, 0x33, 0x09, 0x5f, 0xb3, 0x31, 0xcc, 0xf4, 0xcf, 0x21,
  0x09, 0xf1, 0x30, 0x4c, 0xc1, 0xf3, 0x21, 0x65, 0x03, 0x10, 0x1b, 0x18, 0x4f, 0xe0, 0x48, 0x06,
  0x45, 0x2e, 0xb0, 0xbc, 0xd4, 0x87, 0x94, 0x9b, 0x00, 0xef, 0xa3, 0x83, 0x4a, 0x48, 0x05, 0x00,
  0x00
};

inline void serveDevicesHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", DEVICES_HTML, DEVICES_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// fingerprints.html
const uint16_t FINGERPRINTS_HTML_L = 673;
const uint8_t FINGERPRINTS_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x9c, 0x93, 0xe5, 0x96, 0xe3, 0x3a,
  0x0c, 0x80, 0x7f, 0x77, 0x9e, 0x42, 0xe3, 0x0b, 0x4d, 0xce, 0x69, 0x9c, 0xcb, 0xd8, 0xcc, 0xc5,
  0x65, 0x66, 0x06, 0x37, 0x56, 0x1a, 0x6f, 0x1d, 0x3b, 0xc7, 0x56, 0xdb, 0xc1, 0x77, 0x5f, 0x3b,
  0xb0, 0xcc, 0x7f, 0x8c, 0xd2, 0x27, 0x9e, 0xef, 0x4a, 0x5b, 0xd2, 0x41, 0x8b, 0x50, 0x53, 0xa3,
  0xf7, 0x76, 0xe6, 0x71, 0x03, 0x2d, 0xcc, 0xb2, 0x60, 0x68, 0xd8, 0xde, 0xce, 0x64, 0x5e, 0xa3,
  0x90, 0x61, 0x9f, 0xcc, 0x1b, 0x24, 0x01, 0x65, 0x2d, 0x9c, 0x47, 0x2a, 0xd8, 0x9a, 0xaa, 0xec,
  0x37, 0x06, 0x79, 0xf7, 0xa5, 0x95, 0x59, 0x81, 0x43, 0x5d, 0x30, 0x55, 0x5a, 0xc3, 0xa0, 0x76,
  0x58, 0x15, 0x8c, 0xe7, 0x95, 0xd8, 0xc4, 0x07, 0xee, 0x37, 0xcb, 0x41, 0xb4, 0xa7, 0x18, 0xd1,
  0x60, 0xc1, 0x36, 0x0a, 0xb7, 0xad, 0x75, 0xc4, 0x20, 0xc8, 0x10, 0x9a, 0x40, 0xdd, 0x2a, 0x49,
  0x75, 0x21, 0x31, 0xa8, 0x61, 0xd6, 0x5d, 0x66, 0xa0, 0x8c, 0x22, 0x25, 0x74, 0xe6, 0x4b, 0xa1,
  0xb1, 0xf8, 0x7e, 0x04, 0xf9, 0xd2, 0xa9, 0x96, 0xe2, 0x71, 0x92, 0xe7, 0x70, 0xc5, 0x40, 0x2b,
  0x96, 0x08, 0xda, 0x0a, 0x09, 0xd6, 0xc1, 0xb6, 0x46, 0x13, 0x9d, 0x35, 0x4b, 0x65, 0x96, 0x40,
  0x35, 0x36, 0xe8, 0x67, 0xb0, 0x40, 0x4f, 0x40, 0x16, 0x84, 0x94, 0x01, 0x1b, 0xbc, 0xc6, 0xb0,
  0xc1, 0xd3, 0x18, 0xe2, 0xd3, 0xee, 0x7d, 0x63, 0x95, 0x84, 0xd3, 0x57, 0x6e, 0xfd, 0x17, 0xb1,
  0xaa, 0x82, 0x44, 0xdb, 0x60, 0xf5, 0x06, 0x59, 0x17, 0xe0, 0xbc, 0xc3, 0x40, 0x51, 0x14, 0x30,
  0x95, 0xc2, 0xad, 0xa6, 0x70, 0x7c, 0x0c, 0xc9, 0x6e, 0x32, 0xed, 0xde, 0xa7, 0x11, 0xf5, 0xaa,
  0x78, 0x0a, 0xdf, 0x7e, 0x0b, 0x5b, 0x65, 0xa4, 0xdd, 0xf2, 0x46, 0x50, 0x59, 0x5f, 0x42, 0xa9,
  0x44, 0x32, 0x4d, 0xda, 0x90, 0x1d, 0x74, 0x3e, 0x2b, 0xad, 0xb6, 0x2e, 0x84, 0x15, 0xb5, 0xff,
  0x80, 0x48, 0x4c, 0xa7, 0x69, 0x2f, 0x8a, 0x3e, 0x4d, 0xe1, 0x28, 0x3a, 0x31, 0x09, 0x05, 0x5a,
  0x37, 0x68, 0x88, 0x8f, 0x87, 0x53, 0x1a, 0xbb, 0x7b, 0xa9, 0x85, 0xf7, 0x17, 0x95, 0x27, 0x1e,
  0xc2, 0x49, 0x7a, 0x8f, 0xd2, 0x3f, 0xa3, 0xce, 0x09, 0xa0, 0xf6, 0xf8, 0xe9, 0xfa, 0x0e, 0x1b,
  0xbb, 0xc1, 0xd7, 0x11, 0x31, 0xc5, 0xf9, 0xcb, 0x1c, 0x8f, 0x55, 0x7e, 0x51, 0x5a, 0xd1, 0xb6,
  0xb9, 0x6a, 0x9a, 0x35, 0x89, 0x85, 0xc6, 0x3c, 0x90, 0x90, 0x7c, 0x1e, 0x82, 0xc5, 0x7d, 0xfe,
  0x6f, 0xb5, 0xba, 0x76, 0xf9, 0xea, 0xcd, 0x1f, 0x78, 0xe9, 0x3d, 0xeb, 0xdb, 0xc2, 0xd3, 0x81,
  0x46, 0x5f, 0x23, 0x12, 0x7b, 0xa3, 0x61, 0x1a, 0x2b, 0xd7, 0x1a, 0x43, 0x4a, 0x62, 0xe5, 0xd8,
  0x7b, 0xf0, 0x68, 0xc8, 0x1d, 0xe4, 0x9e, 0x84, 0x23, 0xfe, 0xff, 0x9d, 0x5b, 0xd5, 0xfa, 0xa2,
  0x5e, 0xf1, 0x67, 0xfe, 0x8b, 0x58, 0x65, 0xbd, 0x36, 0x2b, 0x9f, 0xff, 0x5f, 0x5d, 0xb8, 0xd4,
  0x5e, 0xbe, 0xf4, 0x63, 0xc4, 0x7c, 0xb9, 0x4b, 0xe1, 0x8d, 0x57, 0xdf, 0xef, 0xff, 0xf3, 0x1b,
  0xdd, 0xfb, 0x77, 0x20, 0xcd, 0xf3, 0x61, 0x62, 0xe6, 0x0b, 0x2b, 0x0f, 0x40, 0x0a, 0x12, 0x99,
  0xdf, 0xa0, 0x26, 0x5c, 0x29, 0xca, 0x06, 0x6a, 0x16, 0x9f, 0x0b, 0x56, 0x87, 0xb4, 0xbb, 0xde,
  0xbc, 0x54, 0x1b, 0xe8, 0xb2, 0x54, 0x30, 0xa9, 0x7c, 0xab, 0xc5, 0xc1, 0x1f, 0xe3, 0x58, 0x74,
  0xd8, 0xd7, 0x3b, 0x7e, 0xd2, 0xd7, 0x76, 0xf2, 0xe4, 0xc9, 0x0b, 0xf4, 0x93, 0x9f, 0x7e, 0xae,
  0xa1, 0x80, 0xe1, 0x63, 0xb2, 0x10, 0x3e, 0x34, 0x95, 0xc1, 0x2d, 0xdc, 0xba, 0x7e, 0x31, 0x61,
  0x9c, 0xcd, 0xba, 0xe6, 0x24, 0x65, 0x4d, 0xca, 0x5b, 0x41, 0x75, 0x9c, 0x41, 0xee, 0xb5, 0x2a,
  0x31, 0xf9, 0x6e, 0x06, 0xd9, 0xf7, 0x69, 0xaf, 0x78, 0xf2, 0xe7, 0x4e, 0x7f, 0x08, 0xc6, 0x3d,
  0x01, 0xf6, 0xcd, 0x12, 0xc0, 0x2f, 0xfa, 0xa8, 0x5c, 0x3b, 0x17, 0xf6, 0x1b, 0x9d, 0x37, 0x01,
  0xe5, 0x5e, 0xf6, 0xd4, 0xa8, 0x7b, 0xd5, 0xd9, 0x46, 0x79, 0xe4, 0x42, 0xeb, 0xe4, 0xc1, 0xe0,
  0x90, 0x6a, 0xe2, 0xac, 0x27, 0x9f, 0x5c, 0xdb, 0x74, 0xf6, 0x49, 0x8a, 0x6f, 0x56, 0x60, 0x08,
  0xe3, 0x51, 0x1a, 0xa7, 0xd5, 0x24, 0xc9, 0x83, 0x90, 0x9a, 0x19, 0x04, 0xa9, 0x47, 0x29, 0x14,
  0x7b, 0x2f, 0xd2, 0x13, 0x5e, 0x79, 0x67, 0x33, 0x09, 0x5f, 0xb3, 0x31, 0xcc, 0xf4, 0xcf, 0x21,
  0x09, 0xf1, 0x30, 0x4c, 0xc1, 0xf3, 0x21, 0x65, 0x03, 0x10, 0x1b, 0x18, 0x4f, 0xe0, 0x48, 0x06,
  0x45, 0x2e, 0xb0, 0xbc, 0xd4, 0x87, 0x94, 0x9b, 0x00, 0xef, 0xa3, 0x83, 0x4a, 0x48, 0x05, 0x00,
  0x00
};

inline void serveFingerprintsHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", FINGERPRINTS_HTML, FINGERPRINTS_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// index.html
const uint16_t INDEX_HTML_L = 673;
const uint8_t INDEX_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x9c, 0x93, 0xe5, 0x96, 0xe3, 0x3a,
  0x0c, 0x80, 0x7f, 0x77, 0x9e, 0x42, 0xe3, 0x0b, 0x4d, 0xce, 0x69, 0x9c, 0xcb, 0xd8, 0xcc, 0xc5,
  0x65, 0x66, 0x06, 0x37, 0x56, 0x1a, 0x6f, 0x1d, 0x3b, 0xc7, 0x56, 0xdb, 0xc1, 0x77, 0x5f, 0x3b,
  0xb0, 0xcc, 0x7f, 0x8c, 0xd2, 0x27, 0x9e, 0xef, 0x4a, 0x5b, 0xd2, 0x41, 0x8b, 0x50, 0x53, 0xa3,
  0xf7, 0x76, 0xe6, 0x71, 0x03, 0x2d, 0xcc, 0xb2, 0x60, 0x68, 0xd8, 0xde, 0xce, 0x64, 0x5e, 0xa3,
  0x90, 0x61, 0x9f, 0xcc, 0x1b, 0x24, 0x01, 0x65, 0x2d, 0x9c, 0x47, 0x2a, 0xd8, 0x9a, 0xaa, 0xec,
  0x37, 0x06, 0x79, 0xf7, 0xa5, 0x95, 0x59, 0x81, 0x43, 0x5d, 0x30, 0x55, 0x5a, 0xc3, 0xa0, 0x76,
  0x58, 0x15, 0x8c, 0xe7, 0x95, 0xd8, 0xc4, 0x07, 0xee, 0x37, 0xcb, 0x41, 0xb4, 0xa7, 0x18, 0xd1,
  0x60, 0xc1, 0x36, 0x0a, 0xb7, 0xad, 0x75, 0xc4, 0x20, 0xc8, 0x10, 0x9a, 0x40, 0xdd, 0x2a, 0x49,
  0x75, 0x21, 0x31, 0xa8, 0x61, 0xd6, 0x5d, 0x66, 0xa0, 0x8c, 0x22, 0x25, 0x74, 0xe6, 0x4b, 0xa1,
  0xb1, 0xf8, 0x7e, 0x04, 0xf9, 0xd2, 0xa9, 0x96, 0xe2, 0x71, 0x92, 0xe7, 0x70, 0xc5, 0x40, 0x2b,
  0x96, 0x08, 0xda, 0x0a, 0x09, 0xd6, 0xc1, 0xb6, 0x46, 0x13, 0x9d, 0x35, 0x4b, 0x65, 0x96, 0x40,
  0x35, 0x36, 0xe8, 0x67, 0xb0, 0x40, 0x4f, 0x40, 0x16, 0x84, 0x94, 0x01, 0x1b, 0xbc, 0xc6, 0xb0,
  0xc1, 0xd3, 0x18, 0xe2, 0xd3, 0xee, 0x7d, 0x63, 0x95, 0x84, 0xd3, 0x57, 0x6e, 0xfd, 0x17, 0xb1,
  0xaa, 0x82, 0x44, 0xdb, 0x60, 0xf5, 0x06, 0x59, 0x17, 0xe0, 0xbc, 0xc3, 0x40, 0x51, 0x14, 0x30,
  0x95, 0xc2, 0xad, 0xa6, 0x70, 0x7c, 0x0c, 0xc9, 0x6e, 0x32, 0xed, 0xde, 0xa7, 0x11, 0xf5, 0xaa,
  0x78, 0x0a, 0xdf, 0x7e, 0x0b, 0x5b, 0x65, 0xa4, 0xdd, 0xf2, 0x46, 0x50, 0x59, 0x5f, 0x42, 0xa9,
  0x44, 0x32, 0x4d, 0xda, 0x90, 0x1d, 0x74, 0x3e, 0x2b, 0xad, 0xb6, 0x2e, 0x84, 0x15, 0xb5, 0xff,
  0x80, 0x48, 0x4c, 0xa7, 0x69, 0x2f, 0x8a, 0x3e, 0x4d, 0xe1, 0x28, 0x3a, 0x31, 0x09, 0x05, 0x5a,
  0x37, 0x68, 0x88, 0x8f, 0x87, 0x53, 0x1a, 0xbb, 0x7b, 0xa9, 0x85, 0xf7, 0x17, 0x95, 0x27, 0x1e,
  0xc2, 0x49, 0x7a, 0x8f, 0xd2, 0x3f, 0xa3, 0xce, 0x09, 0xa0, 0xf6, 0xf8, 0xe9, 0xfa, 0x0e, 0x1b,
  0xbb, 0xc1, 0xd7, 0x11, 0x31, 0xc5, 0xf9, 0xcb, 0x1c, 0x8f, 0x55, 0x7e, 0x51, 0x5a, 0xd1, 0xb6,
  0xb9, 0x6a, 0x9a, 0x35, 0x89, 0x85, 0xc6, 0x3c, 0x90, 0x90, 0x7c, 0x1e, 0x82, 0xc5, 0x7d, 0xfe,
  0x6f, 0xb5, 0xba, 0x76, 0xf9, 0xea, 0xcd, 0x1f, 0x78, 0xe9, 0x3d, 0xeb, 0xdb, 0xc2, 0xd3, 0x81,
  0x46, 0x5f, 0x23, 0x12, 0x7b, 0xa3, 0x61, 0x1a, 0x2b, 0xd7, 0x1a, 0x43, 0x4a, 0x62, 0xe5, 0xd8,
  0x7b, 0xf0, 0x68, 0xc8, 0x1d, 0xe4, 0x9e, 0x84, 0x23, 0xfe, 0xff, 0x9d, 0x5b, 0xd5, 0xfa, 0xa2,
  0x5e, 0xf1, 0x67, 0xfe, 0x8b, 0x58, 0x65, 0xbd, 0x36, 0x2b, 0x9f, 0xff, 0x5f, 0x5d, 0xb8, 0xd4,
  0x5e, 0xbe, 0xf4, 0x63, 0xc4, 0x7c, 0xb9, 0x4b, 0xe1, 0x8d, 0x57, 0xdf, 0xef, 0xff, 0xf3, 0x1b,
  0xdd, 0xfb, 0x77, 0x20, 0xcd, 0xf3, 0x61, 0x62, 0xe6, 0x0b, 0x2b, 0x0f, 0x40, 0x0a, 0x12, 0x99,
  0xdf, 0xa0, 0x26, 0x5c, 0x29, 0xca, 0x06, 0x6a, 0x16, 0x9f, 0x0b, 0x56, 0x87, 0xb4, 0xbb, 0xde,
  0xbc, 0x54, 0x1b, 0xe8, 0xb2, 0x54, 0x30, 0xa9, 0x7c, 0xab, 0xc5, 0xc1, 0x1f, 0xe3, 0x58, 0x74,
  0xd8, 0xd7, 0x3b, 0x7e, 0xd2, 0xd7, 0x76, 0xf2, 0xe4, 0xc9, 0x0b, 0xf4, 0x93, 0x9f, 0x7e, 0xae,
  0xa1, 0x80, 0xe1, 0x63, 0xb2, 0x10, 0x3e, 0x34, 0x95, 0xc1, 0x2d, 0xdc, 0xba, 0x7e, 0x31, 0x61,
  0x9c, 0xcd, 0xba, 0xe6, 0x24, 0x65, 0x4d, 0xca, 0x5b, 0x41, 0x75, 0x9c, 0x41, 0xee, 0xb5, 0x2a,
  0x31, 0xf9, 0x6e, 0x06, 0xd9, 0xf7, 0x69, 0xaf, 0x78, 0xf2, 0xe7, 0x4e, 0x7f, 0x08, 0xc6, 0x3d,
  0x01, 0xf6, 0xcd, 0x12, 0xc0, 0x2f, 0xfa, 0xa8, 0x5c, 0x3b, 0x17, 0xf6, 0x1b, 0x9d, 0x37, 0x01,
  0xe5, 0x5e, 0xf6, 0xd4, 0xa8, 0x7b, 0xd5, 0xd9, 0x46, 0x79, 0xe4, 0x42, 0xeb, 0xe4, 0xc1, 0xe0,
  0x90, 0x6a, 0xe2, 0xac, 0x27, 0x9f, 0x5c, 0xdb, 0x74, 0xf6, 0x49, 0x8a, 0x6f, 0x56, 0x60, 0x08,
  0xe3, 0x51, 0x1a, 0xa7, 0xd5, 0x24, 0xc9, 0x83, 0x90, 0x9a, 0x19, 0x04, 0xa9, 0x47, 0x29, 0x14,
  0x7b, 0x2f, 0xd2, 0x13, 0x5e, 0x79, 0x67, 0x33, 0x09, 0x5f, 0xb3, 0x31, 0xcc, 0xf4, 0xcf, 0x21,
  0x09, 0xf1, 0x30, 0x4c, 0xc1, 0xf3, 0x21, 0x65, 0x03, 0x10, 0x1b, 0x18, 0x4f, 0xe0, 0x48, 0x06,
  0x45, 0x2e, 0xb0, 0xbc, 0xd4, 0x87, 0x94, 0x9b, 0x00, 0xef, 0xa3, 0x83, 0x4a, 0x48, 0x05, 0x00,
  0x00
};

inline void serveIndexHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", INDEX_HTML, INDEX_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// network.html
const uint16_t NETWORK_HTML_L = 673;
const uint8_t NETWORK_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x9c, 0x93, 0xe5, 0x96, 0xe3, 0x3a,
  0x0c, 0x80, 0x7f, 0x77, 0x9e, 0x42, 0xe3, 0x0b, 0x4d, 0xce, 0x69, 0x9c, 0xcb, 0xd8, 0xcc, 0xc5,
  0x65, 0x66, 0x06, 0x37, 0x56, 0x1a, 0x6f, 0x1d, 0x3b, 0xc7, 0x56, 0xdb, 0xc1, 0x77, 0x5f, 0x3b,
  0xb0, 0xcc, 0x7f, 0x8c, 0xd2, 0x27, 0x9e, 0xef, 0x4a, 0x5b, 0xd2, 0x41, 0x8b, 0x50, 0x53, 0xa3,
  0xf7, 0x76, 0xe6, 0x71, 0x03, 0x2d, 0xcc, 0xb2, 0x60, 0x68, 0xd8, 0xde, 0xce, 0x64, 0x5e, 0xa3,
  0x90, 0x61, 0x9f, 0xcc, 0x1b, 0x24, 0x01, 0x65, 0x2d, 0x9c, 0x47, 0x2a, 0xd8, 0x9a, 0xaa, 0xec,
  0x37, 0x06, 0x79, 0xf7, 0xa5, 0x95, 0x59, 0x81, 0x43, 0x5d, 0x30, 0x55, 0x5a, 0xc3, 0xa0, 0x76,
  0x58, 0x15, 0x8c, 0xe7, 0x95, 0xd8, 0xc4, 0x07, 0xee, 0x37, 0xcb, 0x41, 0xb4, 0xa7, 0x18, 0xd1,
  0x60, 0xc1, 0x36, 0x0a, 0xb7, 0xad, 0x75, 0xc4, 0x20, 0xc8, 0x10, 0x9a, 0x40, 0xdd, 0x2a, 0x49,
  0x75, 0x21, 0x31, 0xa8, 0x61, 0xd6, 0x5d, 0x66, 0xa0, 0x8c, 0x22, 0x25, 0x74, 0xe6, 0x4b, 0xa1,
  0xb1, 0xf8, 0x7e, 0x04, 0xf9, 0xd2, 0xa9, 0x96, 0xe2, 0x71, 0x92, 0xe7, 0x70, 0xc5, 0x40, 0x2b,
  0x96, 0x08, 0xda, 0x0a, 0x09, 0xd6, 0xc1, 0xb6, 0x46, 0x13, 0x9d, 0x35, 0x4b, 0x65, 0x96, 0x40,
  0x35, 0x36, 0xe8, 0x67, 0xb0, 0x40, 0x4f, 0x40, 0x16, 0x84, 0x94, 0x01, 0x1b, 0xbc, 0xc6, 0xb0,
  0xc1, 0xd3, 0x18, 0xe2, 0xd3, 0xee, 0x7d, 0x63, 0x95, 0x84, 0xd3, 0x57, 0x6e, 0xfd, 0x17, 0xb1,
  0xaa, 0x82, 0x44, 0xdb, 0x60, 0xf5, 0x06, 0x59, 0x17, 0xe0, 0xbc, 0xc3, 0x40, 0x51, 0x14, 0x30,
  0x95, 0xc2, 0xad, 0xa6, 0x70, 0x7c, 0x0c, 0xc9, 0x6e, 0x32, 0xed, 0xde, 0xa7, 0x11, 0xf5, 0xaa,
  0x78, 0x0a, 0xdf, 0x7e, 0x0b, 0x5b, 0x65, 0xa4, 0xdd, 0xf2, 0x46, 0x50, 0x59, 0x5f, 0x42, 0xa9,
  0x44, 0x32, 0x4d, 0xda, 0x90, 0x1d, 0x74, 0x3e, 0x2b, 0xad, 0xb6, 0x2e, 0x84, 0x15, 0xb5, 0xff,
  0x80, 0x48, 0x4c, 0xa7, 0x69, 0x2f, 0x8a, 0x3e, 0x4d, 0xe1, 0x28, 0x3a, 0x31, 0x09, 0x05, 0x5a,
  0x37, 0x68, 0x88, 0x8f, 0x87, 0x53, 0x1a, 0xbb, 0x7b, 0xa9, 0x85, 0xf7, 0x17, 0x95, 0x27, 0x1e,
  0xc2, 0x49, 0x7a, 0x8f, 0xd2, 0x3f, 0xa3, 0xce, 0x09, 0xa0, 0xf6, 0xf8, 0xe9, 0xfa, 0x0e, 0x1b,
  0xbb, 0xc1, 0xd7, 0x11, 0x31, 0xc5, 0xf9, 0xcb, 0x1c, 0x8f, 0x55, 0x7e, 0x51, 0x5a, 0xd1, 0xb6,
  0xb9, 0x6a, 0x9a, 0x35, 0x89, 0x85, 0xc6, 0x3c, 0x90, 0x90, 0x7c, 0x1e, 0x82, 0xc5, 0x7d, 0xfe,
  0x6f, 0xb5, 0xba, 0x76, 0xf9, 0xea, 0xcd, 0x1f, 0x78, 0xe9, 0x3d, 0xeb, 0xdb, 0xc2, 0xd3, 0x81,
  0x46, 0x5f, 0x23, 0x12, 0x7b, 0xa3, 0x61, 0x1a, 0x2b, 0xd7, 0x1a, 0x43, 0x4a, 0x62, 0xe5, 0xd8,
  0x7b, 0xf0, 0x68, 0xc8, 0x1d, 0xe4, 0x9e, 0x84, 0x23, 0xfe, 0xff, 0x9d, 0x5b, 0xd5, 0xfa, 0xa2,
  0x5e, 0xf1, 0x67, 0xfe, 0x8b, 0x58, 0x65, 0xbd, 0x36, 0x2b, 0x9f, 0xff, 0x5f, 0x5d, 0xb8, 0xd4,
  0x5e, 0xbe, 0xf4, 0x63, 0xc4, 0x7c, 0xb9, 0x4b, 0xe1, 0x8d, 0x57, 0xdf, 0xef, 0xff, 0xf3, 0x1b,
  0xdd, 0xfb, 0x77, 0x20, 0xcd, 0xf3, 0x61, 0x62, 0xe6, 0x0b, 0x2b, 0x0f, 0x40, 0x0a, 0x12, 0x99,
  0xdf, 0xa0, 0x26, 0x5c, 0x29, 0xca, 0x06, 0x6a, 0x16, 0x9f, 0x0b, 0x56, 0x87, 0xb4, 0xbb, 0xde,
  0xbc, 0x54, 0x1b, 0xe8, 0xb2, 0x54, 0x30, 0xa9, 0x7c, 0xab, 0xc5, 0xc1, 0x1f, 0xe3, 0x58, 0x74,
  0xd8, 0xd7, 0x3b, 0x7e, 0xd2, 0xd7, 0x76, 0xf2, 0xe4, 0xc9, 0x0b, 0xf4, 0x93, 0x9f, 0x7e, 0xae,
  0xa1, 0x80, 0xe1, 0x63, 0xb2, 0x10, 0x3e, 0x34, 0x95, 0xc1, 0x2d, 0xdc, 0xba, 0x7e, 0x31, 0x61,
  0x9c, 0xcd, 0xba, 0xe6, 0x24, 0x65, 0x4d, 0xca, 0x5b, 0x41, 0x75, 0x9c, 0x41, 0xee, 0xb5, 0x2a,
  0x31, 0xf9, 0x6e, 0x06, 0xd9, 0xf7, 0x69, 0xaf, 0x78, 0xf2, 0xe7, 0x4e, 0x7f, 0x08, 0xc6, 0x3d,
  0x01, 0xf6, 0xcd, 0x12, 0xc0, 0x2f, 0xfa, 0xa8, 0x5c, 0x3b, 0x17, 0xf6, 0x1b, 0x9d, 0x37, 0x01,
  0xe5, 0x5e, 0xf6, 0xd4, 0xa8, 0x7b, 0xd5, 0xd9, 0x46, 0x79, 0xe4, 0x42, 0xeb, 0xe4, 0xc1, 0xe0,
  0x90, 0x6a, 0xe2, 0xac, 0x27, 0x9f, 0x5c, 0xdb, 0x74, 0xf6, 0x49, 0x8a, 0x6f, 0x56, 0x60, 0x08,
  0xe3, 0x51, 0x1a, 0xa7, 0xd5, 0x24, 0xc9, 0x83, 0x90, 0x9a, 0x19, 0x04, 0xa9, 0x47, 0x29, 0x14,
  0x7b, 0x2f, 0xd2, 0x13, 0x5e, 0x79, 0x67, 0x33, 0x09, 0x5f, 0xb3, 0x31, 0xcc, 0xf4, 0xcf, 0x21,
  0x09, 0xf1, 0x30, 0x4c, 0xc1, 0xf3, 0x21, 0x65, 0x03, 0x10, 0x1b, 0x18, 0x4f, 0xe0, 0x48, 0x06,
  0x45, 0x2e, 0xb0, 0xbc, 0xd4, 0x87, 0x94, 0x9b, 0x00, 0xef, 0xa3, 0x83, 0x4a, 0x48, 0x05, 0x00,
  0x00
};

inline void serveNetworkHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", NETWORK_HTML, NETWORK_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

// settings.html
const uint16_t SETTINGS_HTML_L = 673;
const uint8_t SETTINGS_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x9c, 0x93, 0xe5, 0x96, 0xe3, 0x3a,
  0x0c, 0x80, 0x7f, 0x77, 0x9e, 0x42, 0xe3, 0x0b, 0x4d, 0xce, 0x69, 0x9c, 0xcb, 0xd8, 0xcc, 0xc5,
  0x65, 0x66, 0x06, 0x37, 0x56, 0x1a, 0x6f, 0x1d, 0x3b, 0xc7, 0x56, 0xdb, 0xc1, 0x77, 0x5f, 0x3b,
  0xb0, 0xcc, 0x7f, 0x8c, 0xd2, 0x27, 0x9e, 0xef, 0x4a, 0x5b, 0xd2, 0x41, 0x8b, 0x50, 0x53, 0xa3,
  0xf7, 0x76, 0xe6, 0x71, 0x03, 0x2d, 0xcc, 0xb2, 0x60, 0x68, 0xd8, 0xde, 0xce, 0x64, 0x5e, 0xa3,
  0x90, 0x61, 0x9f, 0xcc, 0x1b, 0x24, 0x01, 0x65, 0x2d, 0x9c, 0x47, 0x2a, 0xd8, 0x9a, 0xaa, 0xec,
  0x37, 0x06, 0x79, 0xf7, 0xa5, 0x95, 0x59, 0x81, 0x43, 0x5d, 0x30, 0x55, 0x5a, 0xc3, 0xa0, 0x76,
  0x58, 0x15, 0x8c, 0xe7, 0x95, 0xd8, 0xc4, 0x07, 0xee, 0x37, 0xcb, 0x41, 0xb4, 0xa7, 0x18, 0xd1,
  0x60, 0xc1, 0x36, 0x0a, 0xb7, 0xad, 0x75, 0xc4, 0x20, 0xc8, 0x10, 0x9a, 0x40, 0xdd, 0x2a, 0x49,
  0x75, 0x21, 0x31, 0xa8, 0x61, 0xd6, 0x5d, 0x66, 0xa0, 0x8c, 0x22, 0x25, 0x74, 0xe6, 0x4b, 0xa1,
  0xb1, 0xf8, 0x7e, 0x04, 0xf9, 0xd2, 0xa9, 0x96, 0xe2, 0x71, 0x92, 0xe7, 0x70, 0xc5, 0x40, 0x2b,
  0x96, 0x08, 0xda, 0x0a, 0x09, 0xd6, 0xc1, 0xb6, 0x46, 0x13, 0x9d, 0x35, 0x4b, 0x65, 0x96, 0x40,
  0x35, 0x36, 0xe8, 0x67, 0xb0, 0x40, 0x4f, 0x40, 0x16, 0x84, 0x94, 0x01, 0x1b, 0xbc, 0xc6, 0xb0,
  0xc1, 0xd3, 0x18, 0xe2, 0xd3, 0xee, 0x7d, 0x63, 0x95, 0x84, 0xd3, 0x57, 0x6e, 0xfd, 0x17, 0xb1,
  0xaa, 0x82, 0x44, 0xdb, 0x60, 0xf5, 0x06, 0x59, 0x17, 0xe0, 0xbc, 0xc3, 0x40, 0x51, 0x14, 0x30,
  0x95, 0xc2, 0xad, 0xa6, 0x70, 0x7c, 0x0c, 0xc9, 0x6e, 0x32, 0xed, 0xde, 0xa7, 0x11, 0xf5, 0xaa,
  0x78, 0x0a, 0xdf, 0x7e, 0x0b, 0x5b, 0x65, 0xa4, 0xdd, 0xf2, 0x46, 0x50, 0x59, 0x5f, 0x42, 0xa9,
  0x44, 0x32, 0x4d, 0xda, 0x90, 0x1d, 0x74, 0x3e, 0x2b, 0xad, 0xb6, 0x2e, 0x84, 0x15, 0xb5, 0xff,
  0x80, 0x48, 0x4c, 0xa7, 0x69, 0x2f, 0x8a, 0x3e, 0x4d, 0xe1, 0x28, 0x3a, 0x31, 0x09, 0x05, 0x5a,
  0x37, 0x68, 0x88, 0x8f, 0x87, 0x53, 0x1a, 0xbb, 0x7b, 0xa9, 0x85, 0xf7, 0x17, 0x95, 0x27, 0x1e,
  0xc2, 0x49, 0x7a, 0x8f, 0xd2, 0x3f, 0xa3, 0xce, 0x09, 0xa0, 0xf6, 0xf8, 0xe9, 0xfa, 0x0e, 0x1b,
  0xbb, 0xc1, 0xd7, 0x11, 0x31, 0xc5, 0xf9, 0xcb, 0x1c, 0x8f, 0x55, 0x7e, 0x51, 0x5a, 0xd1, 0xb6,
  0xb9, 0x6a, 0x9a, 0x35, 0x89, 0x85, 0xc6, 0x3c, 0x90, 0x90, 0x7c, 0x1e, 0x82, 0xc5, 0x7d, 0xfe,
  0x6f, 0xb5, 0xba, 0x76, 0xf9, 0xea, 0xcd, 0x1f, 0x78, 0xe9, 0x3d, 0xeb, 0xdb, 0xc2, 0xd3, 0x81,
  0x46, 0x5f, 0x23, 0x12, 0x7b, 0xa3, 0x61, 0x1a, 0x2b, 0xd7, 0x1a, 0x43, 0x4a, 0x62, 0xe5, 0xd8,
  0x7b, 0xf0, 0x68, 0xc8, 0x1d, 0xe4, 0x9e, 0x84, 0x23, 0xfe, 0xff, 0x9d, 0x5b, 0xd5, 0xfa, 0xa2,
  0x5e, 0xf1, 0x67, 0xfe, 0x8b, 0x58, 0x65, 0xbd, 0x36, 0x2b, 0x9f, 0xff, 0x5f, 0x5d, 0xb8, 0xd4,
  0x5e, 0xbe, 0xf4, 0x63, 0xc4, 0x7c, 0xb9, 0x4b, 0xe1, 0x8d, 0x57, 0xdf, 0xef, 0xff, 0xf3, 0x1b,
  0xdd, 0xfb, 0x77, 0x20, 0xcd, 0xf3, 0x61, 0x62, 0xe6, 0x0b, 0x2b, 0x0f, 0x40, 0x0a, 0x12, 0x99,
  0xdf, 0xa0, 0x26, 0x5c, 0x29, 0xca, 0x06, 0x6a, 0x16, 0x9f, 0x0b, 0x56, 0x87, 0xb4, 0xbb, 0xde,
  0xbc, 0x54, 0x1b, 0xe8, 0xb2, 0x54, 0x30, 0xa9, 0x7c, 0xab, 0xc5, 0xc1, 0x1f, 0xe3, 0x58, 0x74,
  0xd8, 0xd7, 0x3b, 0x7e, 0xd2, 0xd7, 0x76, 0xf2, 0xe4, 0xc9, 0x0b, 0xf4, 0x93, 0x9f, 0x7e, 0xae,
  0xa1, 0x80, 0xe1, 0x63, 0xb2, 0x10, 0x3e, 0x34, 0x95, 0xc1, 0x2d, 0xdc, 0xba, 0x7e, 0x31, 0x61,
  0x9c, 0xcd, 0xba, 0xe6, 0x24, 0x65, 0x4d, 0xca, 0x5b, 0x41, 0x75, 0x9c, 0x41, 0xee, 0xb5, 0x2a,
  0x31, 0xf9, 0x6e, 0x06, 0xd9, 0xf7, 0x69, 0xaf, 0x78, 0xf2, 0xe7, 0x4e, 0x7f, 0x08, 0xc6, 0x3d,
  0x01, 0xf6, 0xcd, 0x12, 0xc0, 0x2f, 0xfa, 0xa8, 0x5c, 0x3b, 0x17, 0xf6, 0x1b, 0x9d, 0x37, 0x01,
  0xe5, 0x5e, 0xf6, 0xd4, 0xa8, 0x7b, 0xd5, 0xd9, 0x46, 0x79, 0xe4, 0x42, 0xeb, 0xe4, 0xc1, 0xe0,
  0x90, 0x6a, 0xe2, 0xac, 0x27, 0x9f, 0x5c, 0xdb, 0x74, 0xf6, 0x49, 0x8a, 0x6f, 0x56, 0x60, 0x08,
  0xe3, 0x51, 0x1a, 0xa7, 0xd5, 0x24, 0xc9, 0x83, 0x90, 0x9a, 0x19, 0x04, 0xa9, 0x47, 0x29, 0x14,
  0x7b, 0x2f, 0xd2, 0x13, 0x5e, 0x79, 0x67, 0x33, 0x09, 0x5f, 0xb3, 0x31, 0xcc, 0xf4, 0xcf, 0x21,
  0x09, 0xf1, 0x30, 0x4c, 0xc1, 0xf3, 0x21, 0x65, 0x03, 0x10, 0x1b, 0x18, 0x4f, 0xe0, 0x48, 0x06,
  0x45, 0x2e, 0xb0, 0xbc, 0xd4, 0x87, 0x94, 0x9b, 0x00, 0xef, 0xa3, 0x83, 0x4a, 0x48, 0x05, 0x00,
  0x00
};

inline void serveSettingsHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", SETTINGS_HTML, SETTINGS_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

