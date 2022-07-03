#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

// Forward declares
class AsyncMqttClient;

namespace BH1750
{
    void Setup();
    void ConnectToWifi();
    void SerialReport();
    void Loop();
    bool SendDiscovery();
}

#endif
