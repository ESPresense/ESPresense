#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

// Support for Sensirion SCD4x family sensors (SCD40, SCD41, SCD43)
// These sensors provide CO2, temperature, and humidity measurements via I2C
// All variants use the same I2C address: 0x62
namespace SensirionSCD4x {
void ConnectToWifi();
void SerialReport();
bool SendDiscovery();
void Setup();
void Loop();
}  // namespace SensirionSCD4x

#endif
