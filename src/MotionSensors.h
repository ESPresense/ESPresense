#pragma  once

#include <ArduinoJson.h>

// Forward declares
class AsyncMqttClient;

namespace Motion
{
    void Setup();
    void ConnectToWifi();
    void SerialReport();
    void Loop(AsyncMqttClient& mqttClient);
    bool SendDiscovery(DynamicJsonDocument& doc);
    bool SendOnline(DynamicJsonDocument& doc);
    bool Command(String& command, String& pay);
}
