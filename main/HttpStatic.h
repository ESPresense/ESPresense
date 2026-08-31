#pragma once
// Helpers the generated ui_*.h headers (ui/plugins/cpp.ts) are written against.
#include <string>

#include "esp_http_server.h"

namespace HttpWebServer {
// Serve an embedded (usually gzip'd) asset.
esp_err_t serveStatic(httpd_req_t* req, const char* contentType, const uint8_t* data, size_t len, bool gzip, bool immutable);
void registerGet(httpd_handle_t server, const char* uri, esp_err_t (*handler)(httpd_req_t*));
}  // namespace HttpWebServer

namespace HttpWebServer {
// CORS + hardening headers every response carries (was DefaultHeaders in the Arduino build).
void commonHeaders(httpd_req_t* req);
// Read the whole request body (capped); returns false if it did not fit.
bool readBody(httpd_req_t* req, std::string& body, size_t cap = 8192);
// Query-string value lookup with %xx decoding; returns false when absent.
bool queryParam(httpd_req_t* req, const char* key, std::string& value);
std::string urlDecode(const char* s, size_t len);
}  // namespace HttpWebServer
