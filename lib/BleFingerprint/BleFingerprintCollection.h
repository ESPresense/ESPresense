#ifndef _BLEFINGERPRINTCOLLECTION_
#define _BLEFINGERPRINTCOLLECTION_

#include "BleFingerprint.h"
#include <ArduinoJson.h>

#define ONE_EURO_FCMIN 0.01
#define ONE_EURO_BETA 0.005
#define ONE_EURO_DCUTOFF 1

class BleFingerprintCollection : public BLEAdvertisedDeviceCallbacks
{
public:
    BleFingerprintCollection(int _maxCapacity) : maxCapacity{_maxCapacity}
    {
        fingerprintSemaphore = xSemaphoreCreateBinary();
        xSemaphoreGive(fingerprintSemaphore);
    }
    BleFingerprint *getFingerprint(BLEAdvertisedDevice *advertisedDevice);
    void cleanupOldFingerprints();
    int getTotalAdverts() { return totalSeen; }
    std::list<BleFingerprint *> getSeen();
    void setDisable(bool val) { disable = val; }

private:
    bool disable = false;
    int totalSeen = 0;
    int maxCapacity;
    SemaphoreHandle_t fingerprintSemaphore;
    std::list<BleFingerprint *> fingerprints;
    BleFingerprint *getFingerprintInternal(BLEAdvertisedDevice *advertisedDevice);

    void onResult(BLEAdvertisedDevice *advertisedDevice)
    {
        totalSeen++;
        if (disable) return;

        Display.seenStart();
        BleFingerprint *f = getFingerprint(advertisedDevice);
        f->seen(advertisedDevice);
        Display.seenEnd();
    }
};

#endif
