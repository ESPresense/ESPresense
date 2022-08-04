#ifndef _BLEFINGERPRINTCOLLECTION_
#define _BLEFINGERPRINTCOLLECTION_

#include "BleFingerprint.h"
#include <ArduinoJson.h>

#define ONE_EURO_FCMIN 1e-5f
#define ONE_EURO_BETA 1e-7f
#define ONE_EURO_DCUTOFF 1e-5f

#ifndef ALLOW_BLE_CONTROLLER_RESTART_AFTER_SECS
#define ALLOW_BLE_CONTROLLER_RESTART_AFTER_SECS 1800
#endif

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
    const std::list<BleFingerprint *>* const getNative();
    const std::list<BleFingerprint *> getCopy();
    void setDisable(bool disable) { _disable = disable; }
    void connectToWifi();
    bool command(String& command, String& pay);
    bool config(String& id, String& json);
    static std::vector<std::pair<uint8_t*,String>> irks;
    static String knownMacs, knownIrks, include, exclude, query;
    static float skipDistance, maxDistance, absorption;
    static int refRssi, forgetMs, skipMs;
    static String countIds;
    static float countEnter, countExit;
    static int countMs;

private:
    bool _disable = false;

    unsigned long lastCleanup = 0;

    SemaphoreHandle_t fingerprintSemaphore;
    std::list<BleFingerprint *> fingerprints;
    BleFingerprint *getFingerprintInternal(BLEAdvertisedDevice *advertisedDevice);

    void onResult(BLEAdvertisedDevice *advertisedDevice) override
    {
        if (_disable) return;

        BLEAdvertisedDevice copy = *advertisedDevice;

        GUI::seenStart();
        BleFingerprint *f = getFingerprint(&copy);
        if (f->seen(&copy))
            GUI::added(f);
        GUI::seenEnd();
    }
};

#endif
