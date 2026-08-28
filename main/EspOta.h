#pragma once
#include <cstdint>
// Native implementation of the espota protocol ArduinoOTA speaks, so `espota.py` /
// `pio run -t upload --upload-port <ip>` keep working as the no-serial fallback.
namespace EspOta {
void Start(uint16_t port = 3232);  // idempotent
}
