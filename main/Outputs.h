#pragma once
#include <string>

namespace Outputs {
constexpr int MAX = 8;

void Setup();
void ConnectToWifi(bool updating);
void SerialReport();
void Loop();
bool SendDiscovery();
bool SendOnline();
bool Command(std::string& command, std::string& pay);
// State of output n (1-based); false when out of range.
bool State(int n);
}  // namespace Outputs
