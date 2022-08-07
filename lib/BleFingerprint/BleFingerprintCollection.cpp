#include "BleFingerprintCollection.h"
#include <sstream>

/* Static variables */
String BleFingerprintCollection::include{}, BleFingerprintCollection::exclude{}, BleFingerprintCollection::query{}, BleFingerprintCollection::knownMacs{}, BleFingerprintCollection::knownIrks{}, BleFingerprintCollection::countIds{};
float BleFingerprintCollection::skipDistance = 0.0f, BleFingerprintCollection::maxDistance = 0.0f, BleFingerprintCollection::absorption = 3.5f, BleFingerprintCollection::countEnter = 2, BleFingerprintCollection::countExit = 4;
int BleFingerprintCollection::refRssi = 0, BleFingerprintCollection::forgetMs = 0, BleFingerprintCollection::skipMs = 0, BleFingerprintCollection::countMs = 10000;
std::vector<DeviceConfig> BleFingerprintCollection::deviceConfigs;
std::vector<uint8_t *> BleFingerprintCollection::irks;

bool BleFingerprintCollection::config(String &id, String &json) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);

    DeviceConfig config = {};
    config.id = id;
    auto alias = doc["id"].as<String>();
    if (alias != id) config.alias = alias;
    config.calRssi = doc["rssi@1m"];
    config.name = doc["name"].as<String>();
    deviceConfigs.push_back(config);

    auto p = id.indexOf("irk:");
    if (p == 0) {
        auto irk_hex = id.substring(4);
        uint8_t *irk = new uint8_t[16];
        if (!hextostr(irk_hex, irk, 16))
            return false;
        irks.push_back(irk);

        for (auto it = std::begin(fingerprints); it != std::end(fingerprints); ++it)
            (*it)->fingerprintAddress();
    }

    return true;
}

void BleFingerprintCollection::connectToWifi() {
    std::istringstream iss(BleFingerprintCollection::knownIrks.c_str());
    std::string irk_hex;
    while (iss >> irk_hex) {
        uint8_t *irk = new uint8_t[16];
        if (!hextostr(irk_hex.c_str(), irk, 16))
            continue;
        irks.push_back(irk);
    }
}

bool BleFingerprintCollection::command(String &command, String &pay) {

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
    bool any = false;
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
            any = true;
            ++it;
        }
    }
    if (!any) {
        auto uptime = (unsigned long)(esp_timer_get_time() / 1000000ULL);
        if (uptime > ALLOW_BLE_CONTROLLER_RESTART_AFTER_SECS) {
            Serial.println("Bluetooth controller seems stuck, restarting");
            ESP.restart();
        }
    }
}

BleFingerprint *BleFingerprintCollection::getFingerprintInternal(BLEAdvertisedDevice *advertisedDevice)
{
    auto mac = advertisedDevice->getAddress();

    auto it = std::find_if(fingerprints.rbegin(), fingerprints.rend(), [mac](BleFingerprint *f) { return f->getAddress() == mac; });
    if (it != fingerprints.rend())
        return *it;

    auto created = new BleFingerprint(this, advertisedDevice, ONE_EURO_FCMIN, ONE_EURO_BETA, ONE_EURO_DCUTOFF);
    auto it2 = std::find_if(fingerprints.begin(), fingerprints.end(), [created](BleFingerprint *f) { return f->getId() == created->getId(); });
    if (it2 != fingerprints.end())
    {
        auto found = *it2;
        //Serial.printf("Detected mac switch for fingerprint id %s\n", found->getId().c_str());
        created->setInitial(found->getRssi(), found->getDistance());
        if (found->getIdType()>ID_TYPE_UNIQUE)
            found->expire();
    }

    fingerprints.push_back(created);
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

const std::vector<BleFingerprint *> BleFingerprintCollection::getCopy() {
    if (xSemaphoreTake(fingerprintSemaphore, 1000) != pdTRUE)
        log_e("Couldn't take semaphore!");
    cleanupOldFingerprints();
    std::vector<BleFingerprint *> copy(fingerprints);
    if (xSemaphoreGive(fingerprintSemaphore) != pdTRUE)
        log_e("Couldn't give semaphore!");
    return std::move(copy);
}

const std::vector<BleFingerprint *> *const BleFingerprintCollection::getNative() { return &fingerprints; }

std::vector<uint8_t *> BleFingerprintCollection::getIrks() { return irks; }

bool BleFingerprintCollection::findDeviceConfig(const String &id, DeviceConfig &config) {
    auto it = std::find_if(deviceConfigs.begin(), deviceConfigs.end(), [id](DeviceConfig dc) { return dc.id == id; });
    if (it == deviceConfigs.end()) return false;
    config = (*it);
    return true;
}

std::vector<DeviceConfig> BleFingerprintCollection::getConfigs()
{
    return deviceConfigs;
}
