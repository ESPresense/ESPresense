#ifdef SENSORS

#pragma  once

#include <ArduinoJson.h>

// Forward declares
class AsyncMqttClient;

namespace TSL2561
{
    void Setup();
    void ConnectToWifi();
    void SerialReport();
    void Loop(AsyncMqttClient& mqttClient);
    bool SendDiscovery(DynamicJsonDocument& doc);
}

#endif