#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace SensirionSGP30 {
void ConnectToWifi(bool updating);
void SerialReport();
bool SendDiscovery();
void Setup();
void Loop();
}  // namespace SensirionSGP30

#endif
