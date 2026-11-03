// Teltonika EYE manufacturer data (#2490): optional fields sit between the flags and the battery
// byte, so the battery offset depends on which sensors are reported.
#include <unity.h>

#include "../../main/TeltonikaEye.h"

namespace {

void test_battery_only(void) {  // EYE beacon: 9a08 01, flags 0x80, battery 0x6d
    const uint8_t d[] = {0x9a, 0x08, 0x01, 0x80, 0x6d};
    TeltonikaEye e;
    TEST_ASSERT_TRUE(parseTeltonikaEye(d, sizeof d, e));
    TEST_ASSERT_TRUE(e.hasVoltage);
    TEST_ASSERT_EQUAL_UINT16(3090, e.mv);
    TEST_ASSERT_FALSE(e.lowBattery);
    TEST_ASSERT_FALSE(e.hasTemp);
    TEST_ASSERT_FALSE(e.hasHumidity);
}

void test_wiki_example_all_sensors(void) {  // Teltonika wiki: flags 0xb7 -> 22.28 °C, 18 %, 3030 mV
    const uint8_t d[] = {0x9a, 0x08, 0x01, 0xb7, 0x08, 0xb4, 0x12, 0x0c, 0xcb, 0x0b, 0xff, 0xc7, 0x67};
    TeltonikaEye e;
    TEST_ASSERT_TRUE(parseTeltonikaEye(d, sizeof d, e));
    TEST_ASSERT_TRUE(e.hasTemp);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 22.28f, e.temp);
    TEST_ASSERT_TRUE(e.hasHumidity);
    TEST_ASSERT_EQUAL_UINT8(18, e.humidity);
    TEST_ASSERT_TRUE(e.hasVoltage);
    TEST_ASSERT_EQUAL_UINT16(3030, e.mv);
}

void test_negative_temperature(void) {
    const uint8_t d[] = {0x9a, 0x08, 0x01, 0x81, 0xfc, 0x18, 0x64};  // -10.00 °C, 3000 mV
    TeltonikaEye e;
    TEST_ASSERT_TRUE(parseTeltonikaEye(d, sizeof d, e));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -10.0f, e.temp);
    TEST_ASSERT_EQUAL_UINT16(3000, e.mv);
}

void test_low_battery_without_voltage(void) {
    const uint8_t d[] = {0x9a, 0x08, 0x01, 0x40};
    TeltonikaEye e;
    TEST_ASSERT_TRUE(parseTeltonikaEye(d, sizeof d, e));
    TEST_ASSERT_TRUE(e.lowBattery);
    TEST_ASSERT_FALSE(e.hasVoltage);
}

void test_truncated_field_is_not_read(void) {
    const uint8_t d[] = {0x9a, 0x08, 0x01, 0x81, 0x08};  // temp flagged but one byte short
    TeltonikaEye e;
    TEST_ASSERT_TRUE(parseTeltonikaEye(d, sizeof d, e));
    TEST_ASSERT_FALSE(e.hasTemp);
    TEST_ASSERT_FALSE(e.hasVoltage);
}

void test_rejects_other_frames(void) {
    TeltonikaEye e;
    const uint8_t apple[] = {0x4c, 0x00, 0x02, 0x15};
    TEST_ASSERT_FALSE(parseTeltonikaEye(apple, sizeof apple, e));
    const uint8_t v2[] = {0x9a, 0x08, 0x02, 0x80, 0x6d};
    TEST_ASSERT_FALSE(parseTeltonikaEye(v2, sizeof v2, e));
    const uint8_t header[] = {0x9a, 0x08, 0x01};
    TEST_ASSERT_FALSE(parseTeltonikaEye(header, sizeof header, e));
}

}  // namespace

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_battery_only);
    RUN_TEST(test_wiki_example_all_sensors);
    RUN_TEST(test_negative_temperature);
    RUN_TEST(test_low_battery_without_voltage);
    RUN_TEST(test_truncated_field_is_not_read);
    RUN_TEST(test_rejects_other_frames);
    return UNITY_END();
}
