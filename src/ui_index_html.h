/*
 * Binary array for the Web UI.
 * Gzip is used for smaller size and improved speeds.
 */

// Autogenerated do not edit!!
const uint16_t INDEX_HTML_L = 230;
const uint8_t INDEX_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0a, 0x2d, 0x90, 0xc1, 0x4e, 0xc4, 0x30,
  0x0c, 0x44, 0x7f, 0x25, 0xf8, 0x4c, 0x37, 0xe2, 0xc6, 0x21, 0xc9, 0x0d, 0xce, 0x48, 0xc0, 0x07,
  0x64, 0x93, 0x61, 0x6b, 0x48, 0x93, 0x2a, 0x76, 0xbb, 0xf4, 0xef, 0x51, 0x5b, 0x2e, 0x96, 0x3c,
  0xa3, 0x37, 0xf2, 0xd8, 0x3d, 0xe4, 0x96, 0x74, 0x9b, 0x61, 0x46, 0x9d, 0x4a, 0x70, 0xfb, 0x34,
  0x25, 0xd6, 0x9b, 0x27, 0x54, 0x0a, 0x6e, 0x44, 0xcc, 0xc1, 0x4d, 0xd0, 0x68, 0xd2, 0x18, 0xbb,
  0x40, 0x3d, 0x7d, 0x7e, 0xbc, 0x0e, 0xcf, 0x64, 0xff, 0xe5, 0x1a, 0x27, 0x78, 0x5a, 0x19, 0xf7,
  0xb9, 0x75, 0x25, 0x93, 0x5a, 0x55, 0x54, 0xf5, 0x74, 0xe7, 0xac, 0xa3, 0xcf, 0x58, 0x39, 0x61,
  0x38, 0x96, 0x47, 0xae, 0xac, 0x1c, 0xcb, 0x20, 0x29, 0x16, 0xf8, 0xa7, 0x3d, 0x43, 0x59, 0x0b,
  0xc2, 0xcb, 0xfb, 0x5b, 0x87, 0xa0, 0x0a, 0x9c, 0x3d, 0x15, 0x27, 0xa9, 0xf3, 0xac, 0x66, 0xbf,
  0xcd, 0xd3, 0xd4, 0xf2, 0x52, 0x40, 0x26, 0xf5, 0x26, 0xd2, 0x3a, 0xdf, 0xb8, 0x1a, 0xe9, 0xc9,
  0x93, 0x5d, 0xd8, 0x72, 0xcd, 0xf8, 0xbd, 0x7c, 0x0b, 0x05, 0x67, 0x4f, 0x2a, 0xb8, 0xc2, 0xf5,
  0xc7, 0x74, 0x14, 0x4f, 0xa2, 0x5b, 0x81, 0x8c, 0x80, 0x92, 0x19, 0x3b, 0xbe, 0x4e, 0xe6, 0xba,
  0xd4, 0x5c, 0x70, 0x49, 0x72, 0x50, 0x67, 0xcb, 0x6b, 0xcb, 0x5b, 0x70, 0x99, 0x57, 0xc3, 0xd9,
  0x53, 0x9c, 0xe7, 0xdd, 0xca, 0xbc, 0x06, 0x67, 0x4f, 0xcb, 0x1e, 0x3f, 0xfa, 0x03, 0x23, 0xd1,
  0x01, 0x74, 0x33, 0x01, 0x00, 0x00
};

void serveIndexHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", INDEX_HTML, INDEX_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}
