#include "BleFingerprintCollection.h"

void BleFingerprintCollection::cleanupOldFingerprints()
{
    auto it = fingerprints.begin();
    while (it != fingerprints.end())
    {
        auto item = (*it);
        long age = item->getAge();
        if (age > _forgetMs)
        {
            Display.removed(item, age);
            it = fingerprints.erase(it);
            delete item;
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

std::list<BleFingerprint *> BleFingerprintCollection::getSeen()
{
    if (xSemaphoreTake(fingerprintSemaphore, 1000) != pdTRUE)
        log_e("Couldn't take semaphore!");
    cleanupOldFingerprints();
    std::list<BleFingerprint *> seen(fingerprints);
    if (xSemaphoreGive(fingerprintSemaphore) != pdTRUE)
        log_e("Couldn't give semaphore!");
    return seen;
}
