#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace AHTX0
{
    void ConnectToWifi(bool updating = false);
    void SerialReport();
    bool SendDiscovery();
    void Setup();
    void Loop();
}

#endif
