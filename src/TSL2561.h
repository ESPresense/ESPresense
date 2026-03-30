#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace TSL2561
{
    void Setup();
    void ConnectToWifi(bool updating);
    void SerialReport();
    void Loop();
    bool SendDiscovery();
}

#endif
