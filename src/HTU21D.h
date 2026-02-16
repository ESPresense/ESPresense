#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace HTU21D
{
    void ConnectToWifi(bool updating);
    void SerialReport();
    bool SendDiscovery();
    void Setup();
    void Loop();
}

#endif
