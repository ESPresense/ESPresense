// DFRobot C4001 line decoding.
#include <unity.h>

#include <string>

#include "C4001.h"

namespace {
int feedAll(C4001Parser& p, const std::string& s, C4001Reading& out) {
    int n = 0;
    for (char c : s) n += p.feed((uint8_t)c, out);
    return n;
}
}  // namespace

void test_presence_lines() {
    C4001Parser p;
    C4001Reading r;
    TEST_ASSERT_EQUAL(1, feedAll(p, "$DFHPD,1, , , *\r\n", r));
    TEST_ASSERT_FALSE(r.speedMode);
    TEST_ASSERT_TRUE(r.present);
    TEST_ASSERT_EQUAL(1, feedAll(p, "$DFHPD,0, , , *\r\n", r));
    TEST_ASSERT_FALSE(r.present);
}

void test_speed_mode_line() {
    C4001Parser p;
    C4001Reading r;
    TEST_ASSERT_EQUAL(1, feedAll(p, "$DFDMD,1,1,1.817,0.129,15304, , *\r\n", r));
    TEST_ASSERT_TRUE(r.speedMode);
    TEST_ASSERT_TRUE(r.present);
    TEST_ASSERT_EQUAL(182, r.distanceCm);
    TEST_ASSERT_EQUAL(13, r.speedCms);
    TEST_ASSERT_EQUAL(15304, r.energy);
}

void test_speed_mode_no_target_and_negative_speed() {
    C4001Parser p;
    C4001Reading r;
    TEST_ASSERT_EQUAL(1, feedAll(p, "$DFDMD,1,1,3.2,-0.456,900, , *", r));
    TEST_ASSERT_EQUAL(-46, r.speedCms);
    TEST_ASSERT_EQUAL(1, feedAll(p, "$DFDMD,0,0,0.000,0.000,0, , *", r));
    TEST_ASSERT_TRUE(r.speedMode);
    TEST_ASSERT_FALSE(r.present);
    TEST_ASSERT_EQUAL(0, r.distanceCm);
}

void test_ignores_responses_noise_and_bad_lines() {
    C4001Parser p;
    C4001Reading r;
    TEST_ASSERT_EQUAL(0, feedAll(p, "leapMMW:/>sensorStart\r\nDone\r\n$JYRPO,6,0.6*\r\n", r));
    TEST_ASSERT_EQUAL(0, feedAll(p, "$DFHPD,x, , , *", r));
    TEST_ASSERT_EQUAL(0, feedAll(p, "$DFDMD,1,1*", r));
    TEST_ASSERT_EQUAL(0, feedAll(p, "garbage $DFH", r));
    // A '$' restarts the line, so a cut-off line doesn't swallow the next.
    TEST_ASSERT_EQUAL(1, feedAll(p, "$DFHPD,1, , , *", r));
    TEST_ASSERT_TRUE(r.present);
}

void test_runaway_line_is_dropped() {
    C4001Parser p;
    C4001Reading r;
    std::string junk = "$DFHPD,1" + std::string(200, ' ') + "*";
    TEST_ASSERT_EQUAL(0, feedAll(p, junk, r));
    TEST_ASSERT_EQUAL(1, feedAll(p, "$DFHPD,0, , , *", r));
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_presence_lines);
    RUN_TEST(test_speed_mode_line);
    RUN_TEST(test_speed_mode_no_target_and_negative_speed);
    RUN_TEST(test_ignores_responses_noise_and_bad_lines);
    RUN_TEST(test_runaway_line_is_dropped);
    return UNITY_END();
}
