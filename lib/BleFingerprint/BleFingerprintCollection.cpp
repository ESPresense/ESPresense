#include "BleFingerprintCollection.h"

void BleFingerprintCollection::cleanupOldFingerprints()
{
    auto now = millis();
    if (now - lastCleanup < 5000) return;
    lastCleanup = now;
    auto it = fingerprints.begin();
    while (it != fingerprints.end())
    {
        auto age = (*it)->getMsSinceLastSeen();
        if (age > forgetMs)
        {
            GUI::removed((*it));
            delete *it;
            it = fingerprints.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

BleFingerprint *BleFingerprintCollection::getFingerprintInternal(BLEAdvertisedDevice *advertisedDevice)
{
    auto mac = advertisedDevice->getAddress();

    auto it = std::find_if(fingerprints.begin(), fingerprints.end(), [mac](BleFingerprint *f)
                           { return f->getAddress() == mac; });
    if (it != fingerprints.end())
        return *it;

    auto created = new BleFingerprint(this, advertisedDevice, ONE_EURO_FCMIN, ONE_EURO_BETA, ONE_EURO_DCUTOFF);
    auto it2 = std::find_if(fingerprints.begin(), fingerprints.end(), [created](BleFingerprint *f)
                            { return f->getId() == created->getId(); });
    if (it2 != fingerprints.end())
    {
        auto found = *it2;
        created->setInitial(found->getRssi(), found->getDistance());
    }

    fingerprints.push_front(created);
    return created;
}

BleFingerprint *BleFingerprintCollection::getFingerprint(BLEAdvertisedDevice *advertisedDevice)
{
    if (xSemaphoreTake(fingerprintSemaphore, 1000) != pdTRUE)
        log_e("Couldn't take semaphore!");
    auto f = getFingerprintInternal(advertisedDevice);
    if (xSemaphoreGive(fingerprintSemaphore) != pdTRUE)
        log_e("Couldn't give semaphore!");
    return f;
}

std::list<BleFingerprint *> BleFingerprintCollection::getCopy()
{
    if (xSemaphoreTake(fingerprintSemaphore, 1000) != pdTRUE)
        log_e("Couldn't take semaphore!");
    cleanupOldFingerprints();
    std::list<BleFingerprint *> copy(fingerprints);
    if (xSemaphoreGive(fingerprintSemaphore) != pdTRUE)
        log_e("Couldn't give semaphore!");
    return copy;
}
String BleFingerprintCollection::include{}, BleFingerprintCollection::exclude{}, BleFingerprintCollection::query{}, BleFingerprintCollection::knownMacs{}, BleFingerprintCollection::countIds{};
float BleFingerprintCollection::skipDistance = 0.0f, BleFingerprintCollection::maxDistance = 0.0f, BleFingerprintCollection::absorption = 3.5f, BleFingerprintCollection::countEnter = 2, BleFingerprintCollection::countExit = 4;
int BleFingerprintCollection::refRssi = 0, BleFingerprintCollection::forgetMs = 0, BleFingerprintCollection::skipMs = 0, BleFingerprintCollection::countMs = 10000;
