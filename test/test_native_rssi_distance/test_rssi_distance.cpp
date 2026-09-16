// rssiToDistance() replaced pow(10, x) on the per-advertisement path (the FPU-less C3 ran
// newlib's double-precision __ieee754_pow there). It has to stay the same curve.
#include <cmath>

#include "unity.h"
#include "../../components/filtering/AdaptivePercentileRSSI.h"

static void matches_pow10() {
    // refRSSI/pathLoss ranges ESPresense actually configures: ref_rssi -100..0, absorption 1..5.
    for (float ref = -100; ref <= 0; ref += 5)
        for (float rssi = -110; rssi <= 0; rssi += 5)
            for (float loss = 1.0f; loss <= 5.0f; loss += 0.5f) {
                double want = pow(10, (ref - rssi) / (10.0 * loss));
                float got = rssiToDistance(ref, rssi, loss);
                TEST_ASSERT_FLOAT_WITHIN(want * 1e-4, want, got);
            }
}

static void known_points() {
    // At the reference RSSI the device is 1m away, whatever the path loss.
    TEST_ASSERT_FLOAT_WITHIN(1e-5, 1.0f, rssiToDistance(-65, -65, 2.0f));
    // 10 dB weaker than reference with n=2 is 10^0.5.
    TEST_ASSERT_FLOAT_WITHIN(1e-4, 3.1622777f, rssiToDistance(-65, -75, 2.0f));
    // Stronger than reference reads closer than 1m.
    TEST_ASSERT_TRUE(rssiToDistance(-65, -55, 2.0f) < 1.0f);
}

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(matches_pow10);
    RUN_TEST(known_points);
    return UNITY_END();
}
