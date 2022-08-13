#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace SHT30
{
    void ConnectToWifi();
    void SerialReport();
    bool SendDiscovery();
    void Setup();
    void Loop();
}

#endif
