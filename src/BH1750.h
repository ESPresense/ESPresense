#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

// Forward declares
class AsyncMqttClient;

namespace BH1750
{
    void Setup();
    void ConnectToWifi(bool updating = false);
    void SerialReport();
    void Loop();
    bool SendDiscovery();
}

#endif
