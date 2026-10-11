// CSE7766 frame decoding (Athom plugs).
#include <unity.h>

#include "CSE7766.h"

namespace {
void put24(uint8_t* f, size_t i, uint32_t v) {
    f[i] = v >> 16;
    f[i + 1] = v >> 8;
    f[i + 2] = v;
}

// 120 V, 100 W, 0.833 A, adj = voltage+current+power valid.
void frame(uint8_t* f, uint8_t state = 0x55, uint16_t cf = 1234) {
    f[0] = state;
    f[1] = 0x5A;
    put24(f, 2, 190000);
    put24(f, 5, 1583);
    put24(f, 8, 16000);
    put24(f, 11, 19200);
    put24(f, 14, 5000000);
    put24(f, 17, 50000);
    f[20] = 0x70;
    f[21] = cf >> 8;
    f[22] = cf & 0xFF;
    uint8_t sum = 0;
    for (int i = 2; i < 23; i++) sum += f[i];
    f[23] = sum;
}
}  // namespace


void test_decodes_a_normal_frame() {
    uint8_t f[24];
    frame(f);
    CseReading r;
    TEST_ASSERT_TRUE(cseDecode(f, r));
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 120.0f, r.voltage);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 100.0f, r.power);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.833f, r.current);
    TEST_ASSERT_EQUAL_UINT16(1234, r.cfPulses);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 5.0f, r.joulesPerPulse);
}

void test_bad_checksum_or_header_rejected() {
    uint8_t f[24];
    CseReading r;
    frame(f);
    f[23] ^= 1;
    TEST_ASSERT_FALSE(cseDecode(f, r));
    frame(f);
    f[1] = 0x00;
    TEST_ASSERT_FALSE(cseDecode(f, r));
    frame(f, 0xAA);  // not calibrated
    TEST_ASSERT_FALSE(cseDecode(f, r));
}

void test_power_overflow_reads_zero_power_and_current() {
    uint8_t f[24];
    frame(f, 0xF2);  // power cycle out of range: no load
    CseReading r;
    TEST_ASSERT_TRUE(cseDecode(f, r));
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 120.0f, r.voltage);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, r.power);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, r.current);
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_decodes_a_normal_frame);
    RUN_TEST(test_bad_checksum_or_header_rejected);
    RUN_TEST(test_power_overflow_reads_zero_power_and_current);
    return UNITY_END();
}
