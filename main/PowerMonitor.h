#pragma once

// Plug power metering with a BL0937 or HLW8012 (most relay plugs): the "power" setting on the
// hardware endpoint, a JSON object such as
//   {"model": "bl0937", "cf": 6, "cf1": 7, "sel": 10, "voltage_divider": 1517, "current_resistor": 0.001}
// Calibration uses ESPHome's voltage_divider/current_resistor, so values from an ESPHome config
// for the same plug carry over. Publishes power (W), voltage (V), current (A) and energy (kWh,
// kept across reboots) every 10 s.
namespace PowerMonitor {
void ConnectToWifi(bool updating);
void Setup();
void Loop();
bool SendDiscovery();
}  // namespace PowerMonitor
