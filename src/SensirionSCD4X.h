#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace SensirionSCD4X {
void ConnectToWifi();
void SerialReport();
bool SendDiscovery();
void Setup();
void Loop();
}  // namespace SensirionSGP30

#endif
