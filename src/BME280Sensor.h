#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace BME280
{
    void Setup();
    void ConnectToWifi();
    void SerialReport();
    void Loop();
    bool SendDiscovery();
}

#endif
