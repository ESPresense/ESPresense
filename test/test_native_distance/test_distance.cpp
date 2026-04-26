// Native (host-side) regression tests for the RSSI -> distance pipeline.
//
// Pinning targets:
//   * lib/rssi_distance/rssi_distance.h    (rssiToLogDistance)
//   * lib/filtering/AdaptivePercentileRSSI (median-IQR / variance math)
//
// Running:  pio test -e native --filter test_native_distance
//
// To add a fixture from a captured RSSI log, see test/fixtures/README.md.

#include <unity.h>

#include <cmath>
#include <cstdio>

#include "Arduino.h"  // host-side shim with mockable millis()

#include "../../lib/rssi_distance/rssi_distance.h"

// Pull AdaptivePercentileRSSI source straight in. Avoids depending on PIO's
// library auto-discovery (disabled in [env:native] anyway) and keeps this
// test binary self-contained.
#include "../../lib/filtering/AdaptivePercentileRSSI.h"
#include "../../lib/filtering/AdaptivePercentileRSSI.cpp"

#include "fixtures.h"

void setUp() {
    mockSetMillis(1000);  // start tests at a non-zero time
}
void tearDown() {}

// ---------------------------------------------------------------------------
// Direct anchor tests for the formula. These are the "if any of these break
// you have changed the model" cases.
// ---------------------------------------------------------------------------

void test_formula_rssi_equals_cal_returns_one_meter() {
    // For ANY absorption, rssi==calRssi must yield exactly 1.0 m.
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 1.0f, rssiToLogDistance(-59.0f, -59.0f, 2.0f));
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 1.0f, rssiToLogDistance(-65.0f, -65.0f, 3.0f));
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 1.0f, rssiToLogDistance(-72.0f, -72.0f, 3.5f));
}

void test_formula_six_db_drop_doubles_distance() {
    // n=2 free-space: 6.02 dB drop ~= 2x distance.
    float d = rssiToLogDistance(-59.0f, -65.02f, 2.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.02f, 2.0f, d);
}

void test_formula_n3_indoor() {
    // n=3 indoor: 9.03 dB drop ~= 2x distance.
    float d = rssiToLogDistance(-59.0f, -68.03f, 3.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.02f, 2.0f, d);
}

void test_formula_stronger_than_cal_means_closer() {
    // rssi > calRssi (positive numerator after sign flip) -> distance < 1 m.
    float d = rssiToLogDistance(-59.0f, -47.0f, 2.0f);
    TEST_ASSERT_TRUE(d < 1.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.005f, 0.2512f, d);
}

void test_formula_absorption_zero_falls_back_to_two() {
    // n=0 must not divide by zero / produce inf or NaN.
    float d = rssiToLogDistance(-59.0f, -65.02f, 0.0f);
    TEST_ASSERT_TRUE(std::isfinite(d));
    TEST_ASSERT_FLOAT_WITHIN(0.02f, 2.0f, d);
}

void test_formula_absorption_negative_falls_back_to_two() {
    float d = rssiToLogDistance(-59.0f, -65.02f, -1.5f);
    TEST_ASSERT_TRUE(std::isfinite(d));
    TEST_ASSERT_FLOAT_WITHIN(0.02f, 2.0f, d);
}

void test_formula_monotonic_in_rssi() {
    // For fixed (calRssi, n>0), distance is strictly decreasing in rssi.
    float prev = rssiToLogDistance(-59.0f, -90.0f, 2.5f);
    for (int rssi = -89; rssi <= -40; ++rssi) {
        float d = rssiToLogDistance(-59.0f, (float)rssi, 2.5f);
        TEST_ASSERT_TRUE_MESSAGE(d < prev,
            "distance must decrease as rssi increases (closer)");
        prev = d;
    }
}

