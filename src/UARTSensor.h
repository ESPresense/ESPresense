#pragma once
#ifdef SENSORS

namespace UARTSensor {
    void Setup();
    void ConnectToWifi();
    void SerialReport();
    void Loop();
    bool SendDiscovery();
}

#endif
