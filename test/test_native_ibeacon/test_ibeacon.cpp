// iBeacon frame parser used to key a second fingerprint per UUID on one MAC (#2492).
#include <cstring>

#include <unity.h>

#include "../../main/IBeacon.h"

namespace {

// BC04P-style frame: 004c 02 15 <uuid> <major> <minor> <rssi@1m>
const uint8_t frame[25] = {0x4c, 0x00, 0x02, 0x15,
                           0x42, 0x6c, 0x75, 0x65, 0x43, 0x68, 0x61, 0x72, 0x6d, 0x42, 0x65, 0x61, 0x63, 0x6f, 0x6e, 0x73,
                           0x00, 0x01, 0x00, 0x02, 0xc5};

void test_parses_uuid(void) {
    uint8_t out[16];
    TEST_ASSERT_TRUE(iBeaconUuid(frame, sizeof frame, out));
    TEST_ASSERT_EQUAL_MEMORY(frame + 4, out, 16);
}

void test_rejects_wrong_length(void) {
    uint8_t out[16];
    TEST_ASSERT_FALSE(iBeaconUuid(frame, 24, out));
}

void test_rejects_other_apple_frame(void) {
    uint8_t nearby[25];
    memcpy(nearby, frame, sizeof frame); /* Flawfinder: ignore */
    nearby[2] = 0x10;
    uint8_t out[16];
    TEST_ASSERT_FALSE(iBeaconUuid(nearby, sizeof nearby, out));
}

void test_rejects_altbeacon(void) {
    uint8_t alt[26] = {0xac, 0xbe, 0xac, 0xbe};
    uint8_t out[16];
    TEST_ASSERT_FALSE(iBeaconUuid(alt, sizeof alt, out));
}

// Which same-MAC fingerprint an advert feeds. Candidates are fed newest (highest slot) first.
const uint16_t A = 0x1111, B = 0x2222;

void test_non_beacon_takes_newest(void) {
    IBeaconPick p{false, 0};
    TEST_ASSERT_TRUE(p.consider(7, true, A));
    TEST_ASSERT_EQUAL(7, p.result());
}

void test_locked_match_wins(void) {
    IBeaconPick p{true, A};
    TEST_ASSERT_TRUE(p.consider(3, true, A));
    TEST_ASSERT_EQUAL(3, p.result());
}

void test_other_uuid_is_skipped_so_a_second_fingerprint_is_created(void) {
    IBeaconPick p{true, B};
    TEST_ASSERT_FALSE(p.consider(3, true, A));
    TEST_ASSERT_EQUAL(IBeaconPick::none, p.result());
}

void test_older_locked_match_beats_newer_unlocked(void) {
    IBeaconPick p{true, A};
    TEST_ASSERT_FALSE(p.consider(5, false, 0));  // newer, no UUID yet
    TEST_ASSERT_TRUE(p.consider(2, true, A));    // older, locked to A
    TEST_ASSERT_EQUAL(2, p.result());
}

void test_unlocked_used_only_when_nothing_matches(void) {
    IBeaconPick p{true, B};
    TEST_ASSERT_FALSE(p.consider(5, true, A));
    TEST_ASSERT_FALSE(p.consider(4, false, 0));
    TEST_ASSERT_FALSE(p.consider(1, false, 0));
    TEST_ASSERT_EQUAL(4, p.result());  // newest unlocked
}

void test_no_candidates(void) {
    IBeaconPick p{true, A};
    TEST_ASSERT_EQUAL(IBeaconPick::none, p.result());
}

}  // namespace

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_parses_uuid);
    RUN_TEST(test_rejects_wrong_length);
    RUN_TEST(test_rejects_other_apple_frame);
    RUN_TEST(test_rejects_altbeacon);
    RUN_TEST(test_non_beacon_takes_newest);
    RUN_TEST(test_locked_match_wins);
    RUN_TEST(test_other_uuid_is_skipped_so_a_second_fingerprint_is_created);
    RUN_TEST(test_older_locked_match_beats_newer_unlocked);
    RUN_TEST(test_unlocked_used_only_when_nothing_matches);
    RUN_TEST(test_no_candidates);
    return UNITY_END();
}
