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
    BleFingerprint(BLEAdvertisedDevice *advertisedDevice);
    ~BleFingerprint();

    void seen(BLEAdvertisedDevice *advertisedDevice);
    bool report(JsonDocument *doc);

    String getId() { return id; }

    float getDistance();
    void setDistance(float distFl);

    NimBLEAddress getAddress() { return address; }
    void setAddress(NimBLEAddress newAddr) { address = newAddr; }

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
