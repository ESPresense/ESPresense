// 1 m references are rssi1m_t: their sums go below -128, and NO_RSSI must stay outside them.
#include <climits>
#include <limits>

#include "unity.h"
#include "rssi.h"

// BleFingerprint's references and DeviceConfig::calRssi are all rssi1m_t, so a narrower type
// fails here rather than wrapping on the device.
static_assert(std::numeric_limits<rssi1m_t>::min() < -100 + INT8_MIN, "rssi1m_t can't hold ref_rssi + tx power");
static_assert(std::numeric_limits<rssi1m_t>::max() >= 100 + INT8_MAX, "rssi1m_t can't hold ref_rssi + tx power");

static void eddystone_converts_without_wrapping() {
    TEST_ASSERT_EQUAL_INT16(-141, eddystone1mRssi(-100));  // int8_t wrapped this to +115
    TEST_ASSERT_EQUAL_INT16(-41, eddystone1mRssi(0));
    TEST_ASSERT_EQUAL_INT16(-169, eddystone1mRssi(INT8_MIN));
}

static void sums_at_the_input_limits_fit() {
    // ref_rssi is -100..100 and an advertised tx power is any int8_t.
    rssi1m_t lowest = -100 + INT8_MIN, highest = 100 + INT8_MAX;
    TEST_ASSERT_EQUAL_INT16(-228, lowest);
    TEST_ASSERT_EQUAL_INT16(227, highest);
}

static void no_rssi_is_not_a_reference() {
    TEST_ASSERT_TRUE(NO_RSSI < eddystone1mRssi(INT8_MIN));
    TEST_ASSERT_TRUE(NO_RSSI < rssi1m_t(-100 + INT8_MIN));
    TEST_ASSERT_TRUE(rssi1m_t(-128) != NO_RSSI);  // -128 is a value now, not "missing"
}

static void rssi_settings_clamp_instead_of_wrapping() {
    TEST_ASSERT_EQUAL_INT8(-100, clampRssi(-200, -100, 0));  // (int8_t)-200 is +56
    TEST_ASSERT_EQUAL_INT8(0, clampRssi(300, -100, 0));      // (int8_t)300 is +44
    TEST_ASSERT_EQUAL_INT8(-59, clampRssi(-59, -100, 0));
    TEST_ASSERT_EQUAL_INT8(-100, clampRssi(-100, -100, 100));
    TEST_ASSERT_EQUAL_INT8(100, clampRssi(100, -100, 100));
    TEST_ASSERT_EQUAL_INT8(100, clampRssi(LONG_MAX, -100, 100));
}

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(eddystone_converts_without_wrapping);
    RUN_TEST(sums_at_the_input_limits_fit);
    RUN_TEST(no_rssi_is_not_a_reference);
    RUN_TEST(rssi_settings_clamp_instead_of_wrapping);
    return UNITY_END();
}
