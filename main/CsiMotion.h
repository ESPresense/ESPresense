#pragma once
#ifdef SENSORS
// Passive WiFi CSI: a room-movement reading from the AP's frames this node receives anyway.
// Never transmits and leaves WiFi power save alone (see CONTRIBUTING.md, Product principles).
namespace CsiMotion
{
#ifdef CSI_REFERENCE
    void NoteAdvert();
#endif
    void ConnectToWifi(bool updating);
    void SerialReport();
    void Loop();
    bool SendDiscovery();
    void SendTelemetry();
}

#endif
