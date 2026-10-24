#pragma once
// First boot on v5.0.0: converts settings that v5 stores differently, once (the v5_migrated file).
// Pure, so host tests can run it on a map. get(key) returns the stored text or "" when absent;
// put(key, value) stores it and returns false if the write failed. Keys are setting names (Settings
// stores each as the file "/<key>"). If any write fails, v5_migrated isn't written, so the whole
// conversion runs again on the next boot (each step skips what's already stored).
//
// - The receiver adjustment: v4/early-v5 rx_adj_rssi (subtracted) becomes rx_offset_rssi
//   (added) = -effective rx_adj_rssi, so distances don't change. "Effective" includes the old
//   default (+20 on ESP32-S3) when nothing was stored. rx_adj_rssi is left for a downgrade.
// - v4's LED settings (led_1_pin, led_1_type, ...) become the leds list.
// - v4's PIR, radar, button and switch settings become the inputs list.
// - v4 had default I2C pins and v5 has none: a v4 node with an I2C sensor on bus 1 and no bus 1
//   pins stored gets v4's default pins for its chip back.
// A node with no WiFi and no Ethernet configured is a fresh install: nothing to convert.
#include <ArduinoJson.h>

#include <cstdlib>
#include <string>
#include <vector>

struct MigrationDefaults {
    int legacyRxAdjRssi;     // the old rx_adj_rssi default for this chip
    int v4I2cSda, v4I2cScl;  // v4's default bus 1 pins for this chip, -1 if v4 had none
    int v4Led1Pin;           // v4's default led_1_pin for a generic board
};

template <class Get, class Put>
std::vector<std::string> migrateSettings(Get get, Put put, const MigrationDefaults& d) {
    std::vector<std::string> done;  // what changed, for the log
    if (!get("v5_migrated").empty()) return done;
    auto num = [&](const std::string& key, long def) {
        std::string v = get(key);
        return v.empty() ? def : strtol(v.c_str(), nullptr, 10);
    };
    auto fnum = [&](const std::string& key, float def) {
        std::string v = get(key);
        return v.empty() ? def : strtof(v.c_str(), nullptr);
    };
    bool ok = true;
    auto store = [&](const std::string& key, const std::string& value) {
        if (!put(key, value)) ok = false;
    };
    auto finish = [&]() {
        if (ok && put("v5_migrated", "1")) return done;
        done.push_back("a write failed; converting again next boot");
        return done;
    };
    if (get("wifi-ssid").empty() && num("eth", 0) <= 0) return finish();

    // Receiver adjustment.
    if (get("rx_offset_rssi").empty()) {
        long offset = -num("rx_adj_rssi", d.legacyRxAdjRssi);
        if (offset != 0) {
            store("rx_offset_rssi", std::to_string(offset));
            done.push_back("rx_offset_rssi=" + std::to_string(offset));
        }
    }

    // v4 hardware settings. Any of these stored means the node was configured on v4.
    static const char* const v4Keys[] = {
        "led_1_pin", "led_1_type", "led_1_cnt", "led_1_cntrl", "led_2_pin", "led_2_type", "led_2_cnt",
        "led_2_cntrl", "led_3_pin", "led_3_type", "led_3_cnt", "led_3_cntrl", "pir_pin", "pir_type",
        "pir_timeout", "radar_pin", "radar_type", "radar_timeout", "button_1_pin", "button_1_type",
        "button_1_timeout", "button_2_pin", "button_2_type", "button_2_timeout", "switch_1_pin",
        "switch_1_type", "switch_1_timeout", "switch_2_pin", "switch_2_type", "switch_2_timeout"};
    bool v4 = false, v4Leds = false;
    for (const char* k : v4Keys)
        if (!get(k).empty()) {
            v4 = true;
            if (k[0] == 'l') v4Leds = true;
        }
    if (!v4) return finish();

    static const char* const ledTypes[] = {"pwm", "pwm_inverted", "grb", "grbw", "rgb", "rgbw"};
    static const char* const ledControls[] = {"mqtt", "status", "motion", "count"};
    static const char* const pinTypes[] = {"pullup", "pullup_inverted", "pulldown", "pulldown_inverted",
                                           "floating", "floating_inverted"};
    auto pick = [](const char* const* names, size_t n, long i, size_t def) { return names[i >= 0 && (size_t)i < n ? (size_t)i : def]; };

    // LEDs: v4's defaults were LED 1 PWM on the board's pin showing status, LEDs 2-3 off. A v4 node
    // that touched any LED setting gets the whole list, even an empty one (an LED it turned off).
    if (v4Leds && get("leds").empty()) {
        DynamicJsonDocument list(1024);
        JsonArray a = list.to<JsonArray>();
        for (int n = 1; n <= 3; n++) {
            std::string p = "led_" + std::to_string(n) + "_";
            long pin = num(p + "pin", n == 1 ? d.v4Led1Pin : -1);
            if (pin < 0) continue;
            JsonObject o = a.createNestedObject();
            o["type"] = pick(ledTypes, 6, num(p + "type", 0), 0);
            o["pin"] = pin;
            o["count"] = num(p + "cnt", 1);
            o["control"] = pick(ledControls, 4, num(p + "cntrl", n == 1 ? 1 : 0), 0);
        }
        std::string text;
        serializeJson(list, text);
        store("leds", text);
        done.push_back("leds=" + text);
    }

    // Inputs: PIR and radar are motion inputs, then the buttons and switches.
    if (get("inputs").empty()) {
        struct Old { const char *prefix, *name, *role; };
        static const Old olds[] = {{"pir", "PIR", "motion"}, {"radar", "Radar", "motion"},
                                   {"button_1", "Button 1", "button"}, {"button_2", "Button 2", "button"},
                                   {"switch_1", "Switch 1", "switch"}, {"switch_2", "Switch 2", "switch"}};
        DynamicJsonDocument list(1536);
        JsonArray a = list.to<JsonArray>();
        for (const Old& o : olds) {
            std::string p = std::string(o.prefix) + "_";
            long pin = num(p + "pin", -1);
            if (pin < 0) continue;
            JsonObject in = a.createNestedObject();
            in["name"] = o.name;
            in["role"] = o.role;
            in["pin"] = pin;
            in["type"] = pick(pinTypes, 6, num(p + "type", 0), 0);
            in["timeout"] = fnum(p + "timeout", 0.5f);
        }
        if (a.size()) {
            std::string text;
            serializeJson(list, text);
            store("inputs", text);
            done.push_back("inputs=" + text);
        }
    }

    // I2C: only when a sensor uses bus 1 and its pins were left at v4's defaults.
    static const char* const sensors[] = {"AHTX0", "BH1750", "BME280", "BMP180", "BMP280", "TSL2561", "SGP30", "SCD4x"};
    bool bus1 = false;
    for (const char* s : sensors) {
        std::string base = s;
        if (!get(base + "_I2c").empty() && num(base + "_I2c_Bus", 1) == 1) bus1 = true;
    }
    if (num("SHT_I2c_Bus", -1) == 1) bus1 = true;
    // Each pin on its own: v4 used its default for whichever pin wasn't set, and a retry after a
    // failed write fills in only the pin still missing.
    if (bus1 && d.v4I2cSda >= 0) {
        std::string filled;
        if (get("I2C_Bus_1_SDA").empty()) {
            store("I2C_Bus_1_SDA", std::to_string(d.v4I2cSda));
            filled += " SDA=" + std::to_string(d.v4I2cSda);
        }
        if (get("I2C_Bus_1_SCL").empty()) {
            store("I2C_Bus_1_SCL", std::to_string(d.v4I2cScl));
            filled += " SCL=" + std::to_string(d.v4I2cScl);
        }
        if (!filled.empty()) done.push_back("I2C bus 1" + filled);
    }
    return finish();
}

