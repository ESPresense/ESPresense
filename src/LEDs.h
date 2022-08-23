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

void Wifi(unsigned int progress);
void Portal(unsigned int progress);
void Connected(bool wifi, bool mqtt);
void Seen(bool inprogress);
void Update(unsigned int progress);
bool Command(String& command, String& pay);
void Counting(bool added);
void Motion(bool pir, bool radar);
void Count(unsigned int count);
}  // namespace LEDs
