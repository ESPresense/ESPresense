#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace AHTX0
{
    void ConnectToWifi();
    void SerialReport();
    bool SendDiscovery();
    void Setup();
    void Loop();
}

#endif
