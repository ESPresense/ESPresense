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

    TimestampFilter<float> tsFilter;
    one_euro_filter<double, unsigned long> oneEuro{one_euro_filter<double, unsigned long>(1, 0.01, 0.01, 1)};
    DifferentialFilter<float> diffFilter;

    StaticJsonDocument<500> doc;
    unsigned long lastReading = 0;
};

#endif
