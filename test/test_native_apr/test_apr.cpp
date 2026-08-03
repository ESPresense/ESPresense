// Losslessness guard for the right-sized (grow-on-demand) AdaptivePercentileRSSI buffer.
// The buffer now starts small and grows toward maxReadings on demand instead of pre-allocating
// Reading[maxReadings]. This test proves that change keeps the SAME readings and produces the
// SAME median/IQR as a direct reference — i.e. it's a pure memory win, not a behavior change.
#include <unity.h>
#include <cstdint>
#include <cmath>
#include <vector>
#include <algorithm>

uint32_t g_now = 0;

#include "AdaptivePercentileRSSI.h"

// Reference Tukey-fenced mean, matching getMedianIQR's algorithm, over an explicit value set.
static float refMedianIQR(std::vector<float> v, float k = 1.5f) {
    if (v.empty()) return 0.0f;
    std::sort(v.begin(), v.end());
    int n = (int)v.size();
    auto interp = [&](float p) { float pos = p * (n - 1); int lo = (int)pos; float f = pos - lo;
        return (lo + 1 < n) ? v[lo] * (1 - f) + v[lo + 1] * f : v[lo]; };
    float q1 = interp(0.25f), med = interp(0.5f), q3 = interp(0.75f), iqr = q3 - q1;
    float lo = q1 - k * iqr, hi = q3 + k * iqr;
    float sum = 0; int surv = 0;
    for (float x : v) if (x >= lo && x <= hi) { sum += x; surv++; }
    return surv ? sum / surv : med;
}

void test_few_readings_small_buffer(void) {  // the flood case: seen a couple times
    AdaptivePercentileRSSI f(15000, 20);
    std::vector<float> vals = {-60, -62};
    g_now = 0;
    for (float x : vals) { f.addMeasurement(x); g_now += 100; }
    TEST_ASSERT_EQUAL_UINT16(2, f.getReadingCount());
    TEST_ASSERT_FLOAT_WITHIN(1e-3, refMedianIQR(vals), f.getMedianIQR());
}

void test_over_threshold_holds_newest(void) {  // >maxReadings: keep newest 20, drop oldest
    AdaptivePercentileRSSI f(15000, 20);
    std::vector<float> all;
    g_now = 0;
    for (int i = 0; i < 30; i++) { float x = -50 - (i % 7); all.push_back(x); f.addMeasurement(x); g_now += 100; }
    TEST_ASSERT_EQUAL_UINT16(20, f.getReadingCount());
    std::vector<float> held(all.end() - 20, all.end());
    TEST_ASSERT_FLOAT_WITHIN(1e-3, refMedianIQR(held), f.getMedianIQR());
}

void test_grow_path_exact(void) {  // buffer grows 4->8->16->20, all 20 held
    AdaptivePercentileRSSI f(15000, 20);
    std::vector<float> all;
    g_now = 0;
    for (int i = 0; i < 20; i++) { float x = -40 - i; all.push_back(x); f.addMeasurement(x); g_now += 50; }
    TEST_ASSERT_EQUAL_UINT16(20, f.getReadingCount());
    TEST_ASSERT_FLOAT_WITHIN(1e-3, refMedianIQR(all), f.getMedianIQR());
}

void test_window_expiry(void) {  // readings older than the 15s window drop out
    AdaptivePercentileRSSI f(15000, 20);
    g_now = 0;
    f.addMeasurement(-90); g_now += 20000;
    std::vector<float> recent = {-55, -57, -56};
    for (float x : recent) { f.addMeasurement(x); g_now += 100; }
    TEST_ASSERT_FLOAT_WITHIN(1e-3, refMedianIQR(recent), f.getMedianIQR());
}

void test_copy_preserves(void) {  // copy ctor keeps the right-sized buffer + outputs
    AdaptivePercentileRSSI f(15000, 20);
    std::vector<float> vals = {-61, -63, -60, -62, -59};
    g_now = 0;
    for (float x : vals) { f.addMeasurement(x); g_now += 100; }
    AdaptivePercentileRSSI g = f;
    TEST_ASSERT_EQUAL_UINT16(5, g.getReadingCount());
    TEST_ASSERT_FLOAT_WITHIN(1e-3, refMedianIQR(vals), g.getMedianIQR());
}

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_few_readings_small_buffer);
    RUN_TEST(test_over_threshold_holds_newest);
    RUN_TEST(test_grow_path_exact);
    RUN_TEST(test_window_expiry);
    RUN_TEST(test_copy_preserves);
    return UNITY_END();
}
