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
}

#endif
