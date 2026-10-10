#pragma once
// Persistent settings: one SPIFFS file per key ("/name"), the same layout the Arduino
// firmware used, so a node OTA'd to this build keeps its configuration.
// Registration order defines the /wifi, /wifi/extras and /wifi/hardware endpoints the UI reads.
#include <string>
#include <vector>

#include "ArduinoJson.h"
#include "SettingsTemplate.h"
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

// A repeated block of settings named <prefix>_<n>_<field>, n = 1..max, on the current endpoint
// (e.g. relay_1_pin .. relay_4_button). Register only the slots in use (n <= <prefix>_count)
// the normal way; the rest cost no heap but still round-trip: GET reports their saved values and
// POST stores whatever the form sends for them, so raising a count in the UI and filling in the
// new slot is one save, and lowering it keeps the hidden slots' values.
void group(const std::string& prefix, int max, const std::vector<std::string>& fields);

void markExtra();                           // following settings belong to /wifi/extras
void markEndpoint(const std::string& name); // following settings belong to /wifi/<name>
void markState();                           // last setting is runtime state, not configuration:
                                            // left out of export and template import
void markBoard();                           // last setting describes the board (e.g. Ethernet
                                            // type): exported in templates like /wifi/hardware

// Board templates (#2529) may set any configuration setting on any endpoint.
// Typed lookup by name; nullptr if unknown, runtime state, or a password (never in templates).
const SettingSpec* spec(const std::string& key);
// The board's settings that differ from their defaults, typed (numbers, bools, strings): every
// /wifi/hardware setting plus those marked markBoard(). Network identity (room, WiFi, MQTT) stays
// out so exported templates are safe to share. Param::put is the typed writer #2493 can reuse.
void serializeBoard(JsonObject out);
// Partial apply: only the given keys are touched, unlike the /wifi/<endpoint> form POST which
// resets every key missing from the body. Values must already be validated (parseTemplate).
// Appends {key, label, from, to} for each value that differs (as far as diff has room) and counts
// them in changed; dryRun stops there. False on a flash write error.
bool apply(const std::vector<SettingChange>& changes, bool dryRun, JsonArray diff, size_t& changed);

// Raw file access. spurt("") removes the file.
std::string slurp(const std::string& fn);
bool spurt(const std::string& fn, const std::string& content);
bool exists(const std::string& fn);
bool remove(const std::string& fn);

// /wifi[/<endpoint>] GET+POST, /wifi/options/<name>, /wifi/scan
void registerHttp(httpd_handle_t server);
}  // namespace Settings

using Settings::spurt;
