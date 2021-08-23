#ifndef _BLEFINGERPRINT_
#define _BLEFINGERPRINT_

#include <ArduinoJson.h>
#include <GUI.h>
#include <NimBLEAdvertisedDevice.h>
#include <NimBLEBeacon.h>
#include <NimBLEDevice.h>
#include <NimBLEEddystoneTLM.h>
#include <NimBLEEddystoneURL.h>
#include <SoftFilters.h>

class BleFingerprint
{

public:
    BleFingerprint(BLEAdvertisedDevice *advertisedDevice, float fcmin, float beta, float dcutoff);
    ~BleFingerprint();

    void seen(BLEAdvertisedDevice *advertisedDevice);
    bool report(JsonDocument *doc, int maxDistance);

    String getId() { return id; }

    float getDistance() { return output.value.position; }
    int getRSSI() { return rssi; }

    void setInitial(int rssi, float distance);

    NimBLEAddress getAddress() { return address; }
    long getLastSeen() { return lastSeenMicros; };

private:
    bool hasValue = false, close = false, reported = false;
    NimBLEAddress address;
    String id, name, url;
    int rssi = -100, calRssi = 0;
    int newest = -100;
    int recent = -100;
    int oldest = -100;
    float raw, lastReported = 0;
    long firstSeenMicros, lastSeenMicros = 0, lastReportedMicros = 0;

    Reading<Differential<float>> output;

    TimestampFilter<float> tsFilter;
    one_euro_filter<double, unsigned long> oneEuro;
    DifferentialFilter<float> diffFilter;

    bool filter();
};
#endif
