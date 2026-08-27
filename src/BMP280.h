#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace BMP280
{
    void ConnectToWifi(bool updating);
    void SerialReport();
    bool SendDiscovery();
    void Setup();
    void Loop();
}

#endif
