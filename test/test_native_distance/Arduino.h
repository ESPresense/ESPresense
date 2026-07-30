#ifndef NATIVE_TEST_ARDUINO_SHIM_H
#define NATIVE_TEST_ARDUINO_SHIM_H

// Minimal Arduino.h shim used only by the native distance-calc test.
// The real Arduino runtime is not available when PlatformIO runs
// test/test_native_distance under [env:native]; this shim provides
// just enough surface for AdaptivePercentileRSSI to compile.
//
// The shim's millis() is mockable via mockSetMillis / mockAdvanceMillis
// so tests can drive the time-windowed buffer deterministically.

#include <cstdint>
#include <cstdlib>
#include <cmath>

uint32_t millis();
void mockSetMillis(uint32_t ms);
void mockAdvanceMillis(uint32_t deltaMs);

template <typename T>
static inline T constrain(T value, T low, T high) {
    return value < low ? low : (value > high ? high : value);
}

template <typename A, typename B>
static inline auto min(A a, B b) -> decltype(a < b ? a : b) {
    return a < b ? a : b;
}

template <typename A, typename B>
static inline auto max(A a, B b) -> decltype(a > b ? a : b) {
    return a > b ? a : b;
}

#endif
