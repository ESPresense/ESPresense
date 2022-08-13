#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace I2C {
    void ConnectToWifi();
    void SerialReport();
    void Setup();
}

#endif
