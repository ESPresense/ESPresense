// BleFingerprint keeps raw RSSI in the filter window and applies the receiver adjustment when
// reading it. That relies on these: shifting every sample by a constant shifts the fenced mean
// by the same amount and leaves the variances unchanged, and distance variance over raw samples
// with the reference shifted by the adjustment equals it over normalized samples.
#include "unity.h"
#include "../../components/filtering/AdaptivePercentileRSSI.cpp"

static const float raw[] = {-62, -65, -66, -64, -70, -75, -63, -90, -66, -65};
static const int n = sizeof(raw) / sizeof(raw[0]);

static void shift_moves_the_mean_and_keeps_the_variance() {
    const float adj = 5;
    AdaptivePercentileRSSI rawWindow(60000), adjusted(60000);
    for (float r : raw) {
        rawWindow.addMeasurement(r);
        adjusted.addMeasurement(r - adj);
    }
    TEST_ASSERT_FLOAT_WITHIN(1e-4, adjusted.getMedianIQR(), rawWindow.getMedianIQR() - adj);
    TEST_ASSERT_FLOAT_WITHIN(1e-4, adjusted.getRSSIVariance(), rawWindow.getRSSIVariance());
}

static void distance_variance_uses_the_shifted_reference() {
    const float adj = 5, ref = -65, loss = 2;
    AdaptivePercentileRSSI rawWindow(60000), adjusted(60000);
    for (float r : raw) {
        rawWindow.addMeasurement(r);
        adjusted.addMeasurement(r - adj);
    }
    float want = adjusted.getDistanceVariance(ref, loss);
    TEST_ASSERT_FLOAT_WITHIN(want * 1e-4, want, rawWindow.getDistanceVariance(ref + adj, loss));
}

static void two_sample_example() {
    // Raw -65 and -75, adjustment +5, reference -65, path loss 2: normalized -70 and -80, so
    // 1.7782794 m and 5.6234133 m, population variance 3.6962636 m^2.
    AdaptivePercentileRSSI w(60000);
    w.addMeasurement(-65);
    w.addMeasurement(-75);
    TEST_ASSERT_FLOAT_WITHIN(1e-3, 3.6962636f, w.getDistanceVariance(-65 + 5, 2));
}

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(shift_moves_the_mean_and_keeps_the_variance);
    RUN_TEST(distance_variance_uses_the_shifted_reference);
    RUN_TEST(two_sample_example);
    return UNITY_END();
}
