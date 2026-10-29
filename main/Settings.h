#pragma once
// Persistent settings: one SPIFFS file per key ("/name"), the same layout the Arduino
// firmware used, so a node OTA'd to this build keeps its configuration.
// Registration order defines the /wifi, /wifi/extras and /wifi/hardware endpoints the UI reads.
#include <optional>
#include <string>
#include <vector>

#include "ArduinoJson.h"
#include "SettingsTemplate.h"
#include "esp_http_server.h"

namespace Settings {
// `label` describes a setting for whoever reads the code; it isn't stored (the UI has its own).
void begin();  // mount SPIFFS (formats on first use); snapshot the chip's GPIOs (pins())

std::string string(const std::string& name, const std::string& init = "", const char* label = "");
std::string pstring(const std::string& name, const std::string& init = "", const char* label = "");
long dropdown(const std::string& name, const std::vector<std::string>& options, long init = 0, const char* label = "");
long integer(const std::string& name, long init = 0, const char* label = "");
long integer(const std::string& name, long min, long max, long init = 0, const char* label = "");
// A GPIO the firmware reads (input) or drives (output; also I2C, 1-Wire and DHT). -1 = disabled.
// Templates and saves are refused when the pin can't do that on this chip; /wifi/<endpoint>
// lists the usable GPIOs as pins.
long pinInput(const std::string& name, long init = -1, const char* label = "");
long pinOutput(const std::string& name, long init = -1, const char* label = "");
// A GPIO read through ADC1 (e.g. a battery voltage): refused unless the pin has an ADC1 channel.
long pinAnalog(const std::string& name, long init = -1, const char* label = "");
float floating(const std::string& name, float init = 0, const char* label = "");
float floating(const std::string& name, long min, long max, float init = 0, const char* label = "");
bool checkbox(const std::string& name, bool init = false, const char* label = "");
// JSON text (an array like leds/inputs/outputs, or an object like power); the UI and templates
// see real JSON.
std::string json(const std::string& name, const std::string& init = "[]", const char* label = "");
// As above, with members holding a GPIO (in the object, or in each list element), checked like pinInput()/pinOutput().
std::string json(const std::string& name, const std::string& init, const char* label,
                 const std::vector<std::pair<std::string, PinUse>>& pins);

void markExtra();                           // following settings belong to /wifi/extras
void markEndpoint(const std::string& name); // following settings belong to /wifi/<name>
void markBoard();                           // last setting describes the board (e.g. Ethernet
                                            // type): exported in templates like /wifi/hardware
// The running chip's usable GPIOs, captured in begin() before any driver claims a pin.
const PinRules& pins();

// Board templates (#2529) may set any configuration setting on any endpoint.
// Typed lookup by name, built on demand; empty if unknown or a password (never in templates).
std::optional<SettingSpec> spec(const std::string& key);
// The board's settings that differ from their defaults, typed (numbers, bools, strings): every
// /wifi/hardware setting plus those marked markBoard(). Network identity (room, WiFi, MQTT) stays
// out so exported templates are safe to share. Param::put is the typed writer #2493 can reuse.
void serializeBoard(JsonObject out);
// Partial apply: only the given keys are touched (as with the /wifi/<endpoint> form POST). Values must already be validated (parseTemplate).
// Appends {key, from, to} for each value that differs (as far as diff has room) and counts
// them in changed; dryRun stops there. False on a flash write error.
bool apply(const std::vector<SettingChange>& changes, bool dryRun, JsonArray diff, size_t& changed);

// Raw file access. spurt("") removes the file.
std::string slurp(const std::string& fn);
bool spurt(const std::string& fn, const std::string& content);
bool exists(const std::string& fn);
bool remove(const std::string& fn);

// /wifi[/<endpoint>] GET+POST, /wifi/scan
void registerHttp(httpd_handle_t server);
}  // namespace Settings

using Settings::spurt;
