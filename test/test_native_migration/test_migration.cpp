// First-boot conversion (SettingsMigration.h) on an in-memory store.
#include <map>
#include <string>

#include "unity.h"
#include "SettingsMigration.h"

typedef std::map<std::string, std::string> Store;
static const MigrationDefaults S3 = {20, 21, 22, 2}, ESP32 = {0, 21, 22, 2}, C6 = {0, -1, -1, 2};

static Store run(Store s, const MigrationDefaults& d = ESP32) {
    migrateSettings([&](const std::string& k) { auto i = s.find(k); return i == s.end() ? std::string() : i->second; },
                    [&](const std::string& k, const std::string& v) { s[k] = v; }, d);
    return s;
}
static std::string at(const Store& s, const char* k) { auto i = s.find(k); return i == s.end() ? "" : i->second; }
static Store configured(Store extra = {}) { extra["wifi-ssid"] = "home"; return extra; }
static Store ethOnly(Store extra = {}, const char* type = "1") { extra["eth"] = type; return extra; }

static void fresh_install_only_marks_itself() {
    Store s = run({}, S3);
    TEST_ASSERT_EQUAL_STRING("1", at(s, "v5_migrated").c_str());
    TEST_ASSERT_EQUAL(1, s.size());  // no rx_offset_rssi: a fresh S3 starts at 0
}

static void ethernet_only_node_still_converts() {
    // No wifi-ssid but eth configured (Network::connect supports Ethernet without WiFi): not a fresh install.
    Store s = run(ethOnly({{"rx_adj_rssi", "7"}}));
    TEST_ASSERT_EQUAL_STRING("-7", at(s, "rx_offset_rssi").c_str());
    TEST_ASSERT_EQUAL_STRING("1", at(s, "v5_migrated").c_str());

    // S3 Ethernet node (Waveshare W5500, eth=14) that never set rx_adj_rssi: the old implicit +20 default.
    Store s3 = run(ethOnly({}, "14"), S3);
    TEST_ASSERT_EQUAL_STRING("-20", at(s3, "rx_offset_rssi").c_str());
}

static void runs_once() {
    Store s = run(configured({{"v5_migrated", "1"}, {"rx_adj_rssi", "7"}}));
    TEST_ASSERT_EQUAL_STRING("", at(s, "rx_offset_rssi").c_str());
}

static void receiver_adjustment_is_negated() {
    TEST_ASSERT_EQUAL_STRING("-20", at(run(configured(), S3), "rx_offset_rssi").c_str());  // the old implicit +20
    TEST_ASSERT_EQUAL_STRING("-7", at(run(configured({{"rx_adj_rssi", "7"}})), "rx_offset_rssi").c_str());
    TEST_ASSERT_EQUAL_STRING("3", at(run(configured({{"rx_adj_rssi", "-3"}}), S3), "rx_offset_rssi").c_str());
    TEST_ASSERT_EQUAL_STRING("", at(run(configured()), "rx_offset_rssi").c_str());  // 0 stays the default
    TEST_ASSERT_EQUAL_STRING("4", at(run(configured({{"rx_offset_rssi", "4"}, {"rx_adj_rssi", "9"}})), "rx_offset_rssi").c_str());
    TEST_ASSERT_EQUAL_STRING("9", at(run(configured({{"rx_adj_rssi", "9"}})), "rx_adj_rssi").c_str());  // kept for a downgrade
}

static void v5_beta_node_only_converts_the_offset() {
    Store s = run(configured({{"rx_adj_rssi", "5"}, {"BME280_I2c", "0x76"}}));
    TEST_ASSERT_EQUAL_STRING("-5", at(s, "rx_offset_rssi").c_str());
    TEST_ASSERT_EQUAL_STRING("", at(s, "leds").c_str());
    TEST_ASSERT_EQUAL_STRING("", at(s, "I2C_Bus_1_SDA").c_str());  // no v4 keys: not a v4 node
}

