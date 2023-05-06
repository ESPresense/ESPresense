#pragma once

#include <Arduino.h>

#include "BleFingerprint.h"

class BleFingerprint;

namespace GUI {
void Setup(bool beforeWifi);
void SerialReport();
bool SendOnline();
void ConnectToWifi();
bool SendDiscovery();
void Loop();
void Added(BleFingerprint *f);
void Removed(BleFingerprint *f);
void Close(BleFingerprint *f);
void Left(BleFingerprint *f);
void Counting(BleFingerprint *f, bool added);
void Motion(bool pir, bool radar);
void Switch(bool switchone, bool switchtwo);
void Button(bool switchone, bool switchtwo);
void Seen(bool inprogress);
void Update(unsigned int percent);
void Connected(bool wifi, bool mqtt);
void Wifi(unsigned int percent);
void Portal(unsigned int percent);
void Count(unsigned int count);
bool Command(String &command, String &pay);
}  // namespace GUI
