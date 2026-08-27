// MikroTik TG-BT5 native manufacturer-data detection: which frames earn a `mikrotik:{mac}` id.
//
// The gating has to be strict enough that foreign vendors and truncated frames fall through to
// the generic `md:` fingerprint instead of claiming the MikroTik id. The valid frame here is the
// worked example from MikroTik's own docs (non-encrypted TG-BT5-IN payload).
#include <cstdint>
#include <initializer_list>
#include <string>

#include <unity.h>

#include "../../src/MikrotikTag.h"

namespace {

std::string bytes(std::initializer_list<int> xs) {
    std::string out;
    for (int x : xs) out.push_back(static_cast<char>(static_cast<uint8_t>(x)));
    return out;
}

// From https://help.mikrotik.com/docs/display/UM/MikroTik+Tag+advertisement+formats:
// 020106 15 ff 4f09 01 00 cea6 0000 0000 0200 a01c 91085700 00 5f
// -> manufacturer data = company id + version + userdata + 16-byte secret = 20 bytes.
const std::string docExample = bytes({0x4f, 0x09, 0x01, 0x00, 0xce, 0xa6, 0x00, 0x00, 0x00, 0x00,
                                      0x02, 0x00, 0xa0, 0x1c, 0x91, 0x08, 0x57, 0x00, 0x00, 0x5f});

void test_documented_frame_is_recognized(void) {
    TEST_ASSERT_TRUE(isMikrotikTagManufacturerData(docExample));
}

void test_foreign_company_id_is_ignored(void) {
    // Same length and a plausible version byte, but Apple's (004c) and unknown (ffff) ids.
    TEST_ASSERT_FALSE(isMikrotikTagManufacturerData(bytes({0x4c, 0x00, 0x01, 0x00, 0xce, 0xa6, 0x00, 0x00,
                                                           0x00, 0x00, 0x02, 0x00, 0xa0, 0x1c, 0x91, 0x08,
                                                           0x57, 0x00, 0x00, 0x5f})));
    TEST_ASSERT_FALSE(isMikrotikTagManufacturerData(bytes({0xff, 0xff, 0x01, 0x00, 0xce, 0xa6, 0x00, 0x00,
                                                           0x00, 0x00, 0x02, 0x00, 0xa0, 0x1c, 0x91, 0x08,
                                                           0x57, 0x00, 0x00, 0x5f})));
    // Company id bytes reversed (big-endian 094f on air) is a different vendor, not MikroTik.
    TEST_ASSERT_FALSE(isMikrotikTagManufacturerData(bytes({0x09, 0x4f, 0x01, 0x00, 0xce, 0xa6, 0x00, 0x00,
                                                           0x00, 0x00, 0x02, 0x00, 0xa0, 0x1c, 0x91, 0x08,
                                                           0x57, 0x00, 0x00, 0x5f})));
}

void test_truncated_frames_are_ignored(void) {
    // Company id only, and a half frame: neither may claim the MikroTik id.
    TEST_ASSERT_FALSE(isMikrotikTagManufacturerData(bytes({0x4f, 0x09})));
    TEST_ASSERT_FALSE(isMikrotikTagManufacturerData(docExample.substr(0, 10)));
    TEST_ASSERT_FALSE(isMikrotikTagManufacturerData(""));
}

void test_overlong_frame_is_ignored(void) {
    std::string padded = docExample + bytes({0x00});
    TEST_ASSERT_FALSE(isMikrotikTagManufacturerData(padded));
}

void test_unknown_version_is_ignored(void) {
    // A future format version falls back to the generic md: fingerprint until parsed.
    std::string v2 = docExample;
    v2[2] = 0x02;
    TEST_ASSERT_FALSE(isMikrotikTagManufacturerData(v2));
}

void test_ibeaacon_style_payload_is_ignored(void) {
    // iBeacon frames live under Apple's company id and must never match here.
    std::string ib = bytes({0x4c, 0x00, 0x02, 0x15});
    TEST_ASSERT_FALSE(isMikrotikTagManufacturerData(ib));
}

}  // namespace

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_documented_frame_is_recognized);
    RUN_TEST(test_foreign_company_id_is_ignored);
    RUN_TEST(test_truncated_frames_are_ignored);
    RUN_TEST(test_overlong_frame_is_ignored);
    RUN_TEST(test_unknown_version_is_ignored);
    RUN_TEST(test_ibeaacon_style_payload_is_ignored);
    return UNITY_END();
}
