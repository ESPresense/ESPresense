#ifndef _BLEFINGERPRINT_
#define _BLEFINGERPRINT_

#include <ArduinoJson.h>
#include <NimBLEAdvertisedDevice.h>
#include <NimBLEBeacon.h>
#include <NimBLEDevice.h>
#include <NimBLEEddystoneTLM.h>
#include <NimBLEEddystoneURL.h>
#include <SoftFilters.h>

#define Sprintf(f, ...) (             \
    {                                 \
        char *s;                      \
        asprintf(&s, f, __VA_ARGS__); \
        String r = s;                 \
        free(s);                      \
        r;                            \
    })
#define SMacf(f) (                                                                                                                                       \
    {                                                                                                                                                    \
        auto nativeAddress = f.getNative();                                                                                                              \
        Sprintf("%02x%02x%02x%02x%02x%02x", nativeAddress[5], nativeAddress[4], nativeAddress[3], nativeAddress[2], nativeAddress[1], nativeAddress[0]); \
    })

class BleFingerprint
{

public:
    BleFingerprint(BLEAdvertisedDevice *advertisedDevice, float fcmin, float beta, float dcutoff);

    void seen(BLEAdvertisedDevice *advertisedDevice);
    bool report(JsonDocument *doc, int maxDistance);

    String getId()
    {
        if (!pid.isEmpty()) return pid;
        if (macPublic) return getMac();
        if (!sid.isEmpty()) return sid;
        return getMac();
    }
    String getMac() { return SMacf(address); }

    float getDistance() { return output.value.position; }
    int getRSSI() { return rssi; }

    void setInitial(int rssi, float distance);

    NimBLEAddress getAddress() { return address; }
    long getLastSeen() { return lastSeenMicros; };

private:
    void fingerprint(BLEAdvertisedDevice *advertisedDevice);

    bool hasValue = false, close = false, reported = false, macPublic = false;
    NimBLEAddress address;
    String pid, sid, name, url;
    int rssi = -100, calRssi = 0;
    int newest = -100;
    int recent = -100;
    int oldest = -100;
    float raw, lastReported, temp = 0;
    long firstSeenMicros, lastSeenMicros = 0, lastReportedMicros = 0;
    uint16_t volts;

    Reading<Differential<float>> output;

    TimestampFilter<float> tsFilter;
    one_euro_filter<double, unsigned long> oneEuro;
    DifferentialFilter<float> diffFilter;

    bool filter();
};
#endif
