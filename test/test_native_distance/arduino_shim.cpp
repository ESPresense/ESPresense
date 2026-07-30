#include "Arduino.h"

namespace {
uint32_t g_mockMillis = 0;
}

uint32_t millis() {
    return g_mockMillis;
}

void mockSetMillis(uint32_t ms) {
    g_mockMillis = ms;
}

void mockAdvanceMillis(uint32_t deltaMs) {
    g_mockMillis += deltaMs;
}
