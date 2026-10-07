#pragma once
// Persistent settings: one SPIFFS file per key ("/name"), the same layout the Arduino
// firmware used, so a node OTA'd to this build keeps its configuration.
// Registration order defines the /wifi, /wifi/extras and /wifi/hardware endpoints the UI reads.
#include <string>
#include <vector>

#include "esp_http_server.h"

namespace Settings {
void begin();  // mount SPIFFS (formats on first use)

std::string string(const std::string& name, const std::string& init = "", const std::string& label = "");
std::string pstring(const std::string& name, const std::string& init = "", const std::string& label = "");
long dropdown(const std::string& name, const std::vector<std::string>& options, long init = 0, const std::string& label = "");
long integer(const std::string& name, long init = 0, const std::string& label = "");
long integer(const std::string& name, long min, long max, long init = 0, const std::string& label = "");
float floating(const std::string& name, float init = 0, const std::string& label = "");
float floating(const std::string& name, long min, long max, float init = 0, const std::string& label = "");
bool checkbox(const std::string& name, bool init = false, const std::string& label = "");

void markExtra();                           // following settings belong to /wifi/extras
void markEndpoint(const std::string& name); // following settings belong to /wifi/<name>

// Raw file access. spurt("") removes the file.
std::string slurp(const std::string& fn);
bool spurt(const std::string& fn, const std::string& content);
bool exists(const std::string& fn);
bool remove(const std::string& fn);

// /wifi[/<endpoint>] GET+POST, /wifi/options/<name>, /wifi/scan
void registerHttp(httpd_handle_t server);
}  // namespace Settings

using Settings::spurt;
