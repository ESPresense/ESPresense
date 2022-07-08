#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace I2CScanner {
    void Setup();
    void ConnectToWifi();
    void SerialReport();
    void Loop();
    bool SendDiscovery();
}

#endif
