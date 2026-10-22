#pragma once
// Board templates (#2529): a shareable JSON document holding the settings that make a board work
// (pins, LED types, Ethernet type, ...).
//
//   {"name": "Athom Smart Plug V3", "chip": "esp32c3", "settings": {"led_1_pin": 6, ...}}
//
// Setting names are unique across endpoints, so "settings" is flat and may name any configuration
// setting; a full backup (#2493) can use the same shape. chip is the lowercase
// chip name ("esp32", "esp32c3", "esp32c6", "esp32s3"); when present it must match the node
// exactly. It is required only when the template sets a pin, since pin numbers mean different
// things on each chip. Without pins (e.g. an add-on sensor's address and bus) a template fits any
// node. Every pin is also checked against the running chip: it must exist, not belong to the
// flash or PSRAM, and be output-capable where the setting drives it.
//
// Parsing and validation only: no flash, no HTTP, so it runs in the host tests. Settings.cpp owns
// the registry (which keys exist, their types and ranges) and applies the result; a template only
// touches the keys it names. Any registered setting is accepted (passwords and runtime state
// excepted), so new settings work in templates without changes here.
#include <cctype>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "ArduinoJson.h"

enum class SettingType { Dropdown, String, Password, Int, Float, Bool, Json };

// How a setting uses a GPIO. Output covers anything the chip drives, including two-way buses
// (I2C, 1-Wire, DHT), so an input-only pin can't take it.
// Analog is an input read through ADC1 (ADC2 is shared with WiFi), so it needs an ADC1 channel.
enum class PinUse : uint8_t { None, Input, Output, Analog };
// A Json setting's members holding GPIOs, e.g. {{"pin", PinUse::Output}}.
using PinFields = std::vector<std::pair<std::string, PinUse>>;

struct SettingSpec {
    SettingType type;
    long min = LONG_MIN, max = LONG_MAX;
    size_t options = 0;  // dropdown option count; the stored value is the index
    PinUse pin = PinUse::None;  // an Int setting holding a GPIO (-1 = disabled)
    // A Json setting's members holding GPIOs: in the object itself or in each array element.
    // A pointer, not a copy: only a few settings have any.
    const PinFields* pinFields = nullptr;
};

struct SettingChange {
    std::string key, value;  // value in the form Settings stores it
};

// The running chip's GPIOs as bit masks (bit n = GPIO n). Defaults allow every pin.
struct PinRules {
    uint64_t exists = ~0ULL;    // GPIOs the chip has
    uint64_t reserved = 0;      // taken by the flash or PSRAM
    uint64_t output = ~0ULL;    // GPIOs that can drive a level
    uint64_t adc1 = ~0ULL;      // GPIOs with an ADC1 channel
};

// Why `pin` can't serve `use` on this chip, or "" when it can. Negative means disabled: fine.
inline std::string pinProblem(long pin, PinUse use, const PinRules& r) {
    if (pin == -1 || use == PinUse::None) return "";
    std::string gpio = "GPIO " + std::to_string(pin);
    // Below -1 too: drivers keep pins in int8_t, so -250 would wrap to a real GPIO (6).
    if (pin < 0 || pin > 63 || !((r.exists >> pin) & 1)) return gpio + " doesn't exist on this chip";
    if ((r.reserved >> pin) & 1) return gpio + " is used by the flash or PSRAM";
    if (use == PinUse::Output && !((r.output >> pin) & 1)) return gpio + " is input-only";
    if (use == PinUse::Analog && !((r.adc1 >> pin) & 1)) return gpio + " has no ADC1 channel";
    return "";
}

