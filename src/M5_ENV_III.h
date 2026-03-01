#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace M5_ENV_III
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