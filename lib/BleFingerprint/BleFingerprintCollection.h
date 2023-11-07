#pragma once
#include <ArduinoJson.h>

#include "BleFingerprint.h"

#define ONE_EURO_FCMIN 0.3f   // Respond to changes moderately fast
#define ONE_EURO_BETA 0.002f  // Balance between smoothing and responsiveness
#define ONE_EURO_DCUTOFF 1.0f // Control noise in the derivative

#ifndef ALLOW_BLE_CONTROLLER_RESTART_AFTER_SECS
#define ALLOW_BLE_CONTROLLER_RESTART_AFTER_SECS 1800
#endif

struct DeviceConfig {
    String id;
    String alias;
    String name;
    int8_t calRssi = NO_RSSI;
};

namespace BleFingerprintCollection {

typedef std::function<void(bool)> TCallbackBool;
typedef std::function<void(BleFingerprint *)> TCallbackFingerprint;

void Setup();
void ConnectToWifi();
bool Command(String &command, String &pay);
bool Config(String &id, String &json);

void Close(BleFingerprint *f, bool close);
void Count(BleFingerprint *f, bool counting);
void Seen(BLEAdvertisedDevice *advertisedDevice);
BleFingerprint *GetFingerprint(BLEAdvertisedDevice *advertisedDevice);
void CleanupOldFingerprints();
const std::vector<BleFingerprint *> GetCopy();
bool FindDeviceConfig(const String &id, DeviceConfig &config);

extern TCallbackBool onSeen;
extern TCallbackFingerprint onAdd;
extern TCallbackFingerprint onDel;
extern TCallbackFingerprint onClose;
extern TCallbackFingerprint onLeft;
extern TCallbackFingerprint onCountAdd;
extern TCallbackFingerprint onCountDel;

extern String include, exclude, query, knownMacs, knownIrks, countIds;
extern float skipDistance, maxDistance, absorption, countEnter, countExit;
extern int8_t rxRefRssi, rxAdjRssi, txRefRssi;
extern int forgetMs, skipMs, countMs, requeryMs;
extern std::vector<DeviceConfig> deviceConfigs;
extern std::vector<uint8_t *> irks;
extern std::vector<BleFingerprint *> fingerprints;
}  // namespace BleFingerprintCollection