// Checks every GPIO a setting's value names; value is in the form Settings stores it (number
// text, or JSON text for a list or object). Returns "<where>: <problem>" for the first unusable
// pin, or "". setsPin is set when any pin is enabled (>= 0), usable or not.
inline std::string checkPins(const std::string& key, const SettingSpec& s, const std::string& value, const PinRules& r, bool& setsPin) {
    if (s.pin != PinUse::None) {
        if (value.empty()) return "";
        char* end = nullptr;
        long pin = strtol(value.c_str(), &end, 10);
        if (*end) return key + ": \"" + value + "\" is not a GPIO number";
        if (pin >= 0) setsPin = true;
        std::string why = pinProblem(pin, s.pin, r);
        return why.empty() ? "" : key + ": " + why;
    }
    if (!s.pinFields || s.pinFields->empty() || value.empty()) return "";
    DynamicJsonDocument doc(value.size() * 2 + 128);
    auto error = deserializeJson(doc, value);
    if (error == DeserializationError::NoMemory) return key + ": insufficient memory to validate GPIOs";
    if (error) return "";  // Malformed form JSON is ignored by Param::set().
    auto check = [&](JsonObjectConst o, const std::string& where) -> std::string {
        for (auto& f : *s.pinFields) {
            JsonVariantConst v = o[f.first];
            if (!v.is<long>()) continue;  // absent or not a number: the firmware treats it as unset
            long pin = v.as<long>();
            if (pin >= 0) setsPin = true;
            std::string why = pinProblem(pin, f.second, r);
            if (!why.empty()) return where + "." + f.first + ": " + why;
        }
        return "";
    };
    if (doc.is<JsonArrayConst>()) {
        size_t i = 0;
        for (JsonVariantConst e : doc.as<JsonArrayConst>()) {
            if (e.is<JsonObjectConst>()) {
                std::string err = check(e.as<JsonObjectConst>(), key + "[" + std::to_string(i) + "]");
                if (!err.empty()) return err;
            }
            i++;
        }
        return "";
    }
    if (doc.is<JsonObjectConst>()) return check(doc.as<JsonObjectConst>(), key);
    return "";
}

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
        case SettingType::Json:  // a list (leds, inputs, outputs) or object (power), stored as JSON text
            if (!v.is<JsonArrayConst>() && !v.is<JsonObjectConst>()) {
                err = "expected an array or object";
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


// rx_adj_rssi was subtracted; v5's rx_offset_rssi is added. A template written with the old name
// is converted the way the MQTT alias is: rx_offset_rssi = -rx_adj_rssi. Run on the settings
// object before parseTemplate. With both keys present nothing changes, so the parser rejects the
// old one rather than one being dropped silently.
inline void upgradeTemplateKeys(JsonObject settings) {
    if (settings.isNull() || settings.containsKey("rx_offset_rssi")) return;
    JsonVariant adj = settings["rx_adj_rssi"];
    if (adj.isNull() || !adj.is<long>()) return;
    long offset = -adj.as<long>();
    settings.remove("rx_adj_rssi");
    settings["rx_offset_rssi"] = offset;
}

// Validates a whole template before anything is written: every key must be known to lookup
// (const SettingSpec*(const char* key), nullptr = not importable) and every value valid, every
// pin usable under pins, or nothing is applied. target is the running firmware's IDF target,
// section the key holding the settings.
template <class Lookup>
bool parseTemplate(JsonObjectConst root, const char* target, const char* section, Lookup lookup, const PinRules& pins, std::vector<SettingChange>& out, std::string& err) {
    out.clear();
    if (root.isNull()) {
        err = "template must be a JSON object";
        return false;
    }
    JsonVariantConst c = root["chip"];
    // Present means containsKey(), not "reads as non-null": ArduinoJson returns null for both a
    // missing key and an explicit null, so a null chip is caught here rather than after the lookup.
    if (root.containsKey("chip") && !c.is<const char*>()) {
        err = "\"chip\" must be a string";
        return false;
    }
    std::string chip = c.isNull() ? "" : c.as<const char*>();
    if (!c.isNull() && chip != target) {
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
    bool setsPin = false;
    for (JsonPairConst kv : settings) {
        const char* key = kv.key().c_str();
        auto spec = lookup(key);  // a pointer or optional: anything that tests false when unknown
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
        why = checkPins(key, *spec, value, pins, setsPin);
        if (!why.empty()) {
            err = why;
            out.clear();
            return false;
        }
        out.push_back({key, value});
    }
    if (setsPin && chip.empty()) {
        err = "missing \"chip\" (required when a template sets pins)";
        out.clear();
        return false;
    }
    return true;
}
