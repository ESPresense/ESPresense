#pragma once
#ifdef SENSORS
#include <ArduinoJson.h>

namespace I2C {
    void ConnectToWifi(bool updating = false);
    void SerialReport();
    void Setup();
}

#endif
