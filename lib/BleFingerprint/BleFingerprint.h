#ifndef _BLEFINGERPRINT_
#define _BLEFINGERPRINT_

#include "rssi.h"
#include "string_utils.h"
#include <ArduinoJson.h>
#include <NimBLEAdvertisedDevice.h>
#include <NimBLEBeacon.h>
#include <NimBLEDevice.h>
#include <NimBLEEddystoneTLM.h>
#include <NimBLEEddystoneURL.h>
#include <SoftFilters.h>

#define NO_RSSI (-128)

#define ID_TYPE_TX_POW short(1)

#define NO_ID_TYPE short(0)

#define ID_TYPE_ECHO_LOST short(-10)
#define ID_TYPE_MISC_APPLE short(-5)

#define ID_TYPE_RAND_MAC short(1)
#define ID_TYPE_AD short(10)
#define ID_TYPE_SD short(15)
#define ID_TYPE_MD short(20)
#define ID_TYPE_MISC short(30)
#define ID_TYPE_FINDMY short(32)
#define ID_TYPE_NAME short(35)
#define ID_TYPE_MSFT short(40)
#define ID_TYPE_UNIQUE short(50)
#define ID_TYPE_PUBLIC_MAC short(55)
#define ID_TYPE_SONOS short(105)
#define ID_TYPE_GARMIN short(107)
#define ID_TYPE_MITHERM short(110)
#define ID_TYPE_MIFIT short(115)
#define ID_TYPE_EXPOSURE short(120)
#define ID_TYPE_SMARTTAG short(121)
#define ID_TYPE_ITAG short(125)
#define ID_TYPE_ITRACK short(127)
#define ID_TYPE_NUT short(128)
#define ID_TYPE_FLORA short(129)
#define ID_TYPE_TRACKR short(130)
#define ID_TYPE_TILE short(135)
#define ID_TYPE_MEATER short(140)
#define ID_TYPE_TRACTIVE short(142)
#define ID_TYPE_VANMOOF short(145)
#define ID_TYPE_APPLE_NEARBY short(150)
#define ID_TYPE_QUERY_MODEL short(155)
#define ID_TYPE_QUERY_NAME short(160)
#define ID_TYPE_RM_ASST short(165)
#define ID_TYPE_EBEACON short(170)
#define ID_TYPE_ABEACON short(175)
#define ID_TYPE_IBEACON short(180)
#define ID_TYPE_KNOWN_IRK short(200)
#define ID_TYPE_KNOWN_MAC short(210)
#define ID_TYPE_ALIAS short(250)

class BleFingerprint
{

public:
    BleFingerprint(NimBLEAdvertisedDevice *advertisedDevice, float fcmin, float beta, float dcutoff);

    bool seen(BLEAdvertisedDevice *advertisedDevice);

    void fill(JsonObject *doc);

    bool report(JsonObject *doc);

    bool query();

    String getId() { return id; }

    String getName() { return name; }

    void setName(String &name) { this->name = name; }

    bool setId(const String &newId, short int newIdType, const String &newName = "");

    void setInitial(int rssi, float distance);

    String getMac() const { return SMacf(address); }

    short getIdType() const { return idType; }

    String const getDiscriminator() { return disc; }

    float getDistance() const { return output.value.position; }

    int getRssi() const { return rssi; }

    int getNewestRssi() const { return newest; }

    int get1mRssi() const;

    void set1mRssi(int8_t rssi) { this->calRssi = rssi; }

    NimBLEAddress const getAddress() { return address; }

    unsigned long getMsSinceLastSeen() const { return lastSeenMillis ? millis() - lastSeenMillis : 4294967295; };

    unsigned long getMsSinceFirstSeen() const { return millis() - firstSeenMillis; };

    bool getVisible() const { return !ignore && !hidden && hasValue; }

    bool getAdded() const { return added; };

    bool getIgnore() const { return ignore; };

    bool getAllowQuery() const { return allowQuery; };

    bool getRmAsst() const { return rmAsst; };

    unsigned int getSeenCount()
    {
        auto sc = seenCount - lastSeenCount;
        lastSeenCount = seenCount;
        return sc;
    }

    bool shouldCount();
    void fingerprintAddress();

    void expire();

private:

    static bool shouldHide(const String &s);

    bool hasValue = false, added = false, close = false, reported = false, ignore = false, allowQuery = false, didQuery = false, rmAsst = false, hidden = false, connectable = false, countable = false, counting = false;
    NimBLEAddress address;
    String id, name, disc;
    short int idType = NO_ID_TYPE;
    int rssi = NO_RSSI, newest = NO_RSSI, recent = NO_RSSI, oldest = NO_RSSI;
    int8_t calRssi = NO_RSSI, bcnRssi = NO_RSSI, mdRssi = NO_RSSI, asRssi = NO_RSSI;
    unsigned int qryAttempts = 0, qryDelayMillis = 0;
    float raw = 0, lastReported = 0, temp = 0, humidity = 0;
    unsigned long firstSeenMillis, lastSeenMillis = 0, lastReportedMillis = 0, lastQryMillis = 0;
    unsigned long seenCount = 1, lastSeenCount = 0;
    uint16_t mv = 0;
    uint8_t battery = 0xFF, addressType = 0xFF;

    Reading<Differential<float>> output;

    OneEuroFilter<float, unsigned long> oneEuro;
    DifferentialFilter<float, unsigned long> diffFilter;

    bool filter();

    void fingerprint(NimBLEAdvertisedDevice *advertisedDevice);
    void fingerprintServiceAdvertisements(NimBLEAdvertisedDevice *advertisedDevice, size_t serviceAdvCount, bool haveTxPower, int8_t txPower);
    void fingerprintServiceData(NimBLEAdvertisedDevice *advertisedDevice, size_t serviceDataCount, bool haveTxPower, int8_t txPower);
    void fingerprintManufactureData(NimBLEAdvertisedDevice *advertisedDevice, bool haveTxPower, int8_t txPower);
};

#endif
