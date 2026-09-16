// getMedianIQR() sorts into a shared static buffer instead of a per-call new[] (#2309 heap
// churn). It must still give the Tukey-fenced mean, and one instance must not see another's data.
#include <algorithm>
#include <vector>

#include "unity.h"
#include "../../components/filtering/AdaptivePercentileRSSI.cpp"

// The pre-scratch-buffer algorithm, kept as the reference.
static float reference(std::vector<float> v, float k = 1.5f) {
    std::sort(v.begin(), v.end());
    auto interp = [&](float p) {
        float pos = p * (v.size() - 1);
        size_t lo = (size_t)pos;
        float frac = pos - lo;
        return lo + 1 < v.size() ? v[lo] * (1 - frac) + v[lo + 1] * frac : v[lo];
    };
    float q1 = interp(0.25f), med = interp(0.5f), q3 = interp(0.75f), iqr = q3 - q1;
    float sum = 0; int n = 0;
    for (float x : v)
        if (x >= q1 - k * iqr && x <= q3 + k * iqr) { sum += x; n++; }
    return n ? sum / n : med;
}

static void matches_reference_and_rejects_outliers() {
    AdaptivePercentileRSSI a;
    std::vector<float> seen;
    for (int i = 0; i < 20; i++) {
        float r = (i % 7 == 0) ? -20.0f : -70.0f - (i % 5);  // every 7th is a wild outlier
        a.addMeasurement(r);
        seen.push_back(r);
        TEST_ASSERT_EQUAL_FLOAT(reference(seen), a.getMedianIQR());
    }
    TEST_ASSERT_TRUE(a.getMedianIQR() < -65.0f);  // outliers fenced out
}

static void instances_do_not_share_state() {
    AdaptivePercentileRSSI near, far;
    std::vector<float> n, f;
    for (int i = 0; i < 20; i++) {
        n.push_back(-40.0f - i % 3); near.addMeasurement(n.back());
        f.push_back(-90.0f - i % 3); far.addMeasurement(f.back());
    }
    // Interleave so a stale buffer would leak between them.
    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_EQUAL_FLOAT(reference(n), near.getMedianIQR());
        TEST_ASSERT_EQUAL_FLOAT(reference(f), far.getMedianIQR());
    }
}

static void full_buffer_fits_scratch() {
    AdaptivePercentileRSSI a(15000, 1000);  // oversize request is capped to MAX_READINGS
    std::vector<float> seen;
    for (int i = 0; i < 1000; i++) {
        float r = -60.0f - (i * 37 % 30);
        a.addMeasurement(r);
        seen.push_back(r);
    }
    seen.erase(seen.begin(), seen.end() - a.getReadingCount());
    TEST_ASSERT_TRUE(a.getReadingCount() <= 200);
    TEST_ASSERT_EQUAL_FLOAT(reference(seen), a.getMedianIQR());
}

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(matches_reference_and_rejects_outliers);
    RUN_TEST(instances_do_not_share_state);
    RUN_TEST(full_buffer_fits_scratch);
    return UNITY_END();
}