// ---------------------------------------------------------------------------
// Fixture corpus sweep. Every row in kFormulaFixtures is asserted within its
// declared tolerance. Failure messages name the row so a regression points
// straight at the broken case.
// ---------------------------------------------------------------------------
void test_formula_fixture_corpus() {
    for (size_t i = 0; i < kFormulaFixtureCount; ++i) {
        const FormulaFixture& f = kFormulaFixtures[i];
        float actual = rssiToLogDistance(f.calRssi, f.rssi, f.absorption);
        char msg[256];
        std::snprintf(msg, sizeof(msg),
            "[%s | %s] calRssi=%.2f rssi=%.2f n=%.2f -> expected %.4f, got %.4f (%s)",
            f.board, f.name, f.calRssi, f.rssi, f.absorption,
            f.expectedDistance, actual, f.note);
        TEST_ASSERT_FLOAT_WITHIN_MESSAGE(f.toleranceMeters, f.expectedDistance, actual, msg);
    }
}

// ---------------------------------------------------------------------------
// AdaptivePercentileRSSI tests against synthetic RSSI streams.
// Time is advanced manually between samples so the rolling-window logic
// stays inside the 15s default window.
// ---------------------------------------------------------------------------

static void feedStream(AdaptivePercentileRSSI& f, const float* samples, size_t n,
                       uint32_t intervalMs = 200) {
    for (size_t i = 0; i < n; ++i) {
        f.addMeasurement(samples[i]);
        mockAdvanceMillis(intervalMs);
    }
}

void test_filter_empty_returns_zero() {
    AdaptivePercentileRSSI f;
    TEST_ASSERT_EQUAL_FLOAT(0.0f, f.getMedianIQR());
    TEST_ASSERT_EQUAL_FLOAT(0.0f, f.getRSSIVariance());
    TEST_ASSERT_EQUAL_UINT16(0, f.getReadingCount());
}

void test_filter_single_reading_returns_that_value() {
    AdaptivePercentileRSSI f;
    f.addMeasurement(-65.0f);
    // Single sample -> median is the value, IQR=0 fence, survivor is the value.
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -65.0f, f.getMedianIQR());
}

void test_filter_steady_state_passes_through() {
    AdaptivePercentileRSSI f;
    feedStream(f, kSteadyMinus65, sizeof(kSteadyMinus65)/sizeof(float));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -65.0f, f.getMedianIQR());
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0f, f.getRSSIVariance());
}

void test_filter_outlier_is_dropped() {
    AdaptivePercentileRSSI f;
    feedStream(f, kOutlierStream, sizeof(kOutlierStream)/sizeof(float));
    // Tukey fence on a near-constant core makes the -90 fall outside.
    TEST_ASSERT_FLOAT_WITHIN(0.5f, -65.0f, f.getMedianIQR());
}

void test_filter_two_sided_outliers_dropped() {
    AdaptivePercentileRSSI f;
    feedStream(f, kTwoOutlierStream, sizeof(kTwoOutlierStream)/sizeof(float));
    TEST_ASSERT_FLOAT_WITHIN(0.5f, -65.0f, f.getMedianIQR());
}

void test_filter_bimodal_keeps_all_samples() {
    AdaptivePercentileRSSI f;
    feedStream(f, kBimodalStream, sizeof(kBimodalStream)/sizeof(float));
    // Wide IQR -> fence covers everything -> trimmed mean ~ -65.
    TEST_ASSERT_FLOAT_WITHIN(0.5f, -65.0f, f.getMedianIQR());
}

void test_filter_p75_above_median() {
    AdaptivePercentileRSSI f;
    feedStream(f, kDriftingStream, sizeof(kDriftingStream)/sizeof(float));
    float p50 = f.getPercentileRSSI(0.5f);
    float p75 = f.getP75RSSI();
    // For an ascending stream (more negative -> less negative), P75 must be
    // numerically greater than P50 (less negative).
    TEST_ASSERT_TRUE_MESSAGE(p75 > p50, "P75 must be > P50 on ascending RSSI ramp");
}

void test_filter_variance_is_nonnegative() {
    // Sweep a few synthetic streams; variance must never go negative
    // even when computational formula could produce tiny rounding noise.
    AdaptivePercentileRSSI f1;
    feedStream(f1, kSteadyMinus65, sizeof(kSteadyMinus65)/sizeof(float));
    TEST_ASSERT_TRUE(f1.getRSSIVariance() >= 0.0f);

    AdaptivePercentileRSSI f2;
    feedStream(f2, kBimodalStream, sizeof(kBimodalStream)/sizeof(float));
    TEST_ASSERT_TRUE(f2.getRSSIVariance() >= 0.0f);
}

