#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace DHT
{
    void Setup();
    void ConnectToWifi();
    void SerialReport();
    void Loop();
    bool SendDiscovery();
    bool SendOnline();
    bool Command(String& command, String& pay);
}
#endif
