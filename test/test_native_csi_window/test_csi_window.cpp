#include "unity.h"
#include "CsiWindow.h"

#include <vector>

static std::vector<int8_t> frame(int amplitude, int ripple = 0) {
    std::vector<int8_t> csi(CsiWindow::kMaxBytes);
    for (size_t i = 0; i < CsiWindow::kMaxSubcarriers; i++) {
        csi[i * 2] = 0;
        csi[i * 2 + 1] = static_cast<int8_t>(amplitude + (i % 2 ? ripple : -ripple));
    }
    return csi;
}

static void add(CsiWindow& w, const std::vector<int8_t>& csi) { w.add(csi.data(), csi.size()); }

void test_identical_frames_have_no_variance() {
    CsiWindow w;
    for (int i = 0; i < 20; i++) add(w, frame(30));
    TEST_ASSERT_EQUAL_UINT32(20, w.frames());
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, w.variance());
}

void test_changing_channel_shape_raises_variance() {
    CsiWindow still, moving;
    for (int i = 0; i < 20; i++) {
        add(still, frame(30, 5));
        add(moving, frame(30, i % 2 ? 5 : 15));
    }
    TEST_ASSERT_TRUE(moving.variance() > still.variance() + 100.0f);
}

// Gain control rescales a whole frame. That is not movement.
void test_gain_change_is_not_variance() {
    CsiWindow w;
    for (int i = 0; i < 20; i++) add(w, frame(i % 2 ? 20 : 40, i % 2 ? 2 : 4));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, w.variance());
}

void test_frame_of_another_length_is_skipped() {
    CsiWindow w;
    add(w, frame(30));
    auto shorter = frame(90);
    w.add(shorter.data(), 64);
    TEST_ASSERT_EQUAL_UINT32(1, w.frames());
}

void test_silent_and_tiny_frames_are_skipped() {
    CsiWindow w;
    add(w, frame(0));
    auto tiny = frame(30);
    w.add(tiny.data(), 4);
    TEST_ASSERT_EQUAL_UINT32(0, w.frames());
    TEST_ASSERT_EQUAL_FLOAT(0.0f, w.variance());
}

void test_oversized_frame_is_capped() {
    CsiWindow w;
    std::vector<int8_t> big(400, 20);
    w.add(big.data(), big.size());
    add(w, frame(0, 20));  // full-length frame compares against the capped one
    TEST_ASSERT_EQUAL_UINT32(2, w.frames());
}

void test_reset_starts_a_new_window() {
    CsiWindow w;
    add(w, frame(30, 5));
    add(w, frame(30, 15));
    w.reset();
    TEST_ASSERT_EQUAL_UINT32(0, w.frames());
    add(w, frame(30));
    add(w, frame(30));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, w.variance());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_identical_frames_have_no_variance);
    RUN_TEST(test_changing_channel_shape_raises_variance);
    RUN_TEST(test_gain_change_is_not_variance);
    RUN_TEST(test_frame_of_another_length_is_skipped);
    RUN_TEST(test_silent_and_tiny_frames_are_skipped);
    RUN_TEST(test_oversized_frame_is_capped);
    RUN_TEST(test_reset_starts_a_new_window);
    return UNITY_END();
}
