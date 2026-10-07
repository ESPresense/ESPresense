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

}  // namespace

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_parses_uuid);
    RUN_TEST(test_rejects_wrong_length);
    RUN_TEST(test_rejects_other_apple_frame);
    RUN_TEST(test_rejects_altbeacon);
    return UNITY_END();
}
