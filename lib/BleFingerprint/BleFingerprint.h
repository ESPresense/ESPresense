#ifndef _BLEFINGERPRINT_
#define _BLEFINGERPRINT_

#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include <NimBLEEddystoneURL.h>
#include <NimBLEEddystoneTLM.h>
#include <NimBLEBeacon.h>
#include <ArduinoJson.h>
#include <SoftFilters.h>

class BleFingerprint
{

public:
    BleFingerprint(BLEAdvertisedDevice *advertisedDevice, float initalDistance);

    void seen(BLEAdvertisedDevice *advertisedDevice);
    void report(BLEAdvertisedDevice *advertisedDevice);

    void setCalRssi(int rssi);

    StaticJsonDocument<500> getJson();

private:
    String id;
    int rssi, calRssi;
    float raw;
    Reading<Differential<float>> output;

    FilterChain compositeFilter;
    TimestampFilter<float> tsFilter;
    MovingAverageFilter<float, float> maFilter{MovingAverageFilter<float, float>(3)};
    WeightedUpdateFilter<float, float> wuFilter{WeightedUpdateFilter<float, float>(0.1)};
    OneEuroFilter<float, unsigned long> oneEuro{OneEuroFilter<float, unsigned long>(1, 1, 0.007, 1)};
    DifferentialFilter<float> diffFilter;

    StaticJsonDocument<500> doc;
    unsigned long lastReading = 0;
};

#endif
