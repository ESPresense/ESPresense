// Board template parsing (#2529): a template may only name registered settings, values must
// match the setting's type and range, the chip must match, and one bad key rejects the lot.
#include <map>
#include <string>
#include <vector>

#include <unity.h>

#include "../../main/SettingsTemplate.h"

namespace {

// A slice of the /wifi/hardware registry.
std::map<std::string, SettingSpec> registry() {
    std::map<std::string, SettingSpec> r;
    r["led_1_pin"] = {SettingType::Int, -1, 48, 0};
    r["led_1_type"] = {SettingType::Dropdown, LONG_MIN, LONG_MAX, 6};
    r["button_1_timeout"] = {SettingType::Float, 0, 300, 0};
    r["I2CDebug"] = {SettingType::Bool, LONG_MIN, LONG_MAX, 0};
    r["AHTX0_I2c"] = {SettingType::String, LONG_MIN, LONG_MAX, 0};
    return r;
}

struct Result {
    bool ok;
    std::string err;
    std::vector<SettingChange> changes;
};

Result parse(const char* json, const char* chip = "esp32c3") {
    static std::map<std::string, SettingSpec> reg = registry();
    DynamicJsonDocument doc(2048);
    TEST_ASSERT_FALSE(deserializeJson(doc, json));
    Result r;
    auto lookup = [](const char* key) -> const SettingSpec* {
        auto it = reg.find(key);
        return it == reg.end() ? nullptr : &it->second;
    };
    r.ok = parseTemplate(doc.as<JsonObjectConst>(), chip, "settings", lookup, r.changes, r.err);
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

void test_missing_chip_rejected(void) {
    Result r = parse(R"({"settings":{"led_1_pin":6}})");
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL_STRING("missing \"chip\"", r.err.c_str());
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
    // Outside the endpoint (mqtt_pass lives on /wifi) or simply unknown: nothing is applied.
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

}  // namespace

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_valid_template_yields_only_its_keys);
    RUN_TEST(test_types_are_stored_canonically);
    RUN_TEST(test_integer_accepted_for_float_and_bool);
    RUN_TEST(test_chip_mismatch_rejected);
    RUN_TEST(test_chip_is_exact_lowercase_name);
    RUN_TEST(test_settings_must_be_under_settings_key);
    RUN_TEST(test_missing_chip_rejected);
    RUN_TEST(test_missing_or_empty_settings_rejected);
    RUN_TEST(test_not_an_object_rejected);
    RUN_TEST(test_unknown_key_rejects_everything);
    RUN_TEST(test_int_range_enforced);
    RUN_TEST(test_dropdown_index_range_enforced);
    RUN_TEST(test_float_range_enforced);
    RUN_TEST(test_wrong_types_rejected);
    return UNITY_END();
}
