#include "unity.h"
#include <initializer_list>
#include "CsiReferenceScan.h"

void unarmed_boot_always_uses_full_window() {
    for (long profile : {-1L, 0L, 1L, 2L, 99L})
        TEST_ASSERT_EQUAL_UINT(128, csiReferenceScanWindowUnits(profile, false, 128));
}
void armed_profiles_keep_requested_interval_fraction() {
    TEST_ASSERT_EQUAL_UINT(128, csiReferenceScanWindowUnits(0, true, 128));
    TEST_ASSERT_EQUAL_UINT(32, csiReferenceScanWindowUnits(1, true, 128));
    TEST_ASSERT_EQUAL_UINT(16, csiReferenceScanWindowUnits(2, true, 128));
}
void invalid_profile_falls_back_to_full() {
    TEST_ASSERT_EQUAL_UINT(128, csiReferenceScanWindowUnits(-1, true, 128));
    TEST_ASSERT_EQUAL_UINT(128, csiReferenceScanWindowUnits(99, true, 128));
}
int main() {
    UNITY_BEGIN();
    RUN_TEST(unarmed_boot_always_uses_full_window);
    RUN_TEST(armed_profiles_keep_requested_interval_fraction);
    RUN_TEST(invalid_profile_falls_back_to_full);
    return UNITY_END();
}
