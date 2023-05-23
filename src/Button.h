#pragma once
#include <Arduino.h>

namespace Button {
void Setup();
void ConnectToWifi();
void SerialReport();
void Loop();
bool SendDiscovery();
bool SendOnline();
bool Command(String& command, String& pay);
}  // namespace Button
