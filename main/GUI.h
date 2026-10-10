#pragma once

#include <cstdint>
#include <string>

#include "BleFingerprint.h"

class BleFingerprint;

namespace GUI {
void Setup(bool beforeWifi);
void SerialReport();
bool SendOnline();
void ConnectToWifi(bool updating);
bool SendDiscovery();
void Loop();
void Added(BleFingerprint *f);
void Removed(BleFingerprint *f);
void Close(BleFingerprint *f);
void Left(BleFingerprint *f);
void Counting(BleFingerprint *f, bool added);
void Motion(bool pir, bool radar);
// Bit n-1 of `mask` is set while input n (of `count`) is on.
void Switch(uint32_t mask, int count);
void Button(uint32_t mask, int count);
void Seen(bool inprogress);
void Update(unsigned int percent);
void Connected(bool wifi, bool mqtt);
void Wifi(unsigned int percent);
void Portal(unsigned int percent);
void Count(unsigned int count);
bool Command(std::string &command, std::string &pay);
}  // namespace GUI
