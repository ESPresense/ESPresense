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
    bool shouldReport();
    StaticJsonDocument<512> report();

    void setCalRssi(int rssi) { calRssi = rssi; }

    NimBLEAddress getAddress() { return address; }

private:
    bool hasValue = false, enroll = false;
    NimBLEAddress address;
    String id, name, url;
    int rssi, calRssi;
    float raw;
    time_t firstSeen;
    time_t lastSeen;

    Reading<Differential<float>> output;

    TimestampFilter<float> tsFilter;
    one_euro_filter<double, unsigned long> oneEuro{one_euro_filter<double, unsigned long>(1, 0.01, 0.01, 1)};
    DifferentialFilter<float> diffFilter;
};
#endif
