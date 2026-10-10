#pragma once
#include <string>

namespace Relay {
constexpr int MAX_RELAYS = 4;

void Setup();
void ConnectToWifi(bool updating);
void SerialReport();
void Loop();
bool SendDiscovery();
bool SendOnline();
bool Command(std::string& command, std::string& pay);
}  // namespace Relay
