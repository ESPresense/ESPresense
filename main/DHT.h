#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>
#include <string>

namespace DHT
{
    void Setup();
    void ConnectToWifi(bool updating);
    void SerialReport();
    void Loop();
    bool SendDiscovery();
    bool SendOnline();
    bool Command(std::string& command, std::string& pay);
}
#endif
