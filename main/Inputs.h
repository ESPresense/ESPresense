#pragma once
#include <cstdint>
#include <string>

// GPIO inputs (PIR, radar, switches, buttons): input_count, then input_<n>_name/role/pin/inv/
// pull/timeout for n = 1..count on the hardware endpoint. Each publishes ON/OFF to
// <room>/input_<n>, held ON for its timeout after the last detection, and appears in Home
// Assistant as a binary_sensor (device class motion for the Motion role).
namespace Inputs {
constexpr int MAX = 8;

enum class Role : uint8_t { Motion = 0, Switch = 1, Button = 2 };

void Setup();
void ConnectToWifi(bool updating);
void SerialReport();
void Loop();
bool SendDiscovery();
bool SendOnline();
bool Command(std::string& command, std::string& pay);
// Debounced state of input n (1-based): HIGH, LOW, or -1 before the first read/out of range.
int8_t Value(int n);
// Role of input n; Motion when out of range.
Role GetRole(int n);
}  // namespace Inputs
