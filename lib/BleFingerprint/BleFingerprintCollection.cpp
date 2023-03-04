#include "BleFingerprintCollection.h"

#include <sstream>
#include <Arduino.h>

namespace BleFingerprintCollection {
// Public (externed)
String include{}, exclude{}, query{}, knownMacs{}, knownIrks{}, countIds{};
float skipDistance = 0.0f, maxDistance = 0.0f, absorption = 3.5f, countEnter = 2, countExit = 4;
int8_t rxRefRssi = -65, rxAdjRssi = 0, txRefRssi = -59;
int forgetMs = 0, skipMs = 0, countMs = 10000;
std::vector<DeviceConfig> deviceConfigs;
std::vector<uint8_t *> irks;
std::vector<BleFingerprint *> fingerprints;
TCallbackBool onSeen = nullptr;
TCallbackFingerprint onAdd = nullptr;
TCallbackFingerprint onDel = nullptr;
TCallbackFingerprint onClose = nullptr;
TCallbackFingerprint onLeft = nullptr;
TCallbackFingerprint onCountAdd = nullptr;
TCallbackFingerprint onCountDel = nullptr;

// Private
unsigned long lastCleanup = 0;
SemaphoreHandle_t fingerprintSemaphore;

void Setup() {
    fingerprintSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(fingerprintSemaphore);
}

void Count(BleFingerprint *f, bool counting) {
    if (counting) {
        if (onCountAdd) onCountAdd(f);
    } else {
        if (onCountDel) onCountDel(f);
    }
}

void Close(BleFingerprint *f, bool close) {
    if (close) {
        if (onClose) onClose(f);
    } else {
        if (onLeft) onLeft(f);
    }
}

void Seen(BLEAdvertisedDevice *advertisedDevice) {
    BLEAdvertisedDevice copy = *advertisedDevice;

    if (onSeen) onSeen(true);
    BleFingerprint *f = GetFingerprint(&copy);
    if (f->seen(&copy) && onAdd)
        onAdd(f);
    if (onSeen) onSeen(false);
}

bool addOrReplace(DeviceConfig config) {
    for (auto &it : deviceConfigs) {
        if (it.id == config.id) {
            it = config;
            return false;
        }
    }
    deviceConfigs.push_back(config);
    return true;
}

bool Config(String &id, String &json) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);

    DeviceConfig config = {};
    config.id = id;
    if (doc.containsKey("id")) {
        auto alias = doc["id"].as<String>();
        if (alias != id) config.alias = alias;
    }
    if (doc.containsKey("rssi@1m"))
        config.calRssi = doc["rssi@1m"].as<int8_t>();
    if (doc.containsKey("name"))
        config.name = doc["name"].as<String>();
    auto isNew = addOrReplace(config);

    if (isNew) {
        auto p = id.indexOf("irk:");
        if (p == 0) {
            auto irk_hex = id.substring(4);
            uint8_t *irk = new uint8_t[16];
            if (!hextostr(irk_hex, irk, 16))
                return false;
            irks.push_back(irk);
        }
    }

    for (auto &it : fingerprints) {
        auto it_id = it->getId();
        if (it_id == id || it_id == config.alias) {
            it->setName(config.name);
            it->setId(config.alias.length() > 0 ? config.alias : config.id, ID_TYPE_ALIAS, config.name);
            if (config.calRssi != NO_RSSI)
                it->set1mRssi(config.calRssi);
        } else
            it->fingerprintAddress();
    }

    return true;
}

void ConnectToWifi() {
    std::istringstream iss(knownIrks.c_str());
    std::string irk_hex;
    while (iss >> irk_hex) {
        uint8_t *irk = new uint8_t[16];
        if (!hextostr(irk_hex.c_str(), irk, 16))
            continue;
        irks.push_back(irk);
    }
}

