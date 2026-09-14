#pragma once
#include <string>

namespace Updater {
void Setup();
void Loop();
void ConnectToWifi(bool updating);
bool SendOnline();
bool SendDiscovery();
bool Command(std::string& command, std::string& pay);
void MarkOtaSuccess();
}  // namespace Updater