void test_filter_distance_variance_uses_log_distance_curve() {
    // Pump in a stream that straddles the calRssi by symmetric +/- dB. The
    // distance variance should be > 0 (non-flat distance distribution) and
    // finite. Catches regressions where someone changes the formula or the
    // variance sum.
    AdaptivePercentileRSSI f;
    const float symmetric[] = {
        -65, -65, -65, -65, -65, -71, -71, -71, -65, -65,
        -65, -59, -59, -59, -65, -65, -65, -65, -65, -65,
    };
    feedStream(f, symmetric, sizeof(symmetric)/sizeof(float));
    float distVar = f.getDistanceVariance(-65.0f, 2.0f);
    TEST_ASSERT_TRUE(std::isfinite(distVar));
    TEST_ASSERT_TRUE_MESSAGE(distVar > 0.0f, "varying RSSI must yield non-zero distance variance");
}

// Sweep the filter fixtures from fixtures.h.
void test_filter_fixture_corpus() {
    for (size_t i = 0; i < kFilterFixtureCount; ++i) {
        const FilterFixture& fx = kFilterFixtures[i];
        // fresh filter + fresh time origin per fixture
        mockSetMillis(1000);
        AdaptivePercentileRSSI f;
        feedStream(f, fx.samples, fx.sampleCount);
        float actual = f.getMedianIQR();
        char msg[256];
        std::snprintf(msg, sizeof(msg),
            "[%s] expected %.3f dBm +/- %.3f, got %.3f (%s)",
            fx.name, fx.expectedMedianIqr, fx.toleranceDb, actual, fx.note);
        TEST_ASSERT_FLOAT_WITHIN_MESSAGE(fx.toleranceDb, fx.expectedMedianIqr, actual, msg);
    }
}

void test_filter_time_window_drops_expired() {
    AdaptivePercentileRSSI f(1000 /* 1s window */);
    f.addMeasurement(-50.0f);
    f.addMeasurement(-50.0f);
    TEST_ASSERT_TRUE(f.getReadingCount() >= 1);
    mockAdvanceMillis(2000);  // jump past the window
    f.addMeasurement(-70.0f);
    // Lock in the precondition: addMeasurement() must have evicted both
    // expired -50 readings via removeExpiredReadings(). getMedianIQR()
    // does NOT filter by timeWindowMs itself, so without this count
    // assert a future refactor that moves expiration logic out of
    // addMeasurement could break the window contract silently while
    // this test still happened to pass.
    TEST_ASSERT_EQUAL_UINT16(1, f.getReadingCount());
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -70.0f, f.getMedianIQR());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_formula_rssi_equals_cal_returns_one_meter);
    RUN_TEST(test_formula_six_db_drop_doubles_distance);
    RUN_TEST(test_formula_n3_indoor);
    RUN_TEST(test_formula_stronger_than_cal_means_closer);
    RUN_TEST(test_formula_absorption_zero_falls_back_to_two);
    RUN_TEST(test_formula_absorption_negative_falls_back_to_two);
    RUN_TEST(test_formula_monotonic_in_rssi);
    RUN_TEST(test_formula_fixture_corpus);

    RUN_TEST(test_filter_empty_returns_zero);
    RUN_TEST(test_filter_single_reading_returns_that_value);
    RUN_TEST(test_filter_steady_state_passes_through);
    RUN_TEST(test_filter_outlier_is_dropped);
    RUN_TEST(test_filter_two_sided_outliers_dropped);
    RUN_TEST(test_filter_bimodal_keeps_all_samples);
    RUN_TEST(test_filter_p75_above_median);
    RUN_TEST(test_filter_variance_is_nonnegative);
    RUN_TEST(test_filter_distance_variance_uses_log_distance_curve);
    RUN_TEST(test_filter_fixture_corpus);
    RUN_TEST(test_filter_time_window_drops_expired);
    return UNITY_END();
}
