#pragma once
// Board templates (#2529): a shareable JSON document holding the settings that make a board work
// (pins, LED types, Ethernet type, ...).
//
//   {"name": "Athom Smart Plug V3", "chip": "esp32c3", "settings": {"led_1_pin": 6, ...}}
//
// Setting names are unique across endpoints, so "settings" is flat and may name any configuration
// setting; a full backup (#2493) can use the same shape. chip is the lowercase
// chip name ("esp32", "esp32c3", "esp32c6", "esp32s3") and must match the node exactly.
//
// Parsing and validation only: no flash, no HTTP, so it runs in the host tests. Settings.cpp owns
// the registry (which keys exist, their types and ranges) and applies the result; a template only
// touches the keys it names. Any registered setting is accepted (passwords and runtime state
// excepted), so new settings work in templates without changes here.
#include <cctype>
#include <climits>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "ArduinoJson.h"

enum class SettingType { Dropdown, String, Password, Int, Float, Bool, Json };

struct SettingSpec {
    SettingType type;
    long min = LONG_MIN, max = LONG_MAX;
    size_t options = 0;  // dropdown option count; the stored value is the index
};

struct SettingChange {
    std::string key, value;  // value in the form Settings stores it
};

// JSON value -> stored string for a setting of this type. False with err set when the value has
// the wrong type or is out of range.
inline bool settingFromJson(const SettingSpec& s, JsonVariantConst v, std::string& out, std::string& err) {
    char buf[32];
    switch (s.type) {
        case SettingType::Bool:
            if (v.is<bool>()) {
                out = v.as<bool>() ? "1" : "0";
                return true;
            }
            if (v.is<long>() && (v.as<long>() == 0 || v.as<long>() == 1)) {
                out = v.as<long>() ? "1" : "0";
                return true;
            }
            err = "expected true or false";
            return false;
        case SettingType::Int:
        case SettingType::Dropdown: {
            if (!v.is<long>()) {
                err = "expected an integer";
                return false;
            }
            long n = v.as<long>();
            long lo = s.type == SettingType::Dropdown ? 0 : s.min;
            long hi = s.type == SettingType::Dropdown ? (long)s.options - 1 : s.max;
            if (n < lo || n > hi) {
                snprintf(buf, sizeof(buf), "%ld..%ld", lo, hi);
                err = std::string("out of range (") + buf + ")";
                return false;
            }
            snprintf(buf, sizeof(buf), "%ld", n);
            out = buf;
            return true;
        }
        case SettingType::Float: {
            if (!v.is<double>() || v.is<bool>()) {
                err = "expected a number";
                return false;
            }
            double d = v.as<double>();
            if (d < (double)s.min || d > (double)s.max) {
                snprintf(buf, sizeof(buf), "%ld..%ld", s.min, s.max);
                err = std::string("out of range (") + buf + ")";
                return false;
            }
            snprintf(buf, sizeof(buf), "%g", d);
            out = buf;
            return true;
        }
        case SettingType::Json:  // a list of objects (inputs, outputs), stored as its JSON text
            if (!v.is<JsonArrayConst>()) {
                err = "expected an array";
                return false;
            }
            out.clear();
            serializeJson(v, out);
            return true;
        case SettingType::String:
        case SettingType::Password:
            if (!v.is<const char*>()) {
                err = "expected a string";
                return false;
            }
            out = v.as<const char*>();
            return true;
    }
    err = "unsupported type";
    return false;
}


// Validates a whole template before anything is written: every key must be known to lookup
// (const SettingSpec*(const char* key), nullptr = not importable) and every value valid, or
// nothing is applied. target is the running firmware's IDF target, section the key holding the settings.
template <class Lookup>
bool parseTemplate(JsonObjectConst root, const char* target, const char* section, Lookup lookup, std::vector<SettingChange>& out, std::string& err) {
    out.clear();
    if (root.isNull()) {
        err = "template must be a JSON object";
        return false;
    }
    JsonVariantConst c = root["chip"];
    if (!c.is<const char*>()) {
        err = "missing \"chip\"";
        return false;
    }
    std::string chip = c.as<const char*>();
    if (chip != target) {
        std::string squashed;  // "ESP32-C6" -> "esp32c6", to tell a misspelling from a wrong chip
        for (unsigned char ch : chip)
            if (isalnum(ch)) squashed += (char)tolower(ch);
        if (squashed == target)
            err = std::string("write \"chip\" as \"") + target + "\", not \"" + chip + "\"";
        else
            err = "template is for " + chip + ", this node is " + target;
        return false;
    }
    JsonObjectConst settings = root[section];
    if (settings.isNull()) {
        err = std::string("missing \"") + section + "\" object";
        return false;
    }
    if (settings.size() == 0) {
        err = std::string("\"") + section + "\" is empty";
        return false;
    }
    for (JsonPairConst kv : settings) {
        const char* key = kv.key().c_str();
        const SettingSpec* spec = lookup(key);
        if (!spec) {
            err = std::string("unknown setting \"") + key + "\"";
            out.clear();
            return false;
        }
        std::string value, why;
        if (!settingFromJson(*spec, kv.value(), value, why)) {
            err = std::string(key) + ": " + why;
            out.clear();
            return false;
        }
        out.push_back({key, value});
    }
    return true;
}
