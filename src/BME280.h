#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace BME280
{
    void Setup();
    void ConnectToWifi(bool updating);
    void SerialReport();
    void Loop();
    bool SendDiscovery();
    bool GetTemperatureHumidity(float &temperature, float &humidity);
}

#endif
