#ifndef RSSI_DISTANCE_H
#define RSSI_DISTANCE_H

#include <math.h>

// Log-distance path-loss model used to convert a smoothed RSSI
// observation into an estimated distance in meters.
//
//   d = 10 ^ ((calRssi - rssi) / (10 * absorption))
//
// calRssi    measured RSSI at 1 m (the per-device "tx power" reference)
// rssi       smoothed RSSI observation in dBm
// absorption path-loss exponent / environmental absorption (typ. 2.0..4.0)
//
// This is the canonical implementation. Any code emitting a "distance"
// value MUST call this function so the math has exactly one definition
// the regression suite locks down (see test/test_native_distance/).
static inline float rssiToLogDistance(float calRssi, float rssi, float absorption) {
    // absorption guard: a 0 or negative path-loss exponent is meaningless;
    // fall back to free-space n=2.0 rather than div-by-zero / NaN distance.
    const float n = (absorption > 0.0f) ? absorption : 2.0f;
    return powf(10.0f, (calRssi - rssi) / (10.0f * n));
}

#endif
