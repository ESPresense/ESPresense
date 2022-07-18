#include "BleFingerprintCollection.h"
#include <sstream>

/* Static variables */
String BleFingerprintCollection::include{}, BleFingerprintCollection::exclude{}, BleFingerprintCollection::query{}, BleFingerprintCollection::knownMacs{}, BleFingerprintCollection::knownIrks{}, BleFingerprintCollection::countIds{};
float BleFingerprintCollection::skipDistance = 0.0f, BleFingerprintCollection::maxDistance = 0.0f, BleFingerprintCollection::absorption = 3.5f, BleFingerprintCollection::countEnter = 2, BleFingerprintCollection::countExit = 4;
int BleFingerprintCollection::refRssi = 0, BleFingerprintCollection::forgetMs = 0, BleFingerprintCollection::skipMs = 0, BleFingerprintCollection::countMs = 10000;
std::vector<std::pair<uint8_t*,String>> BleFingerprintCollection::irks;

bool BleFingerprintCollection::config(String& id, String& json) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);
    auto p = id.indexOf("irk:");
    if (p == 0) {
        auto irk_hex = id.substring(4);
        uint8_t* irk = new uint8_t[16];
        if (!hextostr(irk_hex, irk, 16))
            return false;
        irks.push_back({irk, doc["id"]});

        for(auto it = std::begin(fingerprints); it != std::end(fingerprints); ++it)
            (*it)->fingerprintAddress();
    }
    return true;
}

void BleFingerprintCollection::connectToWifi() {
    std::istringstream iss(BleFingerprintCollection::knownIrks.c_str());
    std::string irk_hex;
    while (iss >> irk_hex) {
        uint8_t* irk = new uint8_t[16];
        if (!hextostr(irk_hex.c_str(), irk, 16))
            continue;
        irks.push_back({irk, String("irk:") + irk_hex.c_str()});
    }
}

bool BleFingerprintCollection::command(String& command, String& pay) {

    if (command == "max_distance")
    {
        BleFingerprintCollection::maxDistance = pay.toFloat();
        spurt("/max_dist", pay);
    }
    else if (command == "absorption")
    {
        BleFingerprintCollection::absorption = pay.toFloat();
        spurt("/absorption", pay);
    }
    else if (command == "query")
    {
        BleFingerprintCollection::query = pay;
        spurt("/query", pay);
    }
    else if (command == "include")
    {
        BleFingerprintCollection::include = pay;
        spurt("/include", pay);
    }
    else if (command == "exclude")
    {
        BleFingerprintCollection::exclude = pay;
        spurt("/exclude", pay);
    }
    else if (command == "known_macs")
    {
        BleFingerprintCollection::knownMacs = pay;
        spurt("/known_macs", pay);
    }
    else if (command == "known_irks")
    {
        BleFingerprintCollection::knownIrks = pay;
        spurt("/known_irks", pay);
    }
    else if (command == "count_ids")
    {
        BleFingerprintCollection::countIds = pay;
        spurt("/count_ids", pay);
    } else
        return false;
    return true;
}

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

const std::list<BleFingerprint *> BleFingerprintCollection::getCopy()
{
    if (xSemaphoreTake(fingerprintSemaphore, 1000) != pdTRUE)
        log_e("Couldn't take semaphore!");
    cleanupOldFingerprints();
    std::list<BleFingerprint *> copy(fingerprints);
    if (xSemaphoreGive(fingerprintSemaphore) != pdTRUE)
        log_e("Couldn't give semaphore!");
    return std::move(copy);
}

const std::list<BleFingerprint *>* const BleFingerprintCollection::getNative()
{
    return &fingerprints;
}
