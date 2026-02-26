#pragma once

#include <Arduino.h>

namespace Relay {
void Setup();
void ConnectToWifi();
void SerialReport();
void Loop();
bool Command(String &command, String &pay);
bool SendDiscovery();
bool SendOnline();
}  // namespace Relay
