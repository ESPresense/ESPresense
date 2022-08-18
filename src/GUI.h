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
void Motion(bool pir, bool radar);
void Erase(bool inprogress);
void Seen(bool inprogress);
void Update(bool inprogress);
void UpdateProgress(unsigned int percent);
void Connected(bool wifi, bool mqtt);
void ConnectProgress();
void Status(const char *message, ...);
void Count(BleFingerprint *f, bool added);
bool Command(String &command, String &pay);
}  // namespace GUI
