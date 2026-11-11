#pragma once
#ifdef SENSORS
#ifdef CSI_REFERENCE
// Bounded research integration: unchanged full ESPectre SDK, active gateway
// traffic and its radio policy. Output stays on the log, outside HA discovery.
#else
// Passive WiFi CSI from AP frames already received; never transmits and leaves
// WiFi power save alone (see CONTRIBUTING.md, Product principles).
#endif
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
