// 1 m references are int16_t: their sums go below -128, and NO_RSSI must stay outside them.
#include "unity.h"
#include "rssi.h"

static void eddystone_converts_without_wrapping() {
    TEST_ASSERT_EQUAL_INT16(-141, eddystone1mRssi(-100));  // int8_t wrapped this to +115
    TEST_ASSERT_EQUAL_INT16(-41, eddystone1mRssi(0));
    TEST_ASSERT_EQUAL_INT16(-169, eddystone1mRssi(INT8_MIN));
}

static void sums_at_the_input_limits_fit() {
    // ref_rssi is -100..100 and an advertised tx power is any int8_t.
    int16_t lowest = int16_t(-100 + INT8_MIN), highest = int16_t(100 + INT8_MAX);
    TEST_ASSERT_EQUAL_INT16(-228, lowest);
    TEST_ASSERT_EQUAL_INT16(227, highest);
}

static void no_rssi_is_not_a_reference() {
    TEST_ASSERT_TRUE(NO_RSSI < eddystone1mRssi(INT8_MIN));
    TEST_ASSERT_TRUE(NO_RSSI < int16_t(-100 + INT8_MIN));
    TEST_ASSERT_TRUE(int16_t(-128) != NO_RSSI);  // -128 is a value now, not "missing"
}

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(eddystone_converts_without_wrapping);
    RUN_TEST(sums_at_the_input_limits_fit);
    RUN_TEST(no_rssi_is_not_a_reference);
    return UNITY_END();
}
