#include "BleFingerprintCollection.h"

#include "defaults.h"
#include <Arduino.h>
#include <sstream>
#include <HeadlessWiFiSettings.h>

namespace BleFingerprintCollection {
// Public (externed)
String include{DEFAULT_INCLUDE},
       exclude{DEFAULT_EXCLUDE},
       query{DEFAULT_QUERY},
       knownMacs{DEFAULT_KNOWN_MACS},
       knownIrks{DEFAULT_KNOWN_IRKS},
       countIds{DEFAULT_COUNT_IDS};
float skipDistance = DEFAULT_SKIP_DISTANCE,
      maxDistance = DEFAULT_MAX_DISTANCE,
      absorption = DEFAULT_ABSORPTION,
      countEnter = DEFAULT_COUNT_ENTER,
      countExit = DEFAULT_COUNT_EXIT;
int8_t rxRefRssi = DEFAULT_RX_REF_RSSI,
       rxAdjRssi = DEFAULT_RX_ADJ_RSSI,
       txRefRssi = DEFAULT_TX_REF_RSSI,
       maxDivisor = DEFAULT_MAX_DIVISOR;
int forgetMs = DEFAULT_FORGET_MS,
    skipMs = DEFAULT_SKIP_MS,
    countMs = DEFAULT_COUNT_MS,
    requeryMs = DEFAULT_REQUERY_MS;
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
const TickType_t MAX_WAIT = portTICK_PERIOD_MS * 100;

unsigned long lastCleanup = 0;
SemaphoreHandle_t fingerprintMutex;
SemaphoreHandle_t deviceConfigMutex;

void Setup() {
    fingerprintMutex = xSemaphoreCreateMutex();
    deviceConfigMutex = xSemaphoreCreateMutex();
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
    if (xSemaphoreTake(deviceConfigMutex, MAX_WAIT) != pdTRUE)
        log_e("Couldn't take deviceConfigMutex in addOrReplace!");

    for (auto &it : deviceConfigs) {
        if (it.id == config.id) {
            it = config;
            xSemaphoreGive(deviceConfigMutex);
            return false;
        }
    }
    deviceConfigs.push_back(config);
    xSemaphoreGive(deviceConfigMutex);
    return true;
}

bool removeConfig(const String &id) {
    if (xSemaphoreTake(deviceConfigMutex, MAX_WAIT) != pdTRUE) {
        log_e("Couldn't take deviceConfigMutex in removeConfig!");
        return false;
    }

    auto it = std::remove_if(deviceConfigs.begin(), deviceConfigs.end(),
        [&id](const DeviceConfig &config) { return config.id == id; });

    bool removed = it != deviceConfigs.end();
    deviceConfigs.erase(it, deviceConfigs.end());

    xSemaphoreGive(deviceConfigMutex);
    return removed;
}

bool Config(String &id, String &json) {
    if (json.isEmpty()) {
        return removeConfig(id);
    }

    DynamicJsonDocument doc(512);
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
            auto *irk = new uint8_t[16];
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
    knownMacs = HeadlessWiFiSettings.string("known_macs", DEFAULT_KNOWN_MACS, "Known BLE mac addresses (no colons, space seperated)");
    knownIrks = HeadlessWiFiSettings.string("known_irks", DEFAULT_KNOWN_IRKS, "Known BLE identity resolving keys, should be 32 hex chars space seperated");

    query = HeadlessWiFiSettings.string("query", DEFAULT_QUERY, "Query device ids for characteristics (eg. flora:)");
    requeryMs = HeadlessWiFiSettings.integer("requery_ms", 30, 3600, DEFAULT_REQUERY_MS / 1000, "Requery interval in seconds") * 1000;

    countIds = HeadlessWiFiSettings.string("count_ids", DEFAULT_COUNT_IDS, "Include id prefixes (space seperated)");
    countEnter = HeadlessWiFiSettings.floating("count_enter", 0, 100, DEFAULT_COUNT_ENTER, "Start counting devices less than distance (in meters)");
    countExit = HeadlessWiFiSettings.floating("count_exit", 0, 100, DEFAULT_COUNT_EXIT, "Stop counting devices greater than distance (in meters)");
    countMs = HeadlessWiFiSettings.integer("count_ms", 0, 3000000, DEFAULT_COUNT_MS, "Include devices with age less than (in ms)");

    include = HeadlessWiFiSettings.string("include", DEFAULT_INCLUDE, "Include only sending these ids to mqtt (eg. apple:iphone10-6 apple:iphone13-2)");
    exclude = HeadlessWiFiSettings.string("exclude", DEFAULT_EXCLUDE, "Exclude sending these ids to mqtt (eg. exp:20 apple:iphone10-6)");
    maxDistance = HeadlessWiFiSettings.floating("max_dist", 0, 100, DEFAULT_MAX_DISTANCE, "Maximum distance to report (in meters)");
    skipDistance = HeadlessWiFiSettings.floating("skip_dist", 0, 10, DEFAULT_SKIP_DISTANCE, "Report early if beacon has moved more than this distance (in meters)");
    skipMs = HeadlessWiFiSettings.integer("skip_ms", 0, 3000000, DEFAULT_SKIP_MS, "Skip reporting if message age is less that this (in milliseconds)");

    rxRefRssi = HeadlessWiFiSettings.integer("ref_rssi", -100, 100, DEFAULT_RX_REF_RSSI, "Rssi expected from a 0dBm transmitter at 1 meter (NOT used for iBeacons or Eddystone)");
    rxAdjRssi = HeadlessWiFiSettings.integer("rx_adj_rssi", -100, 100, DEFAULT_RX_ADJ_RSSI, "Rssi adjustment for receiver (use only if you know this device has a weak antenna)");
    absorption = HeadlessWiFiSettings.floating("absorption", 1, 5, DEFAULT_ABSORPTION, "Factor used to account for absorption, reflection, or diffraction");
    forgetMs = HeadlessWiFiSettings.integer("forget_ms", 0, 3000000, DEFAULT_FORGET_MS, "Forget beacon if not seen for (in milliseconds)");
    txRefRssi = HeadlessWiFiSettings.integer("tx_ref_rssi", -100, 0, DEFAULT_TX_REF_RSSI, "Rssi expected from this tx power at 1m (used for node iBeacon)");
    maxDivisor = HeadlessWiFiSettings.integer("max_divisor", 2, 10, DEFAULT_MAX_DIVISOR, "Max divisor for reporting interval");

    std::istringstream iss(knownIrks.c_str());
    std::string irk_hex;
    while (iss >> irk_hex) {
        auto *irk = new uint8_t[16];
        if (!hextostr(irk_hex.c_str(), irk, 16))
            continue;
        irks.push_back(irk);
    }
}

bool Command(String &command, String &pay) {
    if (command == "skip_ms") {
        BleFingerprintCollection::skipMs = pay.isEmpty() ? DEFAULT_SKIP_MS : pay.toInt();
        spurt("/skip_ms", String(skipMs));
    } else if (command == "skip_distance") {
        BleFingerprintCollection::skipDistance = pay.isEmpty() ? DEFAULT_SKIP_DISTANCE : pay.toFloat();
        spurt("/skip_dist", String(skipDistance));
    } else if (command == "max_distance") {
        maxDistance = pay.isEmpty() ? DEFAULT_MAX_DISTANCE : pay.toFloat();
        spurt("/max_dist", String(maxDistance));
    } else if (command == "absorption") {
        absorption = pay.isEmpty() ? DEFAULT_ABSORPTION : pay.toFloat();
        spurt("/absorption", String(absorption));
    } else if (command == "rx_adj_rssi") {
        rxAdjRssi = pay.isEmpty() ? DEFAULT_RX_ADJ_RSSI : (int8_t)pay.toInt();
        spurt("/rx_adj_rssi", String(rxAdjRssi));
    } else if (command == "ref_rssi") {
        rxRefRssi = pay.isEmpty() ? DEFAULT_RX_REF_RSSI : (int8_t)pay.toInt();
        spurt("/ref_rssi", String(rxRefRssi));
    } else if (command == "tx_ref_rssi") {
        txRefRssi = pay.isEmpty() ? DEFAULT_TX_REF_RSSI : (int8_t)pay.toInt();
        spurt("/tx_ref_rssi", String(txRefRssi));
    } else if (command == "query") {
        query = pay.isEmpty() ? DEFAULT_QUERY : pay;
        spurt("/query", query);
    } else if (command == "include") {
        include = pay.isEmpty() ? DEFAULT_INCLUDE : pay;
        spurt("/include", include);
    } else if (command == "exclude") {
        exclude = pay.isEmpty() ? DEFAULT_EXCLUDE : pay;
        spurt("/exclude", exclude);
    } else if (command == "known_macs") {
        knownMacs = pay.isEmpty() ? DEFAULT_KNOWN_MACS : pay;
        spurt("/known_macs", knownMacs);
    } else if (command == "known_irks") {
        knownIrks = pay.isEmpty() ? DEFAULT_KNOWN_IRKS : pay;
        spurt("/known_irks", knownIrks);
    } else if (command == "count_ids") {
        countIds = pay.isEmpty() ? DEFAULT_COUNT_IDS : pay;
        spurt("/count_ids", countIds);
    } else if (command == "max_divisor") {
        maxDivisor = pay.isEmpty() ? DEFAULT_MAX_DIVISOR : pay.toInt();
        spurt("/max_divisor", String(maxDivisor));
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

    auto created = new BleFingerprint(advertisedDevice);
    auto it2 = std::find_if(fingerprints.begin(), fingerprints.end(), [created](BleFingerprint *f) { return f->getId() == created->getId(); });
    if (it2 != fingerprints.end()) {
        auto found = *it2;
        // Serial.printf("Detected mac switch for fingerprint id %s\r\n", found->getId().c_str());
        created->setInitial(*found);
        if (found->getIdType() > ID_TYPE_UNIQUE)
            found->expire();
    }

    fingerprints.push_back(created);
    return created;
}

BleFingerprint *GetFingerprint(BLEAdvertisedDevice *advertisedDevice) {
    if (xSemaphoreTake(fingerprintMutex, MAX_WAIT) != pdTRUE)
        log_e("Couldn't take semaphore!");
    auto f = getFingerprintInternal(advertisedDevice);
    xSemaphoreGive(fingerprintMutex);
    return f;
}

const std::vector<BleFingerprint *> GetCopy() {
    if (xSemaphoreTake(fingerprintMutex, MAX_WAIT) != pdTRUE)
        log_e("Couldn't take fingerprintMutex!");
    CleanupOldFingerprints();
    std::vector<BleFingerprint *> copy(fingerprints);
    xSemaphoreGive(fingerprintMutex);
    return std::move(copy);
}

bool FindDeviceConfig(const String &id, DeviceConfig &config) {
    if (xSemaphoreTake(deviceConfigMutex, MAX_WAIT) == pdTRUE) {
        auto it = std::find_if(deviceConfigs.begin(), deviceConfigs.end(), [id](DeviceConfig dc) { return dc.id == id; });
        if (it != deviceConfigs.end()) {
            config = *it;
            xSemaphoreGive(deviceConfigMutex);
            return true;
        }
        xSemaphoreGive(deviceConfigMutex);
        return false;
    }
    log_e("Couldn't take deviceConfigMutex!");
    return false;
}

}  // namespace BleFingerprintCollection
