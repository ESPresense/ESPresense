#pragma once
#include <Arduino.h>

#define MQTT_STATE_ON_PAYLOAD   "ON"
#define MQTT_STATE_OFF_PAYLOAD  "OFF"

namespace LEDs {
void Setup();
void ConnectToWifi();
void SerialReport();
void Loop();
bool SendDiscovery();
bool SendOnline();
bool Command(String& command, String& pay);

void ConnectProgress();
void Connected(bool wifi, bool mqtt);
void Seen(bool inprogress);
void Update(bool inprogress);
void UpdateProgress(int percent);
bool Command(String& command, String& pay);
}  // namespace LEDs