bool Command(String &command, String &pay) {
    if (command == "max_distance") {
        maxDistance = pay.toFloat();
        spurt("/max_dist", pay);
    } else if (command == "absorption") {
        absorption = pay.toFloat();
        spurt("/absorption", pay);
    } else if (command == "rx_adj_rssi") {
        rxAdjRssi = pay.toFloat();
        spurt("/rx_adj_rssi", pay);
    } else if (command == "ref_rssi") {
        rxRefRssi = pay.toFloat();
        spurt("/ref_rssi", pay);
    } else if (command == "tx_ref_rssi") {
        rxRefRssi = pay.toFloat();
        spurt("/tx_ref_rssi", pay);
    } else if (command == "query") {
        query = pay;
        spurt("/query", pay);
    } else if (command == "include") {
        include = pay;
        spurt("/include", pay);
    } else if (command == "exclude") {
        exclude = pay;
        spurt("/exclude", pay);
    } else if (command == "known_macs") {
        knownMacs = pay;
        spurt("/known_macs", pay);
    } else if (command == "known_irks") {
        knownIrks = pay;
        spurt("/known_irks", pay);
    } else if (command == "count_ids") {
        countIds = pay;
        spurt("/count_ids", pay);
    } else
        return false;
    return true;
}

void CleanupOldFingerprints() {
    auto now = millis();
    if (now - lastCleanup < 5000) return;
    lastCleanup = now;
    auto it = fingerprints.begin();
    bool any = false;
    while (it != fingerprints.end()) {
        auto age = (*it)->getMsSinceLastSeen();
        if (age > forgetMs) {
            if (onDel) onDel((*it));
            delete *it;
            it = fingerprints.erase(it);
        } else {
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

BleFingerprint *getFingerprintInternal(BLEAdvertisedDevice *advertisedDevice) {
    auto mac = advertisedDevice->getAddress();

    auto it = std::find_if(fingerprints.rbegin(), fingerprints.rend(), [mac](BleFingerprint *f) { return f->getAddress() == mac; });
    if (it != fingerprints.rend())
        return *it;

    auto created = new BleFingerprint(advertisedDevice, ONE_EURO_FCMIN, ONE_EURO_BETA, ONE_EURO_DCUTOFF);
    auto it2 = std::find_if(fingerprints.begin(), fingerprints.end(), [created](BleFingerprint *f) { return f->getId() == created->getId(); });
    if (it2 != fingerprints.end()) {
        auto found = *it2;
        // Serial.printf("Detected mac switch for fingerprint id %s\n", found->getId().c_str());
        created->setInitial(found->getRssi(), found->getDistance());
        if (found->getIdType() > ID_TYPE_UNIQUE)
            found->expire();
    }

    fingerprints.push_back(created);
    return created;
}

BleFingerprint *GetFingerprint(BLEAdvertisedDevice *advertisedDevice) {
    if (xSemaphoreTake(fingerprintSemaphore, 1000) != pdTRUE)
        log_e("Couldn't take semaphore!");
    auto f = getFingerprintInternal(advertisedDevice);
    if (xSemaphoreGive(fingerprintSemaphore) != pdTRUE)
        log_e("Couldn't give semaphore!");
    return f;
}

const std::vector<BleFingerprint *> GetCopy() {
    if (xSemaphoreTake(fingerprintSemaphore, 1000) != pdTRUE)
        log_e("Couldn't take semaphore!");
    CleanupOldFingerprints();
    std::vector<BleFingerprint *> copy(fingerprints);
    if (xSemaphoreGive(fingerprintSemaphore) != pdTRUE)
        log_e("Couldn't give semaphore!");
    return std::move(copy);
}

bool FindDeviceConfig(const String &id, DeviceConfig &config) {
    auto it = std::find_if(deviceConfigs.begin(), deviceConfigs.end(), [id](DeviceConfig dc) { return dc.id == id; });
    if (it == deviceConfigs.end()) return false;
    config = (*it);
    return true;
}

}  // namespace BleFingerprintCollection
