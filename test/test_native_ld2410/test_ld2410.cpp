// LD2410 report frame decoding.
#include <unity.h>

#include <vector>

#include "LD2410.h"

namespace {
// A basic report frame: state, moving cm/energy, still cm/energy, detection cm.
std::vector<uint8_t> basic(uint8_t state, uint16_t mCm, uint8_t mE, uint16_t sCm, uint8_t sE, uint16_t dCm) {
    return {0xF4, 0xF3, 0xF2, 0xF1, 0x0D, 0x00,
            0x02, 0xAA, state, (uint8_t)mCm, (uint8_t)(mCm >> 8), mE, (uint8_t)sCm, (uint8_t)(sCm >> 8), sE,
            (uint8_t)dCm, (uint8_t)(dCm >> 8), 0x55, 0x00,
            0xF8, 0xF7, 0xF6, 0xF5};
}

// An engineering frame: the same leading fields, then gate counts, 9 + 9 gate energies, 2 extra.
std::vector<uint8_t> engineering(uint8_t state, uint16_t mCm, uint16_t sCm) {
    std::vector<uint8_t> p = {0x01, 0xAA, state, (uint8_t)mCm, (uint8_t)(mCm >> 8), 50, (uint8_t)sCm, (uint8_t)(sCm >> 8), 60, 0x10, 0x00,
                              0x08, 0x08};
    for (int i = 0; i < 18; i++) p.push_back(i);
    p.push_back(0x00);
    p.push_back(0x00);
    p.push_back(0x55);
    p.push_back(0x00);
    std::vector<uint8_t> f = {0xF4, 0xF3, 0xF2, 0xF1, (uint8_t)p.size(), 0x00};
    f.insert(f.end(), p.begin(), p.end());
    f.insert(f.end(), {0xF8, 0xF7, 0xF6, 0xF5});
    return f;
}

// Feeds bytes; returns how many frames decoded, the last one in out.
int feedAll(Ld2410Parser& p, const std::vector<uint8_t>& bytes, Ld2410Reading& out) {
    int n = 0;
    for (auto b : bytes) n += p.feed(b, out);
    return n;
}
}  // namespace

void test_decodes_a_basic_frame() {
    Ld2410Parser p;
    Ld2410Reading r;
    TEST_ASSERT_EQUAL(1, feedAll(p, basic(0x03, 120, 45, 310, 80, 150), r));
    TEST_ASSERT_TRUE(r.moving);
    TEST_ASSERT_TRUE(r.still);
    TEST_ASSERT_EQUAL(120, r.movingCm);
    TEST_ASSERT_EQUAL(45, r.movingEnergy);
    TEST_ASSERT_EQUAL(310, r.stillCm);
    TEST_ASSERT_EQUAL(80, r.stillEnergy);
    TEST_ASSERT_EQUAL(150, r.detectionCm);
}

void test_target_state_bits() {
    Ld2410Parser p;
    Ld2410Reading r;
    feedAll(p, basic(0x00, 0, 0, 0, 0, 0), r);
    TEST_ASSERT_FALSE(r.moving);
    TEST_ASSERT_FALSE(r.still);
    feedAll(p, basic(0x01, 0, 0, 0, 0, 0), r);
    TEST_ASSERT_TRUE(r.moving);
    TEST_ASSERT_FALSE(r.still);
    feedAll(p, basic(0x02, 0, 0, 0, 0, 0), r);
    TEST_ASSERT_FALSE(r.moving);
    TEST_ASSERT_TRUE(r.still);
}

void test_decodes_an_engineering_frame() {
    Ld2410Parser p;
    Ld2410Reading r;
    TEST_ASSERT_EQUAL(1, feedAll(p, engineering(0x01, 200, 0), r));
    TEST_ASSERT_TRUE(r.moving);
    TEST_ASSERT_EQUAL(200, r.movingCm);
    TEST_ASSERT_EQUAL(50, r.movingEnergy);
    TEST_ASSERT_EQUAL(16, r.detectionCm);
}

void test_resyncs_after_garbage_and_split_frames() {
    Ld2410Parser p;
    Ld2410Reading r;
    std::vector<uint8_t> s = {0x00, 0xF4, 0xF4, 0xF3, 0x12, 0xF4};  // false starts
    auto f = basic(0x02, 0, 0, 90, 70, 90);
    s.insert(s.end(), f.begin(), f.end());
    s.insert(s.end(), f.begin(), f.end());
    TEST_ASSERT_EQUAL(2, feedAll(p, s, r));
    TEST_ASSERT_EQUAL(90, r.stillCm);
}

void test_rejects_bad_frames() {
    Ld2410Parser p;
    Ld2410Reading r;
    auto badTail = basic(0x01, 1, 1, 1, 1, 1);
    badTail.back() = 0x00;
    auto badHead = basic(0x01, 1, 1, 1, 1, 1);
    badHead[7] = 0xAB;  // 0xAA marker
    auto badCheck = basic(0x01, 1, 1, 1, 1, 1);
    badCheck[17] = 0x54;  // 0x55 marker
    auto badType = basic(0x01, 1, 1, 1, 1, 1);
    badType[6] = 0x03;
    TEST_ASSERT_EQUAL(0, feedAll(p, badTail, r));
    TEST_ASSERT_EQUAL(0, feedAll(p, badHead, r));
    TEST_ASSERT_EQUAL(0, feedAll(p, badCheck, r));
    TEST_ASSERT_EQUAL(0, feedAll(p, badType, r));
    // ...and the next good frame still decodes.
    TEST_ASSERT_EQUAL(1, feedAll(p, basic(0x01, 5, 5, 5, 5, 5), r));
}

void test_rejects_oversize_and_tiny_lengths() {
    Ld2410Parser p;
    Ld2410Reading r;
    // A command ACK header is ignored; a report header with an absurd length is dropped.
    std::vector<uint8_t> s = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01,
                              0xF4, 0xF3, 0xF2, 0xF1, 0xFF, 0x7F,
                              0xF4, 0xF3, 0xF2, 0xF1, 0x02, 0x00};
    TEST_ASSERT_EQUAL(0, feedAll(p, s, r));
    TEST_ASSERT_EQUAL(1, feedAll(p, basic(0x02, 0, 0, 42, 9, 42), r));
    TEST_ASSERT_EQUAL(42, r.stillCm);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_decodes_a_basic_frame);
    RUN_TEST(test_target_state_bits);
    RUN_TEST(test_decodes_an_engineering_frame);
    RUN_TEST(test_resyncs_after_garbage_and_split_frames);
    RUN_TEST(test_rejects_bad_frames);
    RUN_TEST(test_rejects_oversize_and_tiny_lengths);
    return UNITY_END();
}
