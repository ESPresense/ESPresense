#pragma once
#include <string>

namespace Switch {
void Setup();
void ConnectToWifi(bool updating);
void SerialReport();
void Loop();
bool SendDiscovery();
bool SendOnline();
bool Command(std::string& command, std::string& pay);
}  // namespace Switch
