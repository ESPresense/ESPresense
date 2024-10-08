/*
 * Binary array for the Web UI.
 * Gzip is used for smaller size and improved speeds.
 */

// Autogenerated do not edit!!
const uint16_t INDEX_HTML_L = 228;
const uint8_t INDEX_HTML[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x13, 0x2d, 0x50, 0x41, 0x4e, 0xc4, 0x30,
  0x10, 0xfb, 0x4a, 0xc8, 0x99, 0x6e, 0xc4, 0x8d, 0x43, 0xd2, 0x1b, 0x9c, 0x91, 0x80, 0x07, 0x64,
  0x13, 0xb3, 0x1d, 0x48, 0x93, 0x28, 0x33, 0xed, 0xd2, 0xdf, 0x93, 0xb6, 0x5c, 0x2c, 0x79, 0x3c,
  0xb6, 0x3c, 0x63, 0x1f, 0x62, 0x09, 0xb2, 0x55, 0xa8, 0x49, 0xe6, 0x34, 0xda, 0x1d, 0x55, 0xf2,
  0xf9, 0xe6, 0x34, 0xb2, 0xee, 0x1c, 0x3e, 0x8e, 0x76, 0x86, 0x78, 0x15, 0x26, 0xdf, 0x18, 0xe2,
  0xf4, 0xe7, 0xc7, 0xeb, 0xf0, 0xac, 0xcd, 0xff, 0x38, 0xfb, 0x19, 0x4e, 0xaf, 0x84, 0x7b, 0x2d,
  0x4d, 0xb4, 0x0a, 0x25, 0x0b, 0x72, 0x5f, 0xbb, 0x53, 0x94, 0xc9, 0x45, 0xac, 0x14, 0x30, 0x1c,
  0xe4, 0x91, 0x32, 0x09, 0xf9, 0x34, 0x70, 0xf0, 0x09, 0xee, 0x69, 0xcf, 0x10, 0x92, 0x84, 0xf1,
  0xe5, 0xfd, 0xad, 0x81, 0x91, 0x19, 0xd6, 0x9c, 0x13, 0xcb, 0xa1, 0x51, 0x15, 0xb5, 0x77, 0x73,
  0x7a, 0x2e, 0x71, 0x49, 0xe8, 0xe1, 0xad, 0x30, 0x97, 0x46, 0x37, 0xca, 0x8a, 0x5b, 0x70, 0xda,
  0x2c, 0x64, 0x28, 0x47, 0xfc, 0x5e, 0xbe, 0xb9, 0xd7, 0x35, 0xa7, 0x6b, 0xb4, 0x89, 0xf2, 0x8f,
  0x6a, 0x48, 0x4e, 0xb3, 0x6c, 0x09, 0x3c, 0x01, 0xbd, 0xdb, 0xd4, 0xf0, 0x75, 0x7a, 0xae, 0x4b,
  0x8e, 0x09, 0x97, 0xc0, 0x87, 0xeb, 0xbc, 0xf2, 0x5a, 0xe2, 0x36, 0xda, 0x48, 0xab, 0xa2, 0xe8,
  0xb4, 0xaf, 0x75, 0x97, 0x3a, 0xed, 0x78, 0x4a, 0xe6, 0xf8, 0xd1, 0x1f, 0x23, 0xd1, 0x01, 0x74,
  0x33, 0x01, 0x00, 0x00
};

void serveIndexHtml(AsyncWebServerRequest* request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", INDEX_HTML, INDEX_HTML_L);
  response->addHeader(F("Content-Encoding"), "gzip");
  request->send(response);
}
