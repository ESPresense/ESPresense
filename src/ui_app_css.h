/*
 * Binary arrays for the Web UI app directory (CSS files).
 * Zopfli compression is used for smaller size and improved speeds.
 */

#pragma once
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

// app_immutable_assets_index_DUGlLD2F_css
const uint16_t APP_IMMUTABLE_ASSETS_INDEX_DUGLLD2F_CSS_L = 3838;
const uint8_t APP_IMMUTABLE_ASSETS_INDEX_DUGLLD2F_CSS[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0xec, 0x58, 0x07, 0x97, 0xeb, 0x2a,
  0x0e, 0xfe, 0x2b, 0x79, 0x7d, 0xe6, 0x1d, 0xc8, 0x26, 0x99, 0xfa, 0x9c, 0xed, 0xbd, 0xf7, 0x3d,
  0xed, 0x55, 0x6c, 0xe4, 0x98, 0x3b, 0x18, 0xbc, 0x80, 0x53, 0xae, 0x8f, 0xf7, 0xb7, 0xaf, 0x11,
  0xc6, 0xb1, 0xe3, 0xcc, 0xf6, 0x3d, 0xf5, 0xb6, 0x19, 0xf4, 0x7d, 0x42, 0x08, 0x49, 0x08, 0x92,
  0x4f, 0x49, 0x92, 0x42, 0xae, 0x0d, 0x90, 0x84, 0xe5, 0x0e, 0x4c, 0x43, 0xa9, 0x3b, 0xd0, 0x54,
  0x1b, 0x0e, 0x86, 0xda, 0x8a, 0x65, 0x42, 0xed, 0xe8, 0x31, 0x59, 0xac, 0xb6, 0xd7, 0x88, 0xd3,
  0x40, 0x38, 0xc3, 0x94, 0x95, 0xcc, 0x01, 0x3d, 0xce, 0xb1, 0x91, 0x9e, 0xd1, 0xae, 0x03, 0x06,
  0xd1, 0xbe, 0xc0, 0x21, 0xce, 0x88, 0xe2, 0x59, 0xd9, 0x66, 0x4c, 0xa2, 0xc1, 0xf5, 0x58, 0x3e,
  0x0d, 0x72, 0xc5, 0x94, 0x67, 0xcf, 0xc2, 0x69, 0x10, 0x84, 0xca, 0x0a, 0xfa, 0x56, 0xeb, 0x32,
  0x22, 0x36, 0x33, 0x5a, 0x4a, 0x6a, 0x15, 0xab, 0xa8, 0x75, 0x46, 0x64, 0x4e, 0x81, 0xb5, 0xc9,
  0xa2, 0x32, 0xfa, 0x28, 0x4a, 0xe1, 0x4e, 0x41, 0x6d, 0x67, 0x18, 0x17, 0xa0, 0x1c, 0xcd, 0x8d,
  0x2e, 0x69, 0xa5, 0xad, 0x70, 0x42, 0xab, 0x68, 0x64, 0x60, 0xf7, 0x82, 0xbd, 0x4e, 0x3a, 0x3d,
  0xe3, 0xb4, 0xe1, 0x42, 0x31, 0x39, 0x38, 0x23, 0x99, 0x2d, 0x80, 0xd3, 0xb7, 0x60, 0x74, 0xc4,
  0x54, 0x5d, 0x82, 0x11, 0x19, 0xcd, 0xc5, 0xae, 0x36, 0x70, 0x81, 0xc6, 0x88, 0xcf, 0x94, 0x0d,
  0xcb, 0xc6, 0xeb, 0x18, 0x9f, 0x15, 0xa1, 0x2c, 0xb8, 0x09, 0xa2, 0xf3, 0xbc, 0x83, 0xe8, 0x41,
  0x70, 0x57, 0x74, 0xd1, 0xad, 0x8e, 0x73, 0x2e, 0xd3, 0x52, 0x9b, 0x64, 0xf1, 0x41, 0x9e, 0xe7,
  0x67, 0x32, 0xa2, 0x66, 0x97, 0xde, 0x3c, 0x7c, 0xb6, 0x58, 0xdf, 0xad, 0x16, 0x9b, 0xfb, 0xc7,
  0xc5, 0xb7, 0x16, 0xcb, 0x87, 0xdb, 0xb9, 0x09, 0x5b, 0x30, 0xae, 0x0f, 0x9d, 0xfd, 0xee, 0xef,
  0x07, 0xab, 0xee, 0xcf, 0x48, 0xe5, 0x3a, 0xf7, 0x8f, 0xe1, 0xb0, 0x3a, 0xf0, 0x19, 0x9d, 0xca,
  0xda, 0xc4, 0xfd, 0xa5, 0x46, 0xec, 0x0a, 0xcc, 0x65, 0x44, 0x32, 0xad, 0x9c, 0x61, 0xd6, 0x8d,
  0xf2, 0x72, 0xc2, 0xca, 0x89, 0x40, 0x51, 0x43, 0x5f, 0x87, 0x11, 0x11, 0x6a, 0x0f, 0x66, 0x98,
  0x60, 0x99, 0xab, 0xcd, 0x88, 0xb5, 0x50, 0x09, 0x16, 0x05, 0x6e, 0x74, 0x15, 0xdd, 0x8e, 0x2e,
  0xb0, 0xec, 0x05, 0x61, 0xf4, 0x6b, 0x06, 0xce, 0x1c, 0x8c, 0xd4, 0xcc, 0xd3, 0x48, 0xcc, 0x5d,
  0x1e, 0x98, 0xb9, 0xef, 0x91, 0x8a, 0x9b, 0xb8, 0x84, 0xb5, 0xaf, 0x1c, 0x77, 0x9a, 0xe1, 0x71,
  0x97, 0x73, 0x22, 0x6e, 0x37, 0x06, 0x93, 0x09, 0x45, 0xad, 0x78, 0x0b, 0x97, 0x98, 0x64, 0x27,
  0x5d, 0xbb, 0x4b, 0xb4, 0x62, 0x42, 0xcd, 0x40, 0xeb, 0x4e, 0x7e, 0x33, 0x6d, 0x92, 0xc4, 0x65,
  0xfe, 0x4f, 0x8d, 0xe6, 0x5d, 0xa3, 0x79, 0xd7, 0x68, 0xde, 0x35, 0x9a, 0x77, 0x8d, 0x66, 0xf6,
  0xae, 0x49, 0xf5, 0xd1, 0xdb, 0xf6, 0xa5, 0xdd, 0xf7, 0x96, 0x0e, 0xd9, 0x86, 0x61, 0x5f, 0xaf,
  0xab, 0x28, 0x06, 0x23, 0x56, 0x4b, 0xc1, 0x23, 0x14, 0xea, 0xf3, 0x03, 0x78, 0x80, 0x27, 0x48,
  0xdb, 0x0b, 0xdb, 0x83, 0x07, 0xe0, 0x1d, 0x7a, 0xff, 0xfd, 0xb6, 0x70, 0xa5, 0x24, 0x49, 0xa1,
  0xad, 0x6b, 0xa4, 0x50, 0x40, 0x0b, 0xf0, 0xa9, 0x49, 0xd6, 0xcb, 0x87, 0x2d, 0x3d, 0x40, 0xfa,
  0x22, 0x1c, 0x75, 0x70, 0x74, 0xb8, 0x59, 0xca, 0xf8, 0x9b, 0xda, 0x76, 0xe4, 0x6a, 0xf5, 0xd1,
  0x96, 0x96, 0xfa, 0x2d, 0x75, 0x2c, 0x45, 0x26, 0xb9, 0xdf, 0x52, 0x3d, 0x96, 0x46, 0xc3, 0x5c,
  0xfb, 0xd6, 0xc1, 0x4a, 0x21, 0x4f, 0x49, 0x2d, 0xa8, 0x65, 0xca, 0x52, 0x0b, 0x46, 0xe4, 0xc4,
  0x9e, 0xac, 0x83, 0x92, 0xd6, 0x82, 0x8c, 0xc0, 0xf7, 0x7f, 0x50, 0x55, 0x12, 0x16, 0x3f, 0xf2,
  0xbb, 0x58, 0xfc, 0xa4, 0xd4, 0x6f, 0xc4, 0xfb, 0xe4, 0xfd, 0x3f, 0xc1, 0x4e, 0xc3, 0xe2, 0x2f,
  0xbf, 0x88, 0xc0, 0x20, 0xff, 0xe9, 0x54, 0xa6, 0x5a, 0x92, 0xf7, 0x7f, 0xab, 0x9d, 0x9e, 0xcc,
  0xe9, 0x97, 0x05, 0x9f, 0x51, 0xa0, 0x16, 0x9c, 0x13, 0x6a, 0x67, 0x13, 0xa5, 0x4d, 0xc9, 0x64,
  0x20, 0xf7, 0xcc, 0x08, 0xe6, 0x3b, 0xc5, 0x8c, 0x1e, 0x76, 0xce, 0x2a, 0x5a, 0x74, 0xf1, 0x90,
  0xdd, 0xff, 0xd8, 0x0e, 0xb0, 0x83, 0x57, 0xcc, 0x80, 0x72, 0x6d, 0xaa, 0xf9, 0xa9, 0x29, 0x99,
  0xd9, 0x09, 0xd5, 0xa5, 0x64, 0x1c, 0x3f, 0xa1, 0x0a, 0x30, 0xc2, 0xb5, 0x85, 0x69, 0x02, 0xd2,
  0xf1, 0x38, 0x3f, 0x32, 0x31, 0x5b, 0x4e, 0x57, 0x7d, 0x4e, 0xd7, 0xd5, 0xb1, 0x65, 0x69, 0x6a,
  0x92, 0x43, 0xa7, 0x00, 0x37, 0x9f, 0x3b, 0xe1, 0x24, 0x7c, 0x79, 0xdb, 0x4c, 0xd2, 0xc0, 0x21,
  0xd3, 0x06, 0x9d, 0x4e, 0x6a, 0xc5, 0xc1, 0xf8, 0x35, 0x17, 0x5c, 0x3b, 0x07, 0x7c, 0xfb, 0xcf,
  0x14, 0xda, 0x62, 0x4d, 0x8a, 0x0d, 0x29, 0xee, 0x48, 0x71, 0x4f, 0x8a, 0x07, 0x52, 0x3c, 0x36,
  0x18, 0x07, 0xcc, 0x53, 0x74, 0x0b, 0x91, 0xc3, 0x74, 0x13, 0xac, 0x99, 0xb8, 0x3e, 0x5b, 0x28,
  0xea, 0xa5, 0xc4, 0x3a, 0xa3, 0xd5, 0xae, 0x19, 0x1b, 0x49, 0xb5, 0xe4, 0x60, 0xda, 0x4c, 0x73,
  0x20, 0x2f, 0x29, 0x27, 0x96, 0x95, 0x15, 0xa9, 0x0c, 0x34, 0x17, 0x75, 0x51, 0x6a, 0xa5, 0x7d,
  0x47, 0x07, 0xf2, 0xa7, 0x9f, 0xfe, 0xa6, 0x1b, 0xd3, 0x3f, 0xc2, 0xae, 0x96, 0xcc, 0x90, 0xdf,
  0x80, 0x92, 0x9a, 0x74, 0x10, 0xcb, 0x34, 0xf9, 0x91, 0x56, 0x56, 0x4b, 0x66, 0xc9, 0xaf, 0x45,
  0x0a, 0x61, 0xf9, 0x45, 0x47, 0x79, 0xa2, 0x36, 0x02, 0xcc, 0xe2, 0xb7, 0x70, 0x20, 0x83, 0xa9,
  0xff, 0xaa, 0x08, 0xce, 0xb1, 0x59, 0x43, 0xd9, 0xda, 0x0e, 0x92, 0xa3, 0x78, 0x3d, 0xaf, 0x3e,
  0x6a, 0x6d, 0x9d, 0x12, 0x5b, 0x57, 0x23, 0xf4, 0xe9, 0xe1, 0xa3, 0x49, 0x19, 0xac, 0xb6, 0xc3,
  0xad, 0x67, 0x40, 0x32, 0x27, 0xf6, 0xb0, 0xf5, 0x1d, 0x49, 0x64, 0x4c, 0x52, 0x26, 0xc5, 0x4e,
  0x25, 0x29, 0xb3, 0xe0, 0xa7, 0x78, 0x6b, 0x4d, 0xaa, 0x9d, 0xd3, 0x65, 0x42, 0x97, 0x9b, 0x07,
  0x28, 0x5b, 0x6f, 0xdb, 0xe9, 0xaa, 0x13, 0xbd, 0xe4, 0x58, 0x2a, 0xa1, 0xc1, 0xd8, 0x0b, 0xc5,
  0x41, 0xb9, 0x73, 0x17, 0x98, 0x57, 0x56, 0x00, 0x25, 0xab, 0x2c, 0x24, 0x71, 0xd0, 0xa6, 0xb5,
  0x73, 0x5a, 0x11, 0xa1, 0xaa, 0xda, 0x11, 0x5d, 0xb9, 0x9d, 0xd1, 0x75, 0x45, 0x2c, 0x48, 0xc8,
  0x1c, 0xf1, 0x86, 0x99, 0x01, 0x36, 0xce, 0x4b, 0x34, 0x79, 0x3d, 0x8e, 0x91, 0x9d, 0x05, 0x72,
  0xce, 0xc7, 0xf8, 0x60, 0xe7, 0xb8, 0x52, 0x62, 0x31, 0x68, 0x53, 0x0c, 0x9c, 0x3b, 0x3f, 0xac,
  0x06, 0x78, 0xba, 0xd9, 0xe1, 0xf4, 0x55, 0x8c, 0x73, 0xaf, 0xb6, 0x8a, 0xdb, 0x0c, 0xfb, 0x0a,
  0x01, 0xc3, 0x43, 0x9b, 0x6b, 0x53, 0x26, 0x4a, 0xab, 0x69, 0x20, 0x86, 0xe3, 0x76, 0xaa, 0xe0,
  0x3b, 0x81, 0xf8, 0xf2, 0xf6, 0x0a, 0x65, 0xc0, 0x82, 0xbb, 0xca, 0xd8, 0x3a, 0x2d, 0x85, 0x1b,
  0x9d, 0x54, 0x56, 0x55, 0xc0, 0x0c, 0x53, 0x19, 0x24, 0xc1, 0xe0, 0xd6, 0x5f, 0x24, 0x3e, 0xda,
  0x8a, 0xcf, 0xbb, 0xc8, 0x98, 0x14, 0x25, 0xdb, 0x41, 0xf0, 0x31, 0xc1, 0x06, 0x9b, 0xeb, 0xac,
  0xb6, 0xfe, 0xb5, 0xd0, 0xe8, 0xda, 0xf9, 0x20, 0x25, 0xac, 0x76, 0xba, 0x27, 0x6b, 0xe1, 0x2f,
  0x38, 0x26, 0x05, 0xf7, 0x77, 0x46, 0xbc, 0x81, 0x71, 0x76, 0x65, 0xf4, 0xce, 0x80, 0xb5, 0xcd,
  0x6b, 0xd5, 0x96, 0x24, 0xd1, 0x5b, 0xa1, 0x14, 0x46, 0x59, 0x28, 0xda, 0xc7, 0xe5, 0xcc, 0xe9,
  0xda, 0x4d, 0xb9, 0xd8, 0xcd, 0xd0, 0x8b, 0x7e, 0xfb, 0xc0, 0x4c, 0x56, 0x7c, 0x79, 0x6d, 0xf7,
  0x3e, 0xf6, 0xb9, 0x00, 0xc9, 0xb7, 0xbd, 0xf7, 0xfd, 0xb3, 0x28, 0xa1, 0x9b, 0xea, 0x38, 0x72,
  0x21, 0x98, 0x18, 0xb5, 0x94, 0x6b, 0xc6, 0x94, 0x9e, 0xb8, 0x9d, 0x0b, 0x09, 0xb4, 0xae, 0xa4,
  0x66, 0x3c, 0xfa, 0xf6, 0x6a, 0xfc, 0xb1, 0xe4, 0x86, 0x36, 0x65, 0xeb, 0xb2, 0x64, 0xe6, 0xd4,
  0x70, 0x61, 0x2b, 0xc9, 0x4e, 0x89, 0x14, 0xd6, 0x51, 0xe1, 0xa0, 0x6c, 0x53, 0xa9, 0xb3, 0x97,
  0xbf, 0xd6, 0xda, 0x01, 0xe1, 0x92, 0x70, 0x4e, 0x66, 0x1d, 0x93, 0x14, 0x86, 0x84, 0xd7, 0x28,
  0x09, 0x5d, 0x2c, 0xd6, 0x5f, 0x8b, 0xfb, 0xb4, 0xe0, 0x9a, 0x2b, 0x15, 0x29, 0x61, 0x07, 0x8a,
  0x37, 0x67, 0x40, 0x4b, 0x52, 0x4b, 0x52, 0x82, 0xaa, 0x1b, 0x5c, 0x1d, 0xaf, 0x71, 0xdc, 0xe1,
  0xb5, 0x82, 0xe6, 0x82, 0x49, 0xbd, 0x1b, 0x4d, 0x1f, 0xce, 0xaa, 0x01, 0x3c, 0x56, 0x31, 0xc9,
  0x6d, 0x28, 0xce, 0x50, 0x1d, 0x95, 0x64, 0x19, 0x14, 0xd8, 0x7d, 0x49, 0x9c, 0x30, 0xa7, 0x9a,
  0xf8, 0x16, 0x5a, 0xf7, 0x07, 0xeb, 0x83, 0xcf, 0x32, 0x76, 0xc7, 0xf2, 0x68, 0xea, 0xba, 0x95,
  0x7f, 0xc1, 0x40, 0x3c, 0x64, 0x9f, 0x1b, 0x2d, 0x87, 0x83, 0xd5, 0x64, 0xb5, 0xb1, 0x9d, 0x4e,
  0xa5, 0x85, 0x72, 0x60, 0xda, 0x84, 0x0b, 0xeb, 0xdb, 0x1a, 0x8f, 0x04, 0x87, 0x9c, 0xd5, 0xd2,
  0xb5, 0xa2, 0xdc, 0x11, 0xbb, 0xdf, 0x91, 0xbd, 0xe0, 0xa0, 0x49, 0xc6, 0xd4, 0x9e, 0x59, 0xc2,
  0x6a, 0x2e, 0x34, 0x11, 0xb9, 0x61, 0x25, 0x10, 0x28, 0x53, 0xe0, 0x44, 0xa7, 0x6f, 0x20, 0x73,
  0x43, 0x2a, 0x31, 0x81, 0x97, 0x1d, 0xb6, 0x14, 0x9c, 0x4b, 0x40, 0x93, 0x68, 0xae, 0x29, 0xd9,
  0x31, 0xde, 0xb9, 0xbe, 0x1f, 0x4d, 0x4a, 0xba, 0xe8, 0x94, 0x41, 0x7d, 0xd9, 0x1f, 0xf0, 0x44,
  0x69, 0x77, 0xd3, 0x63, 0xdf, 0xa9, 0x95, 0x13, 0x92, 0xe6, 0xfe, 0xa0, 0x7e, 0x79, 0x7b, 0x1b,
  0xd7, 0x0c, 0x75, 0xb9, 0xec, 0x5f, 0x75, 0x60, 0x9a, 0xb3, 0xe5, 0xf6, 0xfb, 0x25, 0x70, 0xc1,
  0x16, 0x37, 0xa5, 0x50, 0xf1, 0x83, 0xc6, 0xe3, 0xfd, 0xaa, 0x3a, 0xde, 0x36, 0x67, 0xfd, 0x91,
  0x37, 0xc8, 0xb5, 0xd7, 0x66, 0x3d, 0x3d, 0x3e, 0xbf, 0x3a, 0x0b, 0xb9, 0xab, 0xb3, 0xd6, 0xab,
  0xcd, 0xfd, 0xab, 0xd3, 0x02, 0x79, 0x7d, 0xde, 0xe6, 0xf9, 0x75, 0x27, 0x03, 0x79, 0x7d, 0xde,
  0xc3, 0xdd, 0xe3, 0xeb, 0xf3, 0x90, 0x6c, 0xdb, 0xa5, 0x35, 0x54, 0x2b, 0x79, 0x6a, 0x86, 0x9b,
  0x91, 0xa5, 0x56, 0xcb, 0xda, 0xc1, 0x76, 0x78, 0x07, 0xc5, 0x94, 0xf8, 0xe1, 0x50, 0xf5, 0xf1,
  0x64, 0x50, 0x8f, 0xea, 0x3d, 0x98, 0x5c, 0xea, 0x43, 0x12, 0x72, 0xb3, 0xcd, 0xa4, 0xa8, 0x12,
  0x03, 0x99, 0xbb, 0x59, 0x11, 0xfc, 0x7b, 0xbb, 0x3d, 0x14, 0xc2, 0x81, 0xbf, 0x3d, 0xb0, 0x77,
  0x1c, 0x0c, 0xab, 0x2e, 0xde, 0xd0, 0xed, 0x32, 0x17, 0x47, 0xe0, 0x67, 0x47, 0x50, 0x6c, 0x97,
  0xd1, 0x9f, 0xb9, 0x87, 0xed, 0x32, 0xde, 0xe2, 0x67, 0x2e, 0x22, 0xed, 0x12, 0x3f, 0x63, 0xd2,
  0x15, 0x5e, 0xd9, 0xab, 0xad, 0xe9, 0x1f, 0x00, 0xe1, 0x4a, 0xef, 0x06, 0x12, 0x72, 0xe7, 0x17,
  0x2d, 0x8f, 0xf4, 0xbe, 0x6f, 0x12, 0x14, 0xb1, 0xb5, 0x81, 0x72, 0xdb, 0x03, 0x38, 0x0b, 0x11,
  0x54, 0x7c, 0x9c, 0x2a, 0x2e, 0x1f, 0xe6, 0xaa, 0x88, 0xa1, 0xb2, 0x2f, 0xe0, 0x89, 0xbe, 0x07,
  0x26, 0xda, 0x88, 0x74, 0xba, 0x27, 0xba, 0x89, 0x8a, 0xde, 0xd9, 0x89, 0xd9, 0xde, 0x5f, 0xc4,
  0x50, 0xf5, 0x71, 0xac, 0xba, 0xbe, 0xa6, 0x3b, 0xf8, 0x90, 0x46, 0xe5, 0x39, 0x25, 0xe9, 0x66,
  0xe2, 0xdb, 0x80, 0x5f, 0x77, 0xbb, 0x63, 0x1c, 0x5d, 0x4f, 0x9c, 0xdc, 0xf4, 0x33, 0xdc, 0xdc,
  0xf9, 0x00, 0xdf, 0x4f, 0x1c, 0x8d, 0xe8, 0xdc, 0xfd, 0x80, 0x3f, 0x8f, 0xf1, 0x0d, 0xa2, 0xd8,
  0x3d, 0xa6, 0xbd, 0xc4, 0x67, 0x15, 0x2f, 0xae, 0x29, 0x37, 0x06, 0xa3, 0xca, 0x05, 0xd9, 0x55,
  0x97, 0x84, 0xe3, 0x00, 0x7a, 0x61, 0x30, 0x36, 0x61, 0x22, 0x86, 0x0a, 0xe1, 0x91, 0x17, 0x29,
  0x94, 0x42, 0x6f, 0x01, 0xe5, 0xec, 0x80, 0x47, 0xa0, 0x5d, 0x86, 0xf2, 0xbf, 0xe8, 0x45, 0x45,
  0x17, 0x8a, 0x02, 0x86, 0x4a, 0xf2, 0xc0, 0xc3, 0x00, 0x60, 0x1c, 0x11, 0x7b, 0x3c, 0x63, 0x11,
  0xca, 0x6b, 0x29, 0x23, 0xea, 0x3b, 0x98, 0xc7, 0x6c, 0x66, 0x00, 0xd4, 0x08, 0xdd, 0x17, 0xed,
  0xd2, 0x9f, 0xfa, 0x82, 0x7e, 0xf1, 0xf9, 0x43, 0x27, 0x7d, 0xf1, 0x65, 0x83, 0x62, 0x50, 0x78,
  0x40, 0xfe, 0x40, 0xef, 0x63, 0x23, 0x44, 0xd3, 0x07, 0xfa, 0x10, 0x65, 0x74, 0x00, 0xa1, 0xc7,
  0x01, 0x8a, 0xc8, 0xd3, 0x26, 0x42, 0xcf, 0x01, 0x41, 0x8f, 0xc6, 0x2d, 0x15, 0x57, 0xee, 0x61,
  0x1c, 0x8e, 0x28, 0x6c, 0x36, 0xf4, 0xe9, 0x28, 0x47, 0x6d, 0xe7, 0x79, 0x85, 0x86, 0x02, 0x25,
  0x77, 0x23, 0xe6, 0x0e, 0x93, 0x8e, 0x59, 0xea, 0x2a, 0xcd, 0xff, 0x4a, 0xd6, 0x8b, 0xf5, 0xc2,
  0x1b, 0x42, 0xcc, 0xc7, 0x32, 0xc0, 0x7e, 0xd4, 0x83, 0xb6, 0x30, 0x42, 0xbd, 0xd0, 0x55, 0x33,
  0x92, 0x92, 0x55, 0x4f, 0xee, 0x8c, 0x3e, 0x34, 0xc3, 0x28, 0x59, 0xf7, 0xe9, 0x0c, 0x15, 0x1e,
  0x86, 0xf8, 0xfd, 0x40, 0x2c, 0xf1, 0x70, 0xe7, 0xd1, 0xfe, 0x32, 0xbc, 0xbc, 0x1b, 0x97, 0xc2,
  0x41, 0x69, 0xa9, 0x75, 0xcc, 0xb8, 0x06, 0x6f, 0x32, 0x8a, 0x08, 0x96, 0x52, 0x80, 0xa3, 0x4e,
  0x06, 0x68, 0x60, 0xac, 0x94, 0x41, 0x30, 0xe2, 0x3f, 0xb0, 0x8b, 0xfc, 0x44, 0xfd, 0xfb, 0x23,
  0x8e, 0xfb, 0xf2, 0x41, 0x43, 0x9e, 0x18, 0xb4, 0xa2, 0xa1, 0x0b, 0xc5, 0x99, 0xb1, 0x14, 0xdc,
  0x01, 0x40, 0xcd, 0xf4, 0xb0, 0xd9, 0x46, 0xb6, 0x5d, 0x06, 0xf1, 0x48, 0x37, 0xdf, 0x1d, 0xdf,
  0xa3, 0x5f, 0xde, 0xfe, 0x6d, 0x2a, 0x86, 0x2f, 0x25, 0xa2, 0xb2, 0x81, 0x3d, 0x18, 0x8b, 0x5f,
  0x87, 0x4e, 0x5a, 0x57, 0xc6, 0x64, 0x76, 0x83, 0x85, 0xb2, 0xf8, 0x74, 0xb1, 0x67, 0xe6, 0xe6,
  0xda, 0xac, 0xbf, 0xb7, 0x6f, 0x45, 0xbd, 0x8d, 0xdb, 0x30, 0xf8, 0x7d, 0xbf, 0x42, 0xc0, 0x5e,
  0x2e, 0x9b, 0xe3, 0x59, 0xb2, 0x65, 0xc7, 0x31, 0xf6, 0xb4, 0x9f, 0xb0, 0xd7, 0x6e, 0x40, 0xda,
  0xa6, 0x6d, 0xd0, 0xf4, 0x56, 0x34, 0xed, 0x2e, 0xbd, 0xe0, 0xfe, 0xfb, 0x44, 0xd9, 0x5f, 0x4d,
  0x2b, 0x82, 0x2d, 0x07, 0xce, 0xda, 0x1b, 0xb6, 0x21, 0x5b, 0x4d, 0x52, 0xfc, 0x28, 0xca, 0x96,
  0x29, 0x92, 0x9e, 0xcd, 0x2a, 0xbe, 0x93, 0x74, 0xc6, 0xd8, 0x0b, 0x29, 0xe6, 0x3d, 0x83, 0x67,
  0xad, 0xcd, 0xd9, 0x64, 0x36, 0xcb, 0xf1, 0x26, 0xcb, 0xf1, 0x16, 0xbf, 0x8e, 0xb1, 0xf8, 0xd5,
  0xb1, 0x18, 0xdb, 0x60, 0x08, 0xf8, 0x2b, 0x03, 0x77, 0xde, 0x03, 0x6c, 0x78, 0xcf, 0xa8, 0xd6,
  0xe2, 0x7c, 0x2a, 0x8b, 0x63, 0x7d, 0xba, 0xcd, 0x50, 0x30, 0x68, 0xf5, 0xf5, 0xc6, 0x44, 0x8f,
  0x86, 0x16, 0x62, 0x34, 0x64, 0xb9, 0xd5, 0x6e, 0xc2, 0xa7, 0x81, 0x7e, 0xdc, 0x7b, 0x0d, 0x67,
  0x1a, 0x60, 0x39, 0x34, 0xd4, 0x96, 0x1f, 0x29, 0xe9, 0x1b, 0x0b, 0xeb, 0x6d, 0x16, 0x75, 0xae,
  0x92, 0x24, 0x7c, 0x12, 0x6d, 0xb2, 0x54, 0x76, 0xd3, 0x0d, 0x94, 0x00, 0x57, 0xaa, 0x14, 0x2a,
  0x95, 0xe6, 0xa7, 0xc5, 0x2f, 0x30, 0xc0, 0x1d, 0x19, 0x09, 0x49, 0x16, 0x20, 0x64, 0xab, 0xb7,
  0x43, 0x5c, 0xd9, 0xad, 0x90, 0x31, 0xf7, 0x60, 0xe3, 0xba, 0x11, 0xb0, 0xd1, 0x5c, 0xbd, 0x2a,
  0x75, 0x30, 0x77, 0x38, 0x8e, 0xef, 0xbe, 0xc5, 0xf6, 0x14, 0x6d, 0x42, 0xba, 0xc6, 0x50, 0xaa,
  0x3e, 0xbc, 0xec, 0x10, 0x3a, 0x80, 0x6b, 0x5f, 0x1e, 0x8e, 0x48, 0x69, 0xfe, 0x79, 0xdc, 0xb7,
  0x22, 0xdb, 0x5b, 0x57, 0x47, 0xad, 0xa2, 0x26, 0x82, 0xf7, 0x16, 0xb0, 0x82, 0x31, 0x57, 0x5d,
  0x96, 0x62, 0xac, 0xcb, 0x83, 0x6f, 0xfd, 0xf8, 0x60, 0xe8, 0xad, 0x77, 0x0f, 0x8f, 0xc0, 0xf3,
  0xc1, 0x9b, 0x35, 0x04, 0x1b, 0x8b, 0xcb, 0x0b, 0x56, 0xe3, 0x97, 0x0f, 0x23, 0x9d, 0xe5, 0xe3,
  0x18, 0xe9, 0x18, 0x8c, 0xc4, 0x60, 0xc8, 0xd4, 0xfc, 0xca, 0x61, 0x8c, 0x5b, 0xaa, 0x17, 0xac,
  0xe7, 0x1a, 0xea, 0x6f, 0x99, 0x6a, 0x27, 0x89, 0x02, 0xf5, 0x69, 0x69, 0xeb, 0x2f, 0x4a, 0x63,
  0x0a, 0x18, 0xe9, 0x57, 0x9d, 0x27, 0xe1, 0xba, 0xd3, 0x42, 0x94, 0xe4, 0xa0, 0x41, 0xcd, 0xd6,
  0x29, 0x72, 0x84, 0x66, 0x95, 0xa5, 0xc6, 0xea, 0x8c, 0x2c, 0x0f, 0xd2, 0xad, 0xc2, 0x75, 0x63,
  0x51, 0x83, 0xed, 0xd6, 0x63, 0xcc, 0x36, 0xaa, 0x75, 0x62, 0x7e, 0x32, 0x50, 0x75, 0xb8, 0x6e,
  0x99, 0x14, 0x42, 0xca, 0x8c, 0x0e, 0x9f, 0x43, 0xba, 0x4d, 0xfd, 0x2b, 0x58, 0x35, 0xee, 0x91,
  0x7c, 0x21, 0xf2, 0x41, 0xc5, 0x2c, 0x2b, 0x77, 0xe8, 0x4b, 0xd9, 0x59, 0x61, 0xbb, 0xff, 0x84,
  0xdb, 0xa0, 0xb5, 0xc0, 0x6f, 0xc0, 0x8a, 0xd7, 0xf5, 0x96, 0x76, 0xbe, 0x51, 0xf7, 0x93, 0xce,
  0x04, 0x2d, 0x8e, 0x2c, 0x87, 0xb4, 0x83, 0x30, 0x2f, 0xf4, 0xb1, 0xf6, 0xb8, 0xd0, 0x74, 0x28,
  0x36, 0x35, 0x61, 0x13, 0x8b, 0x5a, 0x2f, 0xd0, 0xc5, 0xb2, 0xb9, 0xb0, 0xe5, 0x95, 0x9a, 0x8f,
  0xe5, 0xad, 0xf9, 0x3f, 0xde, 0x94, 0x37, 0xab, 0x9b, 0xcb, 0x6f, 0xf1, 0xe3, 0x5c, 0x22, 0x4b,
  0x85, 0x4d, 0xd7, 0xd0, 0x54, 0x4b, 0x03, 0x29, 0x6d, 0x49, 0x05, 0x68, 0xd9, 0x1b, 0x4d, 0x36,
  0x94, 0xbc, 0xa5, 0x60, 0xe4, 0xbe, 0x15, 0x63, 0x87, 0x67, 0x50, 0xd8, 0xe9, 0x99, 0x44, 0xb5,
  0x23, 0x4a, 0x26, 0xb9, 0xd2, 0x96, 0x8a, 0x01, 0xb9, 0x3b, 0xc0, 0x27, 0x0f, 0x53, 0x5e, 0xdb,
  0x89, 0xb5, 0x07, 0x51, 0x08, 0x23, 0xbc, 0x60, 0xc2, 0x17, 0xb1, 0xee, 0xca, 0xe7, 0xca, 0x37,
  0x82, 0xc8, 0x18, 0x93, 0x75, 0x06, 0xc8, 0x63, 0x69, 0xcc, 0xf5, 0x12, 0x1e, 0x6d, 0x39, 0x70,
  0x2c, 0x25, 0xed, 0xec, 0x74, 0x6c, 0x26, 0xbc, 0x49, 0x89, 0xd1, 0x75, 0xcd, 0x42, 0xc8, 0xcf,
  0x98, 0x35, 0xa5, 0x61, 0xdb, 0x49, 0x73, 0x2e, 0x11, 0xcc, 0x58, 0x9b, 0xc4, 0xa7, 0x42, 0xcb,
  0xbf, 0x51, 0xc7, 0x69, 0x6c, 0x49, 0xf7, 0xbc, 0x02, 0x23, 0xe3, 0x85, 0x9d, 0x62, 0xa7, 0x76,
  0xa0, 0x62, 0xc5, 0xa6, 0x4d, 0xd9, 0x6e, 0x3e, 0xc2, 0x95, 0x96, 0x31, 0xf7, 0xd1, 0x6d, 0x47,
  0xb9, 0x54, 0xfa, 0x58, 0x9c, 0xfb, 0x74, 0xf7, 0xc0, 0xe4, 0x7d, 0xb6, 0xc8, 0x8e, 0x2d, 0x8e,
  0xed, 0x43, 0xda, 0xf7, 0x3b, 0xae, 0xdd, 0xa3, 0xdc, 0x8a, 0x5a, 0x11, 0x4a, 0xc4, 0xbd, 0x3c,
  0x74, 0x8a, 0x6e, 0x3a, 0x49, 0x1a, 0xe6, 0x6e, 0xfd, 0xb0, 0xa1, 0x12, 0x5c, 0x87, 0x9d, 0xbf,
  0xb1, 0x9f, 0xef, 0x36, 0x9f, 0x3b, 0x2c, 0x49, 0xac, 0x17, 0x93, 0x40, 0x7f, 0xba, 0x22, 0xe7,
  0x39, 0xd5, 0x13, 0x30, 0xcc, 0x8a, 0x6c, 0xd7, 0x2b, 0x7b, 0xbb, 0xe9, 0x83, 0x7f, 0xce, 0x4f,
  0x66, 0x9f, 0x22, 0xfe, 0x97, 0x0d, 0x05, 0x4c, 0x4e, 0x2d, 0x27, 0x4e, 0x34, 0x66, 0xea, 0xbc,
  0x96, 0x89, 0xc4, 0xb7, 0xbb, 0xfe, 0xb7, 0x31, 0xc4, 0xf9, 0x1e, 0x67, 0x49, 0x76, 0x7f, 0xca,
  0x86, 0x95, 0x4a, 0x9d, 0x0b, 0x99, 0xa7, 0x42, 0x16, 0xe1, 0x6a, 0x75, 0x80, 0x5a, 0xf6, 0x32,
  0x0b, 0x54, 0x1b, 0xe2, 0x02, 0x63, 0xe4, 0x42, 0x93, 0x17, 0x42, 0x75, 0x16, 0xc3, 0x3a, 0xb5,
  0x16, 0xb9, 0x51, 0x2b, 0x83, 0x75, 0x2e, 0x02, 0x96, 0x4a, 0x8a, 0x4c, 0x0a, 0x1d, 0xee, 0xd2,
  0x32, 0xc0, 0xa5, 0x85, 0x50, 0x99, 0x48, 0x43, 0xe6, 0x8e, 0x57, 0x73, 0xbf, 0x42, 0xfe, 0x46,
  0xee, 0xd7, 0x59, 0x17, 0xd6, 0x0e, 0x4e, 0x77, 0x0e, 0x1d, 0xba, 0x52, 0xf3, 0x4b, 0x04, 0x69,
  0x4b, 0x84, 0xf9, 0xd7, 0xe8, 0x89, 0xe5, 0x2c, 0x6a, 0x98, 0xca, 0xfc, 0x28, 0x59, 0xed, 0xf2,
  0xfd, 0xfd, 0x3c, 0x5c, 0x21, 0x8c, 0xe9, 0x08, 0x1d, 0xeb, 0xf5, 0x8b, 0x55, 0xac, 0x16, 0x08,
  0x09, 0x4f, 0x4f, 0x52, 0xd4, 0x76, 0x10, 0xcd, 0xa2, 0x8e, 0x5c, 0x8f, 0x00, 0xa0, 0xe0, 0x14,
  0x73, 0xfd, 0xe0, 0xf3, 0x8b, 0x3a, 0xf6, 0x4b, 0xa2, 0x7b, 0x26, 0x8e, 0x11, 0x1f, 0x61, 0x46,
  0xfb, 0xad, 0x3b, 0x23, 0x95, 0x90, 0x81, 0x9a, 0xdc, 0x4e, 0xff, 0xf1, 0x2c, 0xf7, 0xc2, 0x90,
  0x65, 0x42, 0x02, 0x79, 0xf0, 0x7a, 0xbb, 0x6a, 0xbd, 0x73, 0xf7, 0x68, 0x7f, 0x4f, 0x1f, 0x51,
  0xf0, 0x47, 0xd1, 0x42, 0xfd, 0x3f, 0x00, 0xd5, 0xdd, 0x67, 0x33, 0x58, 0xc6, 0x1b, 0xd0, 0x18,
  0x11, 0x9d, 0x5d, 0x20, 0xf1, 0x36, 0x34, 0xd0, 0x78, 0x27, 0x5a, 0x4b, 0x44, 0x33, 0x1a, 0x23,
  0xa0, 0x53, 0x0b, 0x24, 0xb4, 0xa4, 0xe1, 0x9a, 0x77, 0xa5, 0x59, 0xc3, 0xed, 0x3b, 0xcc, 0x56,
  0x6c, 0x6a, 0x87, 0xed, 0x0e, 0x8c, 0xf2, 0x68, 0x44, 0x8d, 0xfe, 0xd7, 0xa5, 0x65, 0x45, 0x6e,
  0x88, 0x1d, 0xf1, 0x43, 0x6d, 0xe4, 0x74, 0xbf, 0x34, 0x54, 0x0a, 0x84, 0x6d, 0xfb, 0xcb, 0xfd,
  0xba, 0x62, 0x9a, 0x9f, 0x37, 0x0f, 0xe4, 0xdb, 0x9b, 0x97, 0xcf, 0x75, 0x4f, 0xe2, 0xd5, 0xcb,
  0xe5, 0xe6, 0xca, 0x64, 0x48, 0xbf, 0x9a, 0x18, 0xe7, 0x53, 0x9c, 0x45, 0x49, 0x14, 0xab, 0x48,
  0xce, 0xf8, 0x90, 0x6b, 0x9a, 0x18, 0xc9, 0xc6, 0x52, 0xef, 0xb8, 0xe1, 0xd8, 0x95, 0xbc, 0x96,
  0x37, 0xbc, 0x73, 0x61, 0x9f, 0xcf, 0x5b, 0x11, 0xf6, 0xda, 0xf6, 0x06, 0x8e, 0xde, 0x02, 0x8e,
  0xa8, 0xbe, 0xd1, 0x22, 0x54, 0xf6, 0xdf, 0x1a, 0xf2, 0x6a, 0x7a, 0xf5, 0xbe, 0x56, 0x7d, 0x50,
  0x6b, 0x82, 0x7d, 0x36, 0xc1, 0x1d, 0x82, 0x4b, 0x9b, 0xa1, 0xf0, 0x8a, 0x29, 0xaa, 0x52, 0x51,
  0x3a, 0xef, 0x62, 0x89, 0x1c, 0x4c, 0x91, 0x24, 0x4b, 0x4b, 0x0a, 0x3f, 0xdf, 0x96, 0xa2, 0x58,
  0x08, 0x25, 0xfb, 0xb3, 0x25, 0x80, 0x41, 0x42, 0x06, 0x30, 0x93, 0xa7, 0x65, 0x1c, 0x20, 0x05,
  0xa0, 0xa9, 0x73, 0x34, 0x00, 0x6a, 0xe3, 0xe4, 0x05, 0xa0, 0x06, 0x43, 0xb0, 0x44, 0xe4, 0x59,
  0x40, 0x58, 0xcb, 0x10, 0x10, 0x8a, 0x01, 0x61, 0x9a, 0x80, 0xcc, 0x76, 0x30, 0x5d, 0x2c, 0xbf,
  0x9a, 0xe5, 0x58, 0xda, 0xbf, 0x0f, 0x5f, 0x6d, 0xff, 0xda, 0x7e, 0x29, 0x13, 0x70, 0x9b, 0x3e,
  0x57, 0x24, 0xff, 0x20, 0x18, 0x9a, 0x62, 0xe4, 0x6d, 0xd8, 0xfd, 0x19, 0x46, 0xe0, 0xa1, 0x1d,
  0x89, 0x2a, 0x7b, 0x0d, 0x18, 0xfa, 0xab, 0x28, 0x10, 0xb1, 0xfd, 0xb4, 0x82, 0x25, 0x7c, 0x9c,
  0xfe, 0x25, 0x12, 0x7a, 0xd3, 0x65, 0x5f, 0x9e, 0x8a, 0x9b, 0xec, 0x69, 0x00, 0x87, 0x59, 0xbc,
  0x1d, 0x7d, 0x26, 0x6c, 0x90, 0x00, 0x96, 0xa7, 0x2f, 0x7d, 0xe6, 0xe5, 0xe1, 0x8d, 0xdf, 0xd2,
  0x03, 0x40, 0x6c, 0x9d, 0x80, 0xe6, 0xf6, 0x73, 0x1f, 0x9e, 0x0b, 0x38, 0x2a, 0xb6, 0xf0, 0x05,
  0x14, 0x9c, 0xea, 0x89, 0x30, 0xba, 0x4e, 0xcc, 0xbe, 0x0b, 0x27, 0x66, 0x1f, 0xdb, 0x89, 0xf6,
  0x19, 0x4a, 0x93, 0xc4, 0xf1, 0x25, 0xdf, 0x93, 0xfc, 0x1f, 0x33, 0xc8, 0xac, 0x10, 0xb2, 0x2c,
  0x84, 0xd2, 0x29, 0x1e, 0x23, 0x88, 0xf9, 0x1e, 0x22, 0x0e, 0xa7, 0xc7, 0xc3, 0xf1, 0x87, 0x36,
  0x1c, 0x0d, 0xdb, 0xed, 0x68, 0x34, 0xbe, 0xe1, 0xf6, 0xc3, 0xa1, 0x23, 0xed, 0x62, 0xd9, 0x94,
  0xe7, 0x4d, 0x65, 0x6c, 0xbe, 0xa2, 0x8c, 0xec, 0xfa, 0xfa, 0xa8, 0x5d, 0x8d, 0xf1, 0xf8, 0x48,
  0xe4, 0x58, 0x75, 0xd2, 0x8e, 0x80, 0x85, 0xb1, 0x36, 0x92, 0xab, 0xa7, 0xfb, 0x8b, 0x25, 0x2f,
  0xc9, 0xd1, 0x2b, 0x71, 0xb3, 0xfb, 0x64, 0x39, 0xe2, 0xa7, 0xd9, 0x44, 0xe5, 0x39, 0x9c, 0xed,
  0x07, 0x6a, 0x73, 0x30, 0x88, 0x97, 0x78, 0x74, 0xd7, 0xa0, 0xe0, 0xbd, 0xd8, 0x9f, 0xf9, 0xc0,
  0xc8, 0x21, 0xd4, 0x7c, 0x3c, 0xaa, 0x3f, 0x31, 0x12, 0x0e, 0x59, 0x9c, 0x08, 0x09, 0xdf, 0x86,
  0x42, 0xe2, 0x51, 0x71, 0xe1, 0x02, 0x22, 0x86, 0x9e, 0x77, 0xdc, 0x6d, 0x2f, 0x58, 0x79, 0x22,
  0x98, 0x01, 0xd2, 0x0b, 0x83, 0xb7, 0x08, 0xc3, 0x82, 0x1f, 0xc7, 0x63, 0x71, 0x3f, 0x86, 0x43,
  0x2d, 0x4e, 0x84, 0x42, 0x5a, 0x2a, 0x1c, 0xea, 0x64, 0x0f, 0x22, 0x38, 0x0a, 0x82, 0x42, 0xc5,
  0xe7, 0x64, 0x30, 0x99, 0x0a, 0xca, 0x61, 0x86, 0x2c, 0x17, 0xaa, 0x37, 0xc7, 0x5e, 0xac, 0x4b,
  0x39, 0xf2, 0x46, 0x95, 0x69, 0x01, 0xe9, 0x36, 0xf0, 0xcc, 0x1c, 0xe9, 0x81, 0xe0, 0xb0, 0xa4,
  0xb4, 0x2a, 0xe5, 0xd6, 0x92, 0xfe, 0xf0, 0x90, 0xe3, 0x20, 0x68, 0x0f, 0xc7, 0xf1, 0x47, 0xec,
  0x61, 0x40, 0x2a, 0x18, 0xc8, 0x1f, 0xb1, 0x07, 0x03, 0xa5, 0xe1, 0x40, 0xfe, 0x7a, 0x79, 0x28,
  0x90, 0xbb, 0x44, 0x93, 0xa5, 0xbe, 0x01, 0x84, 0xcc, 0x2a, 0x47, 0x99, 0x2a, 0xcb, 0x0a, 0x14,
  0xe7, 0xa8, 0x98, 0xe3, 0x34, 0xc2, 0x31, 0x27, 0x2e, 0xe0, 0xbb, 0xc0, 0xd8, 0xce, 0x00, 0x7c,
  0xc6, 0x4d, 0x0d, 0xb8, 0xdc, 0xbb, 0x80, 0x3d, 0x9f, 0x8f, 0x8f, 0xe2, 0xb7, 0x05, 0x22, 0x2a,
  0x8e, 0x1a, 0x18, 0x5d, 0xf9, 0x0f, 0x9b, 0x10, 0x1a, 0x82, 0x87, 0xaf, 0x4f, 0x86, 0xe7, 0xde,
  0xee, 0x45, 0xef, 0x6b, 0xc6, 0xdf, 0x3d, 0x5c, 0x2c, 0x47, 0x95, 0x7f, 0xfb, 0xdb, 0xed, 0xb7,
  0xb7, 0x56, 0xdd, 0xa2, 0xab, 0x4e, 0x1d, 0x2b, 0x53, 0x56, 0x95, 0xfd, 0x8a, 0xd0, 0x69, 0x68,
  0xaf, 0xbf, 0xf6, 0x8a, 0x77, 0x7f, 0xaf, 0x4d, 0xce, 0x66, 0x7e, 0xbd, 0x5b, 0xdd, 0x14, 0xcf,
  0x9d, 0x36, 0xd8, 0xcd, 0xee, 0xf7, 0xbf, 0x9e, 0x3c, 0x42, 0x91, 0xe1, 0xfc, 0xb6, 0xdd, 0x5c,
  0xdd, 0x1f, 0xb3, 0xdc, 0x06, 0xd3, 0xe7, 0xa7, 0x2e, 0x5f, 0x18, 0xe5, 0xf5, 0x67, 0x0f, 0x5d,
  0xfa, 0x4c, 0xd4, 0x9f, 0x52, 0xf9, 0x99, 0xdc, 0x2a, 0xfb, 0xcd, 0x88, 0x8d, 0x55, 0xed, 0xa7,
  0x41, 0x7f, 0xfe, 0x7a, 0x75, 0xb7, 0xbe, 0xba, 0x37, 0x07, 0x9f, 0x3f, 0x66, 0xc7, 0xad, 0xdc,
  0x55, 0xf7, 0xa3, 0x8d, 0x56, 0x4d, 0xe5, 0xb6, 0x50, 0x55, 0xd3, 0xf7, 0x1a, 0x55, 0xd3, 0xf6,
  0x6d, 0xf4, 0xcd, 0xfc, 0x5c, 0xc9, 0x8c, 0x6a, 0xf2, 0xb3, 0x96, 0xe8, 0x9d, 0x06, 0x9e, 0x4d,
  0xac, 0xbf, 0x5c, 0x5c, 0x97, 0x5f, 0x2e, 0x77, 0x82, 0xdd, 0x16, 0x8c, 0x3c, 0x3b, 0xc7, 0x91,
  0xa1, 0x9a, 0x30, 0xfc, 0xaa, 0xa6, 0xda, 0x54, 0xeb, 0x87, 0xe3, 0xbf, 0x7d, 0x7f, 0x57, 0x13,
  0xe4, 0x02, 0xe1, 0xa9, 0xef, 0xe8, 0x79, 0x80, 0xc5, 0xff, 0xdf, 0xfa, 0x18, 0xe0, 0xd9, 0xf5,
  0xec, 0x4e, 0xd1, 0x6e, 0x14, 0xfc, 0xcb, 0x13, 0xdc, 0xf4, 0xf4, 0xa7, 0xaf, 0x25, 0xf7, 0x1c,
  0x37, 0xfe, 0x34, 0x5d, 0x24, 0xbe, 0x99, 0x9d, 0x2b, 0xc5, 0x25, 0xfc, 0x68, 0x67, 0xba, 0xe9,
  0xc2, 0x93, 0x25, 0x77, 0xca, 0xfd, 0x98, 0x8b, 0x96, 0x2c, 0xa8, 0x89, 0xa9, 0x72, 0xba, 0x8d,
  0xa6, 0xd9, 0x3f, 0xee, 0xd4, 0x78, 0xa7, 0x4c, 0x75, 0xab, 0xdf, 0x65, 0x83, 0x9f, 0xc1, 0x61,
  0xea, 0x41, 0x4d, 0x64, 0x95, 0xd3, 0xae, 0x55, 0x4d, 0xd2, 0x52, 0x73, 0x97, 0xbd, 0x9f, 0x87,
  0x56, 0x13, 0x35, 0x70, 0xad, 0x26, 0x2d, 0x70, 0xad, 0xa6, 0x75, 0x48, 0x78, 0x6a, 0x03, 0xf3,
  0x98, 0x1c, 0x9f, 0xe5, 0xe9, 0xfb, 0x0d, 0xd8, 0xae, 0x2e, 0xd7, 0x5b, 0xe7, 0x11, 0xf6, 0x86,
  0x26, 0x41, 0x6d, 0x8c, 0xe1, 0x37, 0x6c, 0x70, 0x5f, 0x19, 0x6c, 0x7c, 0xbf, 0x7b, 0xf6, 0xd1,
  0xb3, 0xc3, 0x7d, 0xfb, 0xe1, 0x1f, 0xdd, 0xc7, 0x0b, 0x88, 0x6a, 0x50, 0x00, 0x00
};

inline void serveAppImmutableAssetsIndexDuGlLd2FCss(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", APP_IMMUTABLE_ASSETS_INDEX_DUGLLD2F_CSS, APP_IMMUTABLE_ASSETS_INDEX_DUGLLD2F_CSS_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}