static void v4_leds_become_the_list() {
    Store s = run(configured({{"led_1_pin", "27"}, {"led_1_type", "2"}, {"led_1_cnt", "25"}, {"led_1_cntrl", "1"},
                              {"led_2_pin", "5"}, {"led_2_cntrl", "3"}}));
    TEST_ASSERT_EQUAL_STRING(R"([{"type":"grb","pin":27,"count":25,"control":"status"},{"type":"pwm","pin":5,"count":1,"control":"count"}])",
                             at(s, "leds").c_str());
    // An LED turned off on v4 stays off (v5's default would light GPIO 2).
    TEST_ASSERT_EQUAL_STRING("[]", at(run(configured({{"led_1_pin", "-1"}})), "leds").c_str());
    // Only led_1_type changed: LED 1 was on v4's default pin.
    TEST_ASSERT_EQUAL_STRING(R"([{"type":"pwm_inverted","pin":2,"count":1,"control":"status"}])",
                             at(run(configured({{"led_1_type", "1"}})), "leds").c_str());
    TEST_ASSERT_EQUAL_STRING("[]", at(run(configured({{"leds", "[]"}, {"led_1_pin", "4"}})), "leds").c_str());  // v5 list wins
}

static void v4_inputs_become_the_list() {
    Store s = run(configured({{"pir_pin", "34"}, {"pir_type", "4"}, {"pir_timeout", "5"},
                              {"button_1_pin", "39"}, {"button_1_type", "5"}, {"switch_2_pin", "-1"}}));
    TEST_ASSERT_EQUAL_STRING(R"([{"name":"PIR","role":"motion","pin":34,"type":"floating","timeout":5},)"
                             R"({"name":"Button 1","role":"button","pin":39,"type":"floating_inverted","timeout":0.5}])",
                             at(s, "inputs").c_str());
    TEST_ASSERT_EQUAL_STRING("", at(run(configured({{"pir_pin", "-1"}})), "inputs").c_str());  // nothing enabled
}

static void v4_i2c_defaults_come_back_for_sensors_on_bus_1() {
    Store s = run(configured({{"pir_pin", "-1"}, {"BME280_I2c", "0x76"}}));
    TEST_ASSERT_EQUAL_STRING("21", at(s, "I2C_Bus_1_SDA").c_str());
    TEST_ASSERT_EQUAL_STRING("22", at(s, "I2C_Bus_1_SCL").c_str());
    TEST_ASSERT_EQUAL_STRING("19", at(run(configured({{"pir_pin", "-1"}, {"SHT_I2c_Bus", "1"}}), {0, 19, 18, 2}), "I2C_Bus_1_SDA").c_str());
    // Pins already stored, a sensor on bus 2, no sensor, or a chip v4 didn't build: untouched.
    TEST_ASSERT_EQUAL_STRING("4", at(run(configured({{"pir_pin", "-1"}, {"BME280_I2c", "0x76"}, {"I2C_Bus_1_SDA", "4"}})), "I2C_Bus_1_SDA").c_str());
    TEST_ASSERT_EQUAL_STRING("", at(run(configured({{"pir_pin", "-1"}, {"BME280_I2c", "0x76"}, {"BME280_I2c_Bus", "2"}})), "I2C_Bus_1_SDA").c_str());
    TEST_ASSERT_EQUAL_STRING("", at(run(configured({{"pir_pin", "-1"}})), "I2C_Bus_1_SDA").c_str());
    TEST_ASSERT_EQUAL_STRING("", at(run(configured({{"pir_pin", "-1"}, {"BME280_I2c", "0x76"}}), C6), "I2C_Bus_1_SDA").c_str());
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(fresh_install_only_marks_itself);
    RUN_TEST(ethernet_only_node_still_converts);
    RUN_TEST(runs_once);
    RUN_TEST(receiver_adjustment_is_negated);
    RUN_TEST(v5_beta_node_only_converts_the_offset);
    RUN_TEST(v4_leds_become_the_list);
    RUN_TEST(v4_inputs_become_the_list);
    RUN_TEST(v4_i2c_defaults_come_back_for_sensors_on_bus_1);
    return UNITY_END();
}
