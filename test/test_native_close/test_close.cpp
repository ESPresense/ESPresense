// Close/left hysteresis (nextClose) on normalized RSSI.
#include "unity.h"
#include "rssi.h"

static void enters_above_close_and_leaves_below_left() {
    TEST_ASSERT_TRUE(nextClose(false, -39.0f));
    TEST_ASSERT_FALSE(nextClose(true, -51.0f));
}

static void keeps_state_at_and_between_the_thresholds() {
    TEST_ASSERT_FALSE(nextClose(false, -40.0f));  // exactly CLOSE_RSSI: not above it
    TEST_ASSERT_TRUE(nextClose(true, -50.0f));    // exactly LEFT_RSSI: not below it
    TEST_ASSERT_FALSE(nextClose(false, -45.0f));
    TEST_ASSERT_TRUE(nextClose(true, -45.0f));
}

static void receiver_adjustment_counts_once() {
    // An S3 with the +20 default reading -10 dBm raw normalizes to -30: that is close. The old
    // check compared -30 with -40 + 20 = -20 and called it not close.
    float normalized = -10.0f - 20;
    TEST_ASSERT_TRUE(nextClose(false, normalized));
    // A negative adjustment (-5, a weak receiver) reading -48 raw normalizes to -43: not yet close.
    TEST_ASSERT_FALSE(nextClose(false, -48.0f + 5));
}

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(enters_above_close_and_leaves_below_left);
    RUN_TEST(keeps_state_at_and_between_the_thresholds);
    RUN_TEST(receiver_adjustment_counts_once);
    return UNITY_END();
}
