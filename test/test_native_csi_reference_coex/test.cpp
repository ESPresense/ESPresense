#include "unity.h"
#include <cstring>
#include <initializer_list>
#include "CsiReferenceCoex.h"

void default_and_malformed_requests_keep_the_library_schedule() {
    for (const char* text : {"0", "", "1,80", "1,80,20,5", "1, 80,20", "+1,80,20", "-1,80,20", "1,80,20 ",
                             "a,80,20", "1,80,x", "1,,20", "000001,80,20", (const char*)nullptr})
        TEST_ASSERT_FALSE(csiReferenceParseCoex(text).valid);
}
void shares_must_sum_to_one_hundred_and_stay_bounded() {
    for (const char* text : {"1,80,21", "1,79,20", "1,96,4", "1,4,96", "0,80,20", "61,80,20", "1,100,0"})
        TEST_ASSERT_FALSE(csiReferenceParseCoex(text).valid);
    const auto request = csiReferenceParseCoex("2,80,20");
    TEST_ASSERT_TRUE(request.valid);
    TEST_ASSERT_EQUAL_UINT8(2, request.period);
    TEST_ASSERT_EQUAL_UINT8(80, request.wifi);
    TEST_ASSERT_EQUAL_UINT8(20, request.bt);
}
void ble_phase_stays_under_thirty_beacon_intervals() {
    TEST_ASSERT_TRUE(csiReferenceParseCoex("60,50,50").valid);   // exactly 30 intervals
    TEST_ASSERT_FALSE(csiReferenceParseCoex("60,49,51").valid);  // 30.6 intervals
    TEST_ASSERT_TRUE(csiReferenceParseCoex("40,50,50").valid);
    TEST_ASSERT_FALSE(csiReferenceParseCoex("40,20,80").valid);  // 32 intervals
}
void apply_changes_only_period_and_shares() {
    uint8_t table[10];
    std::memcpy(table, CSI_REFERENCE_COEX_LIBRARY_TABLE, sizeof(table));
    csiReferenceApplyCoex(table, csiReferenceParseCoex("0"));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(CSI_REFERENCE_COEX_LIBRARY_TABLE, table, 10);
    csiReferenceApplyCoex(table, csiReferenceParseCoex("2,90,10"));
    const uint8_t expected[10] = {2, 2, 90, 3, 0, 0, 10, 0, 0, 0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, table, 10);
}
void scan_cycle_keeps_scanning_at_least_half_of_a_bounded_cycle() {
    for (const char* text : {"0", "", "3", "3,12,1", "0,12", "11,60", "3,5", "3,121", "3, 12", (const char*)nullptr})
        TEST_ASSERT_FALSE(csiReferenceParseScanCycle(text).valid);
    const auto cycle = csiReferenceParseScanCycle("3,12");
    TEST_ASSERT_TRUE(cycle.valid);
    TEST_ASSERT_EQUAL_UINT16(3, cycle.pauseSeconds);
    TEST_ASSERT_EQUAL_UINT16(12, cycle.periodSeconds);
    TEST_ASSERT_TRUE(csiReferenceParseScanCycle("10,20").valid);
}
void pause_leads_each_cycle_and_never_applies_without_a_cycle() {
    const auto cycle = csiReferenceParseScanCycle("3,12");
    TEST_ASSERT_TRUE(csiReferenceScanPausedAt(0, cycle));
    TEST_ASSERT_TRUE(csiReferenceScanPausedAt(2999, cycle));
    TEST_ASSERT_FALSE(csiReferenceScanPausedAt(3000, cycle));
    TEST_ASSERT_FALSE(csiReferenceScanPausedAt(11999, cycle));
    TEST_ASSERT_TRUE(csiReferenceScanPausedAt(12000, cycle));
    TEST_ASSERT_FALSE(csiReferenceScanPausedAt(0, CsiReferenceScanCycle{}));
}
void only_whole_second_plans_request_the_calibration_assist() {
    const CsiReferenceScanCycle none;
    TEST_ASSERT_FALSE(csiReferenceNeedsCalibrationAssist(csiReferenceParseCoex("0"), none));
    TEST_ASSERT_FALSE(csiReferenceNeedsCalibrationAssist(csiReferenceParseCoex("1,80,20"), none));
    TEST_ASSERT_FALSE(csiReferenceNeedsCalibrationAssist(csiReferenceParseCoex("4,80,20"), none));
    TEST_ASSERT_TRUE(csiReferenceNeedsCalibrationAssist(csiReferenceParseCoex("40,50,50"), none));
    TEST_ASSERT_TRUE(csiReferenceNeedsCalibrationAssist(csiReferenceParseCoex("0"), csiReferenceParseScanCycle("3,12")));
}
int main() {
    UNITY_BEGIN();
    RUN_TEST(default_and_malformed_requests_keep_the_library_schedule);
    RUN_TEST(shares_must_sum_to_one_hundred_and_stay_bounded);
    RUN_TEST(ble_phase_stays_under_thirty_beacon_intervals);
    RUN_TEST(apply_changes_only_period_and_shares);
    RUN_TEST(scan_cycle_keeps_scanning_at_least_half_of_a_bounded_cycle);
    RUN_TEST(pause_leads_each_cycle_and_never_applies_without_a_cycle);
    RUN_TEST(only_whole_second_plans_request_the_calibration_assist);
    return UNITY_END();
}
