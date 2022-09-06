#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace SHTC3
{
    void ConnectToWifi();
    void SerialReport();
    bool SendDiscovery();
    void Setup();
    void Loop();
}

#endif
