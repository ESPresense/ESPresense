// Teltonika EYE beacon battery telemetry parsing (Company ID 0x089A, protocol v1).
//
// This data only ever arrives in a BLE Scan Response manufacturer-data AD structure, which is
// solicited from firmware only during a periodic active-scan window and merged onto whatever
// device already exists for that MAC — the parser itself never sees or affects iBeacon
// identity, it only ever computes {valid, hasVoltage, mv, lowBattery} from one AD structure's
// raw bytes, so it must never read out of bounds or misclassify a non-Teltonika payload.
#include <cstdint>
#include <vector>

#include <unity.h>

#include "../../src/TeltonikaEye.h"

namespace {

TeltonikaEyeData parse(std::vector<uint8_t> bytes) {
    return parseTeltonikaEye(bytes.data(), bytes.size());
}

void test_real_packet(void) {
    // 9A 08 = company id 0x089A (LE), 01 = protocol v1, 80 = voltage-present flag only, 6D = raw battery
    auto eye = parse({0x9A, 0x08, 0x01, 0x80, 0x6D});
    TEST_ASSERT_TRUE(eye.valid);
    TEST_ASSERT_TRUE(eye.hasVoltage);
    TEST_ASSERT_EQUAL_UINT16(3090, eye.mv);
    TEST_ASSERT_FALSE(eye.lowBattery);
}

void test_low_battery_forces_indicator_regardless_of_voltage(void) {
    // bit6 (0x40) set = low battery, regardless of whether voltage is present/what it computes to
    auto eye = parse({0x9A, 0x08, 0x01, 0xC0, 0x6D});
    TEST_ASSERT_TRUE(eye.valid);
    TEST_ASSERT_TRUE(eye.lowBattery);
    TEST_ASSERT_TRUE(eye.hasVoltage);
    TEST_ASSERT_EQUAL_UINT16(3090, eye.mv);

    auto eyeLowOnly = parse({0x9A, 0x08, 0x01, 0x40, 0x6D});
    TEST_ASSERT_TRUE(eyeLowOnly.valid);
    TEST_ASSERT_TRUE(eyeLowOnly.lowBattery);
    TEST_ASSERT_FALSE(eyeLowOnly.hasVoltage);
}

void test_no_voltage_present_flag(void) {
    // flags = 0x00: neither low-battery nor voltage-present bits set
    auto eye = parse({0x9A, 0x08, 0x01, 0x00, 0x6D});
    TEST_ASSERT_TRUE(eye.valid);
    TEST_ASSERT_FALSE(eye.hasVoltage);
    TEST_ASSERT_EQUAL_UINT16(0, eye.mv);
    TEST_ASSERT_FALSE(eye.lowBattery);
}

void test_foreign_manufacturer_is_ignored(void) {
    // Apple's 0x004C, not Teltonika's 0x089A
    auto eye = parse({0x4C, 0x00, 0x01, 0x80, 0x6D});
    TEST_ASSERT_FALSE(eye.valid);
}

void test_wrong_protocol_version_is_ignored(void) {
    auto eye = parse({0x9A, 0x08, 0x02, 0x80, 0x6D});
    TEST_ASSERT_FALSE(eye.valid);
}

void test_short_data_does_not_read_out_of_bounds(void) {
    TEST_ASSERT_FALSE(parse({}).valid);
    TEST_ASSERT_FALSE(parse({0x9A}).valid);
    TEST_ASSERT_FALSE(parse({0x9A, 0x08}).valid);

    // Valid header but truncated before flags/rawBattery: must report valid (it is a Teltonika
    // EYE v1 packet) without fabricating flags or a voltage.
    auto eye = parse({0x9A, 0x08, 0x01});
    TEST_ASSERT_TRUE(eye.valid);
    TEST_ASSERT_FALSE(eye.hasVoltage);
    TEST_ASSERT_FALSE(eye.lowBattery);

    // Flags present but voltage-present bit set with no raw battery byte following.
    auto eyeNoRaw = parse({0x9A, 0x08, 0x01, 0x80});
    TEST_ASSERT_TRUE(eyeNoRaw.valid);
    TEST_ASSERT_FALSE(eyeNoRaw.hasVoltage);
}

void test_null_data_is_safe(void) {
    TeltonikaEyeData eye = parseTeltonikaEye(nullptr, 5);
    TEST_ASSERT_FALSE(eye.valid);
}

}  // namespace

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_real_packet);
    RUN_TEST(test_low_battery_forces_indicator_regardless_of_voltage);
    RUN_TEST(test_no_voltage_present_flag);
    RUN_TEST(test_foreign_manufacturer_is_ignored);
    RUN_TEST(test_wrong_protocol_version_is_ignored);
    RUN_TEST(test_short_data_does_not_read_out_of_bounds);
    RUN_TEST(test_null_data_is_safe);
    return UNITY_END();
}
