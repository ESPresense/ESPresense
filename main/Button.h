#pragma once
#include <cstdint>
#include <string>

namespace Button {
void Setup();
void ConnectToWifi(bool updating);
void SerialReport();
void Loop();
bool SendDiscovery();
bool SendOnline();
bool Command(std::string& command, std::string& pay);
// Debounced state of button n (1-based): HIGH (pressed), LOW, or -1 before the first read/disabled.
int8_t Value(int index);
}  // namespace Button