// I2C bus selection, once (the i2c_bus_migrated file). A sensor used to be on when its I2C address
// was set, with the bus a separate 1-2 number; now the bus setting is a dropdown (0 None, 1-3) and
// turns the sensor on. A sensor with an address keeps its bus (default 1); one with only a bus
// stored was off, so its bus becomes None. The M5StickC "axp192" checkbox becomes the battery
// gauge chip on bus 2. Run after migrateSettings, which still reads the old meaning.
template <class Get, class Put>
std::vector<std::string> migrateI2cBus(Get get, Put put) {
    std::vector<std::string> done;
    if (!get("i2c_bus_migrated").empty()) return done;
    bool ok = true;
    auto store = [&](const std::string& key, const std::string& value) {
        if (!put(key, value)) ok = false;
    };
    static const char* const sensors[] = {"AHTX0", "BH1750", "BME280", "BMP180", "BMP280", "TSL2561", "SGP30", "SCD4x"};
    for (const char* s : sensors) {
        std::string base = s, bus = get(base + "_I2c_Bus");
        long old = bus.empty() ? 1 : strtol(bus.c_str(), nullptr, 10);
        std::string now = !get(base + "_I2c").empty() && old >= 1 && old <= 2 ? std::to_string(old) : "0";
        if (now == "0" && bus.empty()) continue;  // never set: already None
        if (bus == now) continue;
        store(base + "_I2c_Bus", now);
        done.push_back(base + "_I2c_Bus=" + now);
    }
    if (strtol(get("axp192").c_str(), nullptr, 10) != 0 && (get("batt_gauge").empty() || get("batt_gauge_bus").empty())) {
        if (get("batt_gauge").empty()) store("batt_gauge", "0");
        if (get("batt_gauge_bus").empty()) store("batt_gauge_bus", "2");  // it was hardwired to bus 2
        done.push_back("batt_gauge=AXP192 on bus 2");
    }
    if (ok && put("i2c_bus_migrated", "1")) return done;
    done.push_back("a write failed; converting again next boot");
    return done;
}
