// Board template parsing (#2529): a template may only name registered settings, values must
// match the setting's type and range, a chip given must match (and is required once a template
// sets a pin), every pin must be usable on the chip, and one bad key rejects the lot.
#include <map>
#include <string>
#include <vector>

#include <unity.h>

#include "../../main/SettingsTemplate.h"

namespace {

// A slice of the settings registry: /wifi/hardware plus Ethernet type from /wifi.
std::map<std::string, SettingSpec> registry() {
    std::map<std::string, SettingSpec> r;
    r["led_1_pin"] = {SettingType::Int, -1, 48, 0, PinUse::Output};
    r["batt_pin"] = {SettingType::Int, -1, 48, 0, PinUse::Input};
    r["led_1_type"] = {SettingType::Dropdown, LONG_MIN, LONG_MAX, 6};
    r["button_1_timeout"] = {SettingType::Float, 0, 300, 0};
    r["I2CDebug"] = {SettingType::Bool, LONG_MIN, LONG_MAX, 0};
    r["AHTX0_I2c"] = {SettingType::String, LONG_MIN, LONG_MAX, 0};
    r["eth"] = {SettingType::Dropdown, LONG_MIN, LONG_MAX, 15};
    r["outputs"] = {SettingType::Json, LONG_MIN, LONG_MAX, 0, PinUse::None, {{"pin", PinUse::Output}}};
    r["power"] = {SettingType::Json, LONG_MIN, LONG_MAX, 0, PinUse::None, {{"rx", PinUse::Input}, {"sel", PinUse::Output}}};
    r["BMP280_I2c"] = {SettingType::String, LONG_MIN, LONG_MAX, 0};
    r["SHT_I2c_Bus"] = {SettingType::Int, 1, 2, 0};
    return r;
}

struct Result {
    bool ok;
    std::string err;
    std::vector<SettingChange> changes;
};

// ESP32 as ESP-IDF sees it: GPIO 24 and 28-31 missing, 6-11 on the flash, 34-39 input-only.
PinRules esp32Pins() {
    PinRules p;
    p.exists = ((1ULL << 40) - 1) & ~((1ULL << 24) | (0xFULL << 28));
    p.reserved = 0x3FULL << 6;
    p.output = p.exists & ~(0x3FULL << 34);
    return p;
}

Result parse(const char* json, const char* chip = "esp32c3", const PinRules& pins = PinRules()) {
    static std::map<std::string, SettingSpec> reg = registry();
    DynamicJsonDocument doc(2048);
    TEST_ASSERT_FALSE(deserializeJson(doc, json));
    Result r;
    auto lookup = [](const char* key) -> const SettingSpec* {
        auto it = reg.find(key);
        return it == reg.end() ? nullptr : &it->second;
    };
    r.ok = parseTemplate(doc.as<JsonObjectConst>(), chip, "settings", lookup, pins, r.changes, r.err);
    return r;
}

std::string value(const Result& r, const char* key) {
    for (auto& c : r.changes)
        if (c.key == key) return c.value;
    return "<absent>";
}

void test_valid_template_yields_only_its_keys(void) {
    Result r = parse(R"({"name":"Plug","chip":"esp32c3","settings":{"led_1_pin":6,"led_1_type":1}})");
    TEST_ASSERT_TRUE_MESSAGE(r.ok, r.err.c_str());
    TEST_ASSERT_EQUAL_UINT(2, r.changes.size());
    TEST_ASSERT_EQUAL_STRING("6", value(r, "led_1_pin").c_str());
    TEST_ASSERT_EQUAL_STRING("1", value(r, "led_1_type").c_str());
    TEST_ASSERT_EQUAL_STRING("<absent>", value(r, "I2CDebug").c_str());
}

void test_types_are_stored_canonically(void) {
    Result r = parse(R"({"chip":"esp32c3","settings":{"button_1_timeout":0.5,"I2CDebug":true,"AHTX0_I2c":"0x38"}})");
    TEST_ASSERT_TRUE_MESSAGE(r.ok, r.err.c_str());
    TEST_ASSERT_EQUAL_STRING("0.5", value(r, "button_1_timeout").c_str());
    TEST_ASSERT_EQUAL_STRING("1", value(r, "I2CDebug").c_str());
    TEST_ASSERT_EQUAL_STRING("0x38", value(r, "AHTX0_I2c").c_str());
}

void test_integer_accepted_for_float_and_bool(void) {
    Result r = parse(R"({"chip":"esp32c3","settings":{"button_1_timeout":2,"I2CDebug":0}})");
    TEST_ASSERT_TRUE_MESSAGE(r.ok, r.err.c_str());
    TEST_ASSERT_EQUAL_STRING("2", value(r, "button_1_timeout").c_str());
    TEST_ASSERT_EQUAL_STRING("0", value(r, "I2CDebug").c_str());
}

void test_ethernet_board_template(void) {
    // Ethernet boards are set by the network-page "eth" dropdown, not a hardware setting.
    Result r = parse(R"({"name":"WT32-ETH01","chip":"esp32","settings":{"eth":1,"led_1_pin":-1}})", "esp32");
    TEST_ASSERT_TRUE_MESSAGE(r.ok, r.err.c_str());
    TEST_ASSERT_EQUAL_STRING("1", value(r, "eth").c_str());
    TEST_ASSERT_TRUE(parse(R"({"chip":"esp32s3","settings":{"eth":14}})", "esp32s3").ok);
    TEST_ASSERT_FALSE(parse(R"({"chip":"esp32","settings":{"eth":15}})", "esp32").ok);
}

void test_chip_mismatch_rejected(void) {
    Result r = parse(R"({"chip":"esp32","settings":{"led_1_pin":6}})");
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL_STRING("template is for esp32, this node is esp32c3", r.err.c_str());
}

void test_chip_is_exact_lowercase_name(void) {
    // "esp32c3" only: no display spellings, no firmware flavors.
    Result r = parse(R"({"chip":"ESP32-C3","settings":{"led_1_pin":6}})");
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL_STRING("write \"chip\" as \"esp32c3\", not \"ESP32-C3\"", r.err.c_str());
    TEST_ASSERT_FALSE(parse(R"({"chip":"ESP32-C6","settings":{"led_1_pin":6}})").ok);
    TEST_ASSERT_FALSE(parse(R"({"chip":"esp32c3-cdc","settings":{"led_1_pin":6}})").ok);
    TEST_ASSERT_TRUE(parse(R"({"chip":"esp32","settings":{"led_1_pin":6}})", "esp32").ok);
}

void test_settings_must_be_under_settings_key(void) {
    // Keys at the top level or under another key (e.g. "hardware") are not read.
    Result r = parse(R"({"chip":"esp32c3","hardware":{"led_1_pin":6}})");
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL_STRING("missing \"settings\" object", r.err.c_str());
}

void test_missing_chip_rejected_when_template_sets_a_pin(void) {
    Result r = parse(R"({"settings":{"led_1_pin":6}})");
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL_STRING("missing \"chip\" (required when a template sets pins)", r.err.c_str());
    TEST_ASSERT_EQUAL_UINT(0, r.changes.size());
    // A pin inside a list counts too.
    TEST_ASSERT_FALSE(parse(R"({"settings":{"outputs":[{"pin":5}]}})").ok);
    TEST_ASSERT_FALSE(parse(R"({"settings":{"power":{"model":"cse7766","rx":3}}})").ok);
}

void test_chip_optional_without_pins(void) {
    // An add-on sensor's address and bus fit any node.
    Result r = parse(R"({"name":"M5Stack ENV IV","settings":{"BMP280_I2c":"0x76","SHT_I2c_Bus":1}})");
    TEST_ASSERT_TRUE_MESSAGE(r.ok, r.err.c_str());
    TEST_ASSERT_EQUAL_STRING("0x76", value(r, "BMP280_I2c").c_str());
    // Disabling a pin, or a list with no pins set, is the same on every chip.
    TEST_ASSERT_TRUE(parse(R"({"settings":{"led_1_pin":-1}})").ok);
    TEST_ASSERT_TRUE(parse(R"({"settings":{"outputs":[]}})").ok);
    TEST_ASSERT_TRUE(parse(R"({"settings":{"outputs":[{"name":"Relay","pin":-1}]}})").ok);
    // A chip given must still be right, and a string.
    TEST_ASSERT_FALSE(parse(R"({"chip":"esp32","settings":{"SHT_I2c_Bus":1}})").ok);
    r = parse(R"({"chip":5,"settings":{"SHT_I2c_Bus":1}})");
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL_STRING("\"chip\" must be a string", r.err.c_str());
}

void test_pins_checked_against_the_chip(void) {
    PinRules p = esp32Pins();
    TEST_ASSERT_TRUE(parse(R"({"chip":"esp32","settings":{"led_1_pin":2}})", "esp32", p).ok);
    Result r = parse(R"({"chip":"esp32","settings":{"led_1_pin":6}})", "esp32", p);
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL_STRING("led_1_pin: GPIO 6 is used by the flash or PSRAM", r.err.c_str());
    r = parse(R"({"chip":"esp32","settings":{"led_1_pin":24}})", "esp32", p);
    TEST_ASSERT_EQUAL_STRING("led_1_pin: GPIO 24 doesn't exist on this chip", r.err.c_str());
    r = parse(R"({"chip":"esp32","settings":{"led_1_pin":45}})", "esp32", p);
    TEST_ASSERT_EQUAL_STRING("led_1_pin: GPIO 45 doesn't exist on this chip", r.err.c_str());
    // Input-only pins can read but not drive.
    r = parse(R"({"chip":"esp32","settings":{"led_1_pin":34}})", "esp32", p);
    TEST_ASSERT_EQUAL_STRING("led_1_pin: GPIO 34 is input-only", r.err.c_str());
    TEST_ASSERT_TRUE(parse(R"({"chip":"esp32","settings":{"batt_pin":34}})", "esp32", p).ok);
    TEST_ASSERT_TRUE(parse(R"({"chip":"esp32","settings":{"led_1_pin":-1}})", "esp32", p).ok);
}

void test_pins_in_lists_and_objects_checked(void) {
    PinRules p = esp32Pins();
    Result r = parse(R"({"chip":"esp32","settings":{"outputs":[{"pin":5},{"pin":36}]}})", "esp32", p);
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL_STRING("outputs[1].pin: GPIO 36 is input-only", r.err.c_str());
    TEST_ASSERT_EQUAL_UINT(0, r.changes.size());
    r = parse(R"({"chip":"esp32","settings":{"power":{"model":"bl0937","rx":-1,"sel":39}}})", "esp32", p);
    TEST_ASSERT_EQUAL_STRING("power.sel: GPIO 39 is input-only", r.err.c_str());
    TEST_ASSERT_TRUE(parse(R"({"chip":"esp32","settings":{"power":{"model":"cse7766","rx":39}}})", "esp32", p).ok);
    // Members the firmware doesn't read as pins are left alone.
    TEST_ASSERT_TRUE(parse(R"({"chip":"esp32","settings":{"outputs":[{"pin":5,"input":9}]}})", "esp32", p).ok);
}

void test_check_pins_on_stored_values(void) {
    // The /wifi form save runs the same check on the stored text.
    PinRules p = esp32Pins();
    SettingSpec led = {SettingType::Int, -1, 48, 0, PinUse::Output};
    bool setsPin = false;
    TEST_ASSERT_EQUAL_STRING("", checkPins("led_1_pin", led, "", p, setsPin).c_str());
    TEST_ASSERT_FALSE(setsPin);
    TEST_ASSERT_EQUAL_STRING("led_1_pin: GPIO 11 is used by the flash or PSRAM", checkPins("led_1_pin", led, "11", p, setsPin).c_str());
    TEST_ASSERT_TRUE(setsPin);
    SettingSpec leds = {SettingType::Json, LONG_MIN, LONG_MAX, 0, PinUse::None, {{"pin", PinUse::Output}}};
    TEST_ASSERT_EQUAL_STRING("leds[0].pin: GPIO 7 is used by the flash or PSRAM", checkPins("leds", leds, R"([{"type":"grb","pin":7}])", p, setsPin).c_str());
    TEST_ASSERT_EQUAL_STRING("", checkPins("leds", leds, "not json", p, setsPin).c_str());
}

void test_missing_or_empty_settings_rejected(void) {
    TEST_ASSERT_FALSE(parse(R"({"chip":"esp32c3"})").ok);
    TEST_ASSERT_FALSE(parse(R"({"chip":"esp32c3","settings":[]})").ok);
    Result r = parse(R"({"chip":"esp32c3","settings":{}})");
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL_STRING("\"settings\" is empty", r.err.c_str());
}

void test_not_an_object_rejected(void) {
    TEST_ASSERT_FALSE(parse("[1,2]").ok);
}

void test_unknown_key_rejects_everything(void) {
    // Not importable (a password: the lookup refuses it) or simply unknown: nothing is applied.
    Result r = parse(R"({"chip":"esp32c3","settings":{"led_1_pin":6,"mqtt_pass":"x"}})");
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL_STRING("unknown setting \"mqtt_pass\"", r.err.c_str());
    TEST_ASSERT_EQUAL_UINT(0, r.changes.size());
}

void test_int_range_enforced(void) {
    Result r = parse(R"({"chip":"esp32c3","settings":{"led_1_pin":49}})");
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL_STRING("led_1_pin: out of range (-1..48)", r.err.c_str());
    TEST_ASSERT_TRUE(parse(R"({"chip":"esp32c3","settings":{"led_1_pin":-1}})").ok);
    TEST_ASSERT_FALSE(parse(R"({"chip":"esp32c3","settings":{"led_1_pin":-2}})").ok);
}

void test_dropdown_index_range_enforced(void) {
    TEST_ASSERT_TRUE(parse(R"({"chip":"esp32c3","settings":{"led_1_type":5}})").ok);
    Result r = parse(R"({"chip":"esp32c3","settings":{"led_1_type":6}})");
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL_STRING("led_1_type: out of range (0..5)", r.err.c_str());
    TEST_ASSERT_FALSE(parse(R"({"chip":"esp32c3","settings":{"led_1_type":-1}})").ok);
}

void test_float_range_enforced(void) {
    TEST_ASSERT_FALSE(parse(R"({"chip":"esp32c3","settings":{"button_1_timeout":300.5}})").ok);
    TEST_ASSERT_FALSE(parse(R"({"chip":"esp32c3","settings":{"button_1_timeout":-0.1}})").ok);
}

void test_wrong_types_rejected(void) {
    Result r = parse(R"({"chip":"esp32c3","settings":{"led_1_pin":"6"}})");
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL_STRING("led_1_pin: expected an integer", r.err.c_str());
    TEST_ASSERT_FALSE(parse(R"({"chip":"esp32c3","settings":{"led_1_pin":6.5}})").ok);
    TEST_ASSERT_FALSE(parse(R"({"chip":"esp32c3","settings":{"led_1_pin":true}})").ok);
    TEST_ASSERT_FALSE(parse(R"({"chip":"esp32c3","settings":{"led_1_pin":null}})").ok);
    TEST_ASSERT_FALSE(parse(R"({"chip":"esp32c3","settings":{"I2CDebug":2}})").ok);
    TEST_ASSERT_FALSE(parse(R"({"chip":"esp32c3","settings":{"I2CDebug":"true"}})").ok);
    TEST_ASSERT_FALSE(parse(R"({"chip":"esp32c3","settings":{"button_1_timeout":true}})").ok);
    TEST_ASSERT_FALSE(parse(R"({"chip":"esp32c3","settings":{"AHTX0_I2c":56}})").ok);
}

void test_json_setting_takes_an_array_or_object_as_text(void) {
    Result r = parse(R"({"chip":"esp32c3","settings":{"outputs":[{"name":"Relay","pin":5,"type":"output"}]}})");
    TEST_ASSERT_TRUE_MESSAGE(r.ok, r.err.c_str());
    TEST_ASSERT_EQUAL_STRING(R"([{"name":"Relay","pin":5,"type":"output"}])", value(r, "outputs").c_str());
    TEST_ASSERT_TRUE(parse(R"({"chip":"esp32c3","settings":{"outputs":[]}})").ok);
    r = parse(R"({"chip":"esp32c3","settings":{"outputs":"[{}]"}})");
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL_STRING("outputs: expected an array or object", r.err.c_str());
    TEST_ASSERT_FALSE(parse(R"({"chip":"esp32c3","settings":{"outputs":5}})").ok);
    r = parse(R"({"chip":"esp32c3","settings":{"outputs":{"current_resistor":0.001}}})");
    TEST_ASSERT_TRUE_MESSAGE(r.ok, r.err.c_str());
    TEST_ASSERT_EQUAL_STRING(R"({"current_resistor":0.001})", value(r, "outputs").c_str());
}

}  // namespace

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_valid_template_yields_only_its_keys);
    RUN_TEST(test_types_are_stored_canonically);
    RUN_TEST(test_integer_accepted_for_float_and_bool);
    RUN_TEST(test_ethernet_board_template);
    RUN_TEST(test_chip_mismatch_rejected);
    RUN_TEST(test_chip_is_exact_lowercase_name);
    RUN_TEST(test_settings_must_be_under_settings_key);
    RUN_TEST(test_missing_chip_rejected_when_template_sets_a_pin);
    RUN_TEST(test_chip_optional_without_pins);
    RUN_TEST(test_pins_checked_against_the_chip);
    RUN_TEST(test_pins_in_lists_and_objects_checked);
    RUN_TEST(test_check_pins_on_stored_values);
    RUN_TEST(test_missing_or_empty_settings_rejected);
    RUN_TEST(test_not_an_object_rejected);
    RUN_TEST(test_unknown_key_rejects_everything);
    RUN_TEST(test_int_range_enforced);
    RUN_TEST(test_dropdown_index_range_enforced);
    RUN_TEST(test_float_range_enforced);
    RUN_TEST(test_wrong_types_rejected);
    RUN_TEST(test_json_setting_takes_an_array_or_object_as_text);
    return UNITY_END();
}
