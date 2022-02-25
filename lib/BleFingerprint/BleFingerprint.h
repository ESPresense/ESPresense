#ifndef _BLEFINGERPRINT_
#define _BLEFINGERPRINT_

#include "GUI.h"
#include "rssi.h"
#include "strings.h"
#include <ArduinoJson.h>
#include <NimBLEAdvertisedDevice.h>
#include <NimBLEBeacon.h>
#include <NimBLEDevice.h>
#include <NimBLEEddystoneTLM.h>
#include <NimBLEEddystoneURL.h>
#include <SoftFilters.h>

#define NO_RSSI (-32768)

#define ID_TYPE_TX_POW short(1)

#define ID_TYPE_MAC short(0)
#define ID_TYPE_PUBLIC_MAC short(5)
#define ID_TYPE_AD short(10)
#define ID_TYPE_SD short(15)
#define ID_TYPE_MD short(20)
#define ID_TYPE_MISC_APPLE short(25)
#define ID_TYPE_MISC short(30)
#define ID_TYPE_NAME short(35)
#define ID_TYPE_MSFT short(40)
#define ID_TYPE_SONOS short(45)
#define ID_TYPE_MITHERM short(50)
#define ID_TYPE_MIFIT short(55)
#define ID_TYPE_EXPOSURE short(60)
#define ID_TYPE_ITAG short(65)
#define ID_TYPE_TRACKR short(70)
#define ID_TYPE_TILE short(75)
#define ID_TYPE_MEATER short(80)
#define ID_TYPE_VANMOOF short(85)
#define ID_TYPE_APPLE_NEARBY short(90)
#define ID_TYPE_QUERY_MODEL short(95)
#define ID_TYPE_QUERY_NAME short(100)
#define ID_TYPE_EBEACON short(105)
#define ID_TYPE_ABEACON short(110)
#define ID_TYPE_IBEACON short(115)
#define ID_TYPE_RM_ASST short(120)

class BleFingerprintCollection;

class BleFingerprint
{

public:
    BleFingerprint(const BleFingerprintCollection *parent, NimBLEAdvertisedDevice *advertisedDevice, float fcmin, float beta, float dcutoff);

    bool seen(BLEAdvertisedDevice *advertisedDevice);

    bool report(JsonDocument *doc);

    bool query();

    String getId()
    {
        if (!id.isEmpty() && idType > 10) return id;
        if (macPublic) return getMac();
        if (!id.isEmpty()) return id;
        return getMac();
    }

    bool setId(const String &newId, short int newIdType, const String &newName = "");

    void setInitial(int rssi, float distance);

    String getMac() const { return SMacf(address); }

    String const getDiscriminator() { return disc; }

    float getDistance() const { return output.value.position; }

    int getRssi() const { return rssi; }

    int getNewestRssi() const { return newest; }

    int const get1mRssi();

    NimBLEAddress const getAddress() { return address; }

    long getAge() const { return millis() - lastSeenMillis; };

    bool getAdded() const { return added; };

    bool getIgnore() const { return ignore; };

    bool getAllowQuery() const { return allowQuery; };

    bool getRmAsst() const { return rmAsst; };

    int getSeenCount()
    {
        auto sc = seenCount;
        seenCount = 0;
        return sc;
    }

private:
    void fingerprint(NimBLEAdvertisedDevice *advertisedDevice);

    static bool shouldHide(const String &s);

    bool hasValue = false, added = false, close = false, reported = false, macPublic = false, ignore = false, allowQuery = false, didQuery = false, rmAsst = false, hidden = false, connectable = false;
    NimBLEAddress address;
    String id, name, disc;
    short int idType = 0;
    int rssi = -100, calRssi = NO_RSSI, mdRssi = NO_RSSI, asRssi = NO_RSSI, newest = NO_RSSI, recent = NO_RSSI, oldest = NO_RSSI;
    int qryAttempts = 0, seenCount = 1, qryDelayMillis = 0;
    float raw = 0, lastReported = 0, temp = 0, humidity = 0;
    unsigned long firstSeenMillis, lastSeenMillis = 0, lastReportedMillis = 0, lastQryMillis = 0;
    uint16_t mv = 0;

    Reading<Differential<float>> output;

    OneEuroFilter<float, unsigned long> oneEuro;
    DifferentialFilter<float, unsigned long> diffFilter;

    bool filter();
};

#endif
