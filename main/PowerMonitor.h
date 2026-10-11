#pragma once

// Plug power metering with a BL0937, HLW8012 or CSE7766 (most relay plugs): the "power" setting on the
// hardware endpoint, a JSON object such as
//   {"model": "bl0937", "cf": 6, "cf1": 7, "sel": 10, "sel_inverted": true,
//    "voltage_divider": 1517, "current_resistor": 0.001}
// sel_inverted, voltage_divider and current_resistor mean what they do in ESPHome's hlw8012
// (sel_inverted = "inverted: true" on sel_pin), so values from an ESPHome config for the same
// plug carry over. If voltage and current come out swapped, flip sel_inverted.
// A CSE7766 (Athom plugs) is {"model": "cse7766", "rx": 20} instead: it sends calibrated serial
// frames (4800 8E1) on one pin, so it needs no calibration values. On an ESP32-C3, GPIO 20 is the
// UART console pin, so use the esp32c3-cdc build.
// Publishes power (W), voltage (V), current (A) and energy (kWh, kept across reboots) every 10 s.
namespace PowerMonitor {
void ConnectToWifi(bool updating);
void Setup();
void Loop();
bool SendDiscovery();
}  // namespace PowerMonitor
