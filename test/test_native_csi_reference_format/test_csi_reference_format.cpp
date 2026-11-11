#include "unity.h"
#include "CsiReferenceFormat.h"
#include <cstdint>
#include <cstring>

void test_zero_is_a_json_number() {
    char buffer[21];
    TEST_ASSERT_EQUAL_STRING("0", csiReferenceDecimal(0, buffer));
}

void test_microseconds_remain_exact_past_32_bit_wrap() {
    char buffer[21];
    TEST_ASSERT_EQUAL_STRING("4294967296", csiReferenceDecimal(UINT64_C(4294967296), buffer));
    TEST_ASSERT_EQUAL_STRING("172800000000", csiReferenceDecimal(UINT64_C(172800000000), buffer));
}

void test_utc_and_full_width_counters_do_not_lose_digits() {
    char buffer[21];
    TEST_ASSERT_EQUAL_STRING("1794342390", csiReferenceDecimal(UINT64_C(1794342390), buffer));
    TEST_ASSERT_EQUAL_STRING("18446744073709551615", csiReferenceDecimal(UINT64_MAX, buffer));
}

void test_small_value_after_max_has_no_stale_suffix() {
    char buffer[21];
    csiReferenceDecimal(UINT64_MAX, buffer);
    TEST_ASSERT_EQUAL_STRING("7", csiReferenceDecimal(7, buffer));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_zero_is_a_json_number);
    RUN_TEST(test_microseconds_remain_exact_past_32_bit_wrap);
    RUN_TEST(test_utc_and_full_width_counters_do_not_lose_digits);
    RUN_TEST(test_small_value_after_max_has_no_stale_suffix);
    return UNITY_END();
}
