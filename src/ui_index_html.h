/*
   * Binary array for the Web UI.
   * gzip is used for smaller size and improved speeds.
   *
   * Please see https://kno.wled.ge/advanced/custom-features/#changing-web-ui
   * to find out how to easily modify the web UI source!
   */

  // Autogenerated do not edit!!
  const uint16_t INDEX_HTML_L = 247;
  const uint8_t INDEX_HTML[] PROGMEM = {
    0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x13, 0x4d, 0x50, 0x3d, 0x53, 0xc3, 0x30,
  0x0c, 0xdd, 0xfb, 0x2b, 0x84, 0x66, 0xd2, 0xc0, 0xc6, 0x71, 0x76, 0x16, 0x5a, 0x56, 0x7a, 0x47,
  0x19, 0x18, 0x5d, 0x5b, 0x60, 0x81, 0xe3, 0xe4, 0x6c, 0x91, 0x36, 0xff, 0x1e, 0x3b, 0x81, 0x83,
  0x49, 0x1f, 0xef, 0xe9, 0x3d, 0x49, 0xea, 0x6a, 0xf7, 0xf4, 0x70, 0x7c, 0x3d, 0xec, 0xc1, 0x4b,
  0x1f, 0xba, 0x8d, 0xaa, 0x01, 0x82, 0x89, 0xef, 0x1a, 0x29, 0x62, 0xb7, 0x01, 0x50, 0x9e, 0x8c,
  0xab, 0x49, 0x49, 0x7b, 0x12, 0x03, 0xd6, 0x9b, 0x94, 0x49, 0x34, 0xbe, 0x1c, 0x1f, 0x9b, 0x3b,
  0x84, 0xf6, 0x3f, 0x18, 0x4d, 0x4f, 0x1a, 0x27, 0xa6, 0xf3, 0x38, 0x24, 0x41, 0xb0, 0x43, 0x14,
  0x8a, 0x85, 0x7c, 0x66, 0x27, 0x5e, 0x3b, 0x9a, 0xd8, 0x52, 0xb3, 0x14, 0xd7, 0xc0, 0x91, 0x85,
  0x4d, 0x68, 0xb2, 0x35, 0x81, 0xf4, 0xed, 0xf6, 0xe6, 0x4f, 0x4c, 0x58, 0x02, 0x75, 0xfb, 0xe7,
  0x43, 0xa2, 0x4c, 0x31, 0xd3, 0x3d, 0xec, 0x96, 0xd1, 0xac, 0xda, 0x15, 0xaa, 0x9b, 0x05, 0x8e,
  0x9f, 0x90, 0x28, 0x68, 0xcc, 0x32, 0x07, 0xca, 0x9e, 0xa8, 0x58, 0xca, 0x3c, 0x96, 0x15, 0x84,
  0x2e, 0xd2, 0xda, 0x9c, 0x11, 0x7c, 0xa2, 0x37, 0x8d, 0xa7, 0xaf, 0xe8, 0x02, 0x6d, 0x6b, 0xa7,
  0x9c, 0xd9, 0xfe, 0x1e, 0xa5, 0x4e, 0x83, 0x9b, 0x7f, 0x3c, 0x1d, 0x4f, 0xc0, 0x4e, 0xa3, 0x19,
  0x47, 0xec, 0x54, 0x5b, 0xca, 0x85, 0x91, 0x6d, 0xe2, 0x51, 0x00, 0x72, 0xb2, 0x1a, 0x39, 0x3a,
  0xba, 0x6c, 0x3f, 0x72, 0x25, 0xac, 0x40, 0x55, 0x5b, 0x45, 0x8a, 0xea, 0xf2, 0xc4, 0x6f, 0xaf,
  0xf4, 0x09, 0x10, 0x55, 0x01, 0x00, 0x00
  };

  void serveIndexHtml(AsyncWebServerRequest* request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", INDEX_HTML, INDEX_HTML_L);
    response->addHeader(F("Content-Encoding"), "gzip");
    request->send(response);
  }
  