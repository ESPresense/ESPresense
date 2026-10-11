#pragma once
#include <cstdint>
#include <string>

// GPIO inputs (PIR, radar, switches, buttons): the "inputs" setting on the hardware endpoint, a
// JSON list such as
//   [{"name": "Hallway PIR", "role": "motion", "pin": 4, "type": "pulldown", "timeout": 5}]
// role: motion | switch | button. type: pullup | pulldown | floating, each optionally
// "_inverted" (active LOW). timeout: seconds held ON after the last detection.
// Input n (1-based list position) publishes ON/OFF to <room>/input_<n> and appears in Home
// Assistant as a binary_sensor named after it (device class motion for the motion role).
namespace Inputs {
constexpr int MAX = 8;

enum class Role : uint8_t { Motion = 0, Switch = 1, Button = 2 };

void Setup();
void ConnectToWifi(bool updating);
void SerialReport();
void Loop();
bool SendDiscovery();
bool SendOnline();
// Debounced state of input n (1-based): HIGH, LOW, or -1 before the first read/out of range.
int8_t Value(int n);
// Role of input n; Motion when out of range.
Role GetRole(int n);
}  // namespace Inputs
