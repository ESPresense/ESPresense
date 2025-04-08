#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace SensirionSCD4x {
void ConnectToWifi();
void SerialReport();
bool SendDiscovery();
void Setup();
void Loop();
}  // namespace SensirionSCD4x

#endif
