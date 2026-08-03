#pragma once
// Minimal Arduino shim so AdaptivePercentileRSSI can be unit-tested on the native platform.
#include <cstdint>
#include <algorithm>
#include <cmath>
extern uint32_t g_now;
inline uint32_t millis() { return g_now; }
using std::min;
using std::max;
using std::ceil;
using std::abs;
template <typename T>
inline T constrain(T x, T lo, T hi) { return x < lo ? lo : (x > hi ? hi : x); }
