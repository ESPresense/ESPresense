// LD2450 report frame decoding.
#include <unity.h>

#include <vector>

#include "LD2450.h"

namespace {
// The example frame from the Hi-Link protocol manual: one target at x -782 mm, y 1713 mm,
// speed -16 cm/s, resolution 320 mm; the other two slots empty.
const std::vector<uint8_t> MANUAL = {0xAA, 0xFF, 0x03, 0x00,
                                     0x0E, 0x03, 0xB1, 0x86, 0x10, 0x00, 0x40, 0x01,
                                     0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0,
                                     0x55, 0xCC};

uint16_t enc(int v) { return v >= 0 ? (uint16_t)(0x8000 | v) : (uint16_t)(-v); }

void putTarget(std::vector<uint8_t>& f, int slot, int x, int y, int speed, int res) {
    uint16_t v[4] = {enc(x), enc(y), enc(speed), (uint16_t)res};
    for (int i = 0; i < 4; i++) {
        f[4 + slot * 8 + i * 2] = v[i] & 0xFF;
        f[4 + slot * 8 + i * 2 + 1] = v[i] >> 8;
    }
}

std::vector<uint8_t> empty() {
    std::vector<uint8_t> f(30, 0);
    f[0] = 0xAA, f[1] = 0xFF, f[2] = 0x03, f[3] = 0x00, f[28] = 0x55, f[29] = 0xCC;
    return f;
}

int feedAll(Ld2450Parser& p, const std::vector<uint8_t>& bytes, Ld2450Reading& out) {
    int n = 0;
    for (auto b : bytes) n += p.feed(b, out);
    return n;
}
}  // namespace

void test_decodes_the_manual_example() {
    Ld2450Parser p;
    Ld2450Reading r;
    TEST_ASSERT_EQUAL(1, feedAll(p, MANUAL, r));
    TEST_ASSERT_EQUAL(1, r.count());
    auto& t = r.targets[0];
    TEST_ASSERT_TRUE(t.present);
    TEST_ASSERT_EQUAL(-782, t.x);
    TEST_ASSERT_EQUAL(1713, t.y);
    TEST_ASSERT_EQUAL(-16, t.speed);
    TEST_ASSERT_EQUAL(320, t.resolution);
    TEST_ASSERT_EQUAL(1883, t.distance());  // sqrt(782² + 1713²) = 1883.06
    TEST_ASSERT_FALSE(r.targets[1].present);
    TEST_ASSERT_FALSE(r.targets[2].present);
}

void test_three_targets_and_signs() {
    Ld2450Parser p;
    Ld2450Reading r;
    auto f = empty();
    putTarget(f, 0, 500, 1000, 0, 360);
    putTarget(f, 1, -1200, 3000, 25, 360);
    putTarget(f, 2, 0, 6000, -40, 360);
    TEST_ASSERT_EQUAL(1, feedAll(p, f, r));
    TEST_ASSERT_EQUAL(3, r.count());
    TEST_ASSERT_EQUAL(500, r.targets[0].x);
    TEST_ASSERT_EQUAL(0, r.targets[0].speed);
    TEST_ASSERT_EQUAL(-1200, r.targets[1].x);
    TEST_ASSERT_EQUAL(25, r.targets[1].speed);
    TEST_ASSERT_EQUAL(0, r.targets[2].x);
    TEST_ASSERT_EQUAL(6000, r.targets[2].y);
    TEST_ASSERT_EQUAL(-40, r.targets[2].speed);
}

void test_targets_leaving_clear_their_slots() {
    Ld2450Parser p;
    Ld2450Reading r;
    feedAll(p, MANUAL, r);
    TEST_ASSERT_EQUAL(1, feedAll(p, empty(), r));
    TEST_ASSERT_EQUAL(0, r.count());
    TEST_ASSERT_EQUAL(0, r.targets[0].x);
}

void test_resyncs_after_garbage() {
    Ld2450Parser p;
    Ld2450Reading r;
    std::vector<uint8_t> s = {0x01, 0xAA, 0xAA, 0xFF, 0x02, 0xAA};  // false starts
    s.insert(s.end(), MANUAL.begin(), MANUAL.end());
    s.insert(s.end(), MANUAL.begin(), MANUAL.end());
    TEST_ASSERT_EQUAL(2, feedAll(p, s, r));
    TEST_ASSERT_EQUAL(1713, r.targets[0].y);
}

void test_rejects_a_bad_tail_then_recovers() {
    Ld2450Parser p;
    Ld2450Reading r;
    auto bad = MANUAL;
    bad[29] = 0xCD;
    TEST_ASSERT_EQUAL(0, feedAll(p, bad, r));
    TEST_ASSERT_EQUAL(1, feedAll(p, MANUAL, r));
}

void test_ignores_command_acks() {
    Ld2450Parser p;
    Ld2450Reading r;
    std::vector<uint8_t> ack = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
    TEST_ASSERT_EQUAL(0, feedAll(p, ack, r));
    TEST_ASSERT_EQUAL(1, feedAll(p, MANUAL, r));
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_decodes_the_manual_example);
    RUN_TEST(test_three_targets_and_signs);
    RUN_TEST(test_targets_leaving_clear_their_slots);
    RUN_TEST(test_resyncs_after_garbage);
    RUN_TEST(test_rejects_a_bad_tail_then_recovers);
    RUN_TEST(test_ignores_command_acks);
    return UNITY_END();
}
