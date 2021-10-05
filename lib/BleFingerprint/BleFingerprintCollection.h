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
    void setParams(int rssiRef, int forgetMs, float skipDistance, int skipMs, float maxDistance)
    {
        _refRssi = rssiRef;
        _forgetMs = forgetMs;
        _skipDistance = skipDistance;
        _skipMs = skipMs;
        _maxDistance = maxDistance;
    }
    int getSkipMs() { return _skipMs; }
    float getSkipDistance() { return _skipDistance; }
    int getRefRssi() { return _refRssi; }

private:
    bool _disable = false;

    float _maxDistance, _skipDistance;
    int _refRssi, _forgetMs, _skipMs;
    unsigned long lastCleanup = 0;

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
