#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace MQ135
{
    void Setup();
    void ConnectToWifi();
    void SerialReport();
    void Loop();
    bool SendDiscovery();
}

#endif