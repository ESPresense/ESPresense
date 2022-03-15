#pragma  once

#include <ArduinoJson.h>

// Forward declares
class AsyncMqttClient;

namespace BME280
{
    void Setup();
    void ConnectToWifi();
    void SerialReport();
    void Loop(AsyncMqttClient& mqttClient);
    bool SendDiscovery(DynamicJsonDocument& doc);
}