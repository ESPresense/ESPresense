#ifndef _BLEFINGERPRINTCOLLECTION_
#define _BLEFINGERPRINTCOLLECTION_

#include "BleFingerprint.h"
#include <ArduinoJson.h>

#define ONE_EURO_FCMIN 1e-8
#define ONE_EURO_BETA 1e-10
#define ONE_EURO_DCUTOFF 1e-8

class BleFingerprintCollection : public BLEAdvertisedDeviceCallbacks
{
public:
    BleFingerprintCollection()
    {
        fingerprintSemaphore = xSemaphoreCreateBinary();
        xSemaphoreGive(fingerprintSemaphore);
    }
    BleFingerprint *getFingerprint(BLEAdvertisedDevice *advertisedDevice);
    void cleanupOldFingerprints();
    std::list<BleFingerprint *> getCopy();
    void setDisable(bool disable) { _disable = disable; }
    void setParams(int rssiRef, int forgetMs, float skipDistance, int skipMs, float maxDistance, String include, String exclude, String query)
    {
        _refRssi = rssiRef;
        _forgetMs = forgetMs;
        _skipDistance = skipDistance;
        _skipMs = skipMs;
        _maxDistance = maxDistance;
        _include = include;
        _exclude = exclude;
        _query = query;
    }
    int getSkipMs() { return _skipMs; }
    float getSkipDistance() { return _skipDistance; }
    int getRefRssi() { return _refRssi; }
    float getMaxDistance() { return _maxDistance; }
    String getInclude() { return _include; }
    String getExclude() { return _exclude; }
    String getQuery() { return _query; }

private:
    bool _disable = false;

    float _maxDistance, _skipDistance;
    int _refRssi, _forgetMs, _skipMs;
    unsigned long lastCleanup = 0;
    String _include, _exclude, _query;

    SemaphoreHandle_t fingerprintSemaphore;
    std::list<BleFingerprint *> fingerprints;
    BleFingerprint *getFingerprintInternal(BLEAdvertisedDevice *advertisedDevice);

    void onResult(BLEAdvertisedDevice *advertisedDevice)
    {
        if (_disable) return;

        Display.seenStart();
        BleFingerprint *f = getFingerprint(advertisedDevice);
        if (f->seen(advertisedDevice))
            Display.added(f);
        Display.seenEnd();
    }
};

#endif
