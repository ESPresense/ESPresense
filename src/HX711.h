#ifdef SENSORS

#pragma  once

#include <ArduinoJson.h>

// Forward declares
class AsyncMqttClient;

namespace HX711
{

    enum HX711Gain {
        HX711_GAIN_128 = 1,
        HX711_GAIN_32 = 2,
        HX711_GAIN_64 = 3,
    };

    void Setup();
    void ConnectToWifi();
    void SerialReport();
    void Loop(AsyncMqttClient& mqttClient);
    bool SendDiscovery(DynamicJsonDocument& doc);
}

#endif
