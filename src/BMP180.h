#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace BMP180
{
    void ConnectToWifi();
    void SerialReport();
    bool SendDiscovery();
    void Setup();
    void Loop();
}

#endif
