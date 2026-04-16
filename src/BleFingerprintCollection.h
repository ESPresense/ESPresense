#pragma once
#include <ArduinoJson.h>

#include "BleFingerprint.h"

#define ONE_EURO_FCMIN 1e-1f
#define ONE_EURO_BETA 1e-3f
#define ONE_EURO_DCUTOFF 5e-3f

#ifndef ALLOW_BLE_CONTROLLER_RESTART_AFTER_SECS
#define ALLOW_BLE_CONTROLLER_RESTART_AFTER_SECS 1800
#endif

struct DeviceConfig {
    String id;
    String alias;
    String name;
    int8_t calRssi = NO_RSSI;
    bool allowConnect = false;
};

namespace BleFingerprintCollection {

typedef std::function<void(bool)> TCallbackBool;
typedef std::function<void(BleFingerprint *)> TCallbackFingerprint;

struct FingerprintLease {
    BleFingerprint *fingerprint = nullptr;
    size_t slot = static_cast<size_t>(-1);

    FingerprintLease() = default;
    FingerprintLease(BleFingerprint *fingerprint, size_t slot) : fingerprint(fingerprint), slot(slot) {}

    explicit operator bool() const { return fingerprint != nullptr; }
};

void Setup();
void ConnectToWifi(bool updating);
bool Command(String &command, String &pay);
bool Config(String &id, String &json);

void Close(BleFingerprint *f, bool close);
void Count(BleFingerprint *f, bool counting);
#ifdef NIMBLE_V2
void Seen(const NimBLEAdvertisedDevice *advertisedDevice);
FingerprintLease GetFingerprint(const NimBLEAdvertisedDevice *advertisedDevice);
#else
void Seen(BLEAdvertisedDevice *advertisedDevice);
FingerprintLease GetFingerprint(BLEAdvertisedDevice *advertisedDevice);
#endif
void CleanupOldFingerprints();
FingerprintLease AcquireNext(size_t &cursor, bool cleanup = true);
void Release(FingerprintLease &lease);
size_t Size(bool cleanup = true);
bool FindDeviceConfig(const String &id, DeviceConfig &config);
bool FindDeviceConfigByAlias(const String &alias, DeviceConfig &config);

extern TCallbackBool onSeen;
extern TCallbackFingerprint onAdd;
extern TCallbackFingerprint onDel;
extern TCallbackFingerprint onClose;
extern TCallbackFingerprint onLeft;
extern TCallbackFingerprint onCountAdd;
extern TCallbackFingerprint onCountDel;

extern String include, exclude, query, knownMacs, knownIrks, countIds;
extern float skipDistance, maxDistance, absorption, countEnter, countExit;
extern int8_t rxRefRssi, rxAdjRssi, txRefRssi, maxDivisor;
extern int forgetMs, skipMs, countMs, requeryMs, maxFingerprints;
extern bool allowConnectAll;
extern std::vector<DeviceConfig> deviceConfigs;
extern std::vector<uint8_t *> irks;
}  // namespace BleFingerprintCollection
