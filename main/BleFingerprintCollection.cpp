#include "BleFingerprintCollection.h"

#include <algorithm>
#include <cctype>
#include <new>

#include "Logger.h"
#include "Settings.h"
#include "defaults.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "mqtt.h"
#include "string_utils.h"
#include "util.h"

namespace BleFingerprintCollection {
namespace {

struct FingerprintSlot {
    BleFingerprint *fingerprint = nullptr;
    uint16_t refs = 0;
};

std::vector<FingerprintSlot> fingerprints;
size_t activeFingerprints = 0;

// Fingerprints detached from their slot, owned here until FingerprintLock's
// destructor notifies and frees them outside the mutex.
std::vector<BleFingerprint *> dying;

void removeSlot(size_t index, bool notify = true) {
    auto &slot = fingerprints[index];
    if (slot.fingerprint == nullptr || slot.refs != 0)
        return;

    auto *doomed = slot.fingerprint;
    slot.fingerprint = nullptr;
    if (activeFingerprints > 0)
        --activeFingerprints;

    if (notify && onDel)
        dying.push_back(doomed);
    else
        delete doomed;
}

size_t findEmptySlot() {
    for (size_t i = 0; i < fingerprints.size(); ++i)
        if (fingerprints[i].fingerprint == nullptr)
            return i;
    return static_cast<size_t>(-1);
}

size_t findEvictionSlot() {
    size_t candidate = static_cast<size_t>(-1);
    unsigned long oldestAge = 0;

    for (size_t i = 0; i < fingerprints.size(); ++i) {
        auto &slot = fingerprints[i];
        if (slot.fingerprint == nullptr || slot.refs != 0)
            continue;

        auto age = slot.fingerprint->getMsSinceLastSeen();
        if (candidate == static_cast<size_t>(-1) || age > oldestAge) {
            candidate = i;
            oldestAge = age;
        }
    }

    return candidate;
}

size_t findAvailableSlot() {
    auto empty = findEmptySlot();
    if (empty != static_cast<size_t>(-1))
        return empty;

    auto eviction = findEvictionSlot();
    if (eviction == static_cast<size_t>(-1))
        return eviction;

    removeSlot(eviction);
    return eviction;
}

void configureSlots(size_t capacity) {
    if (!fingerprints.empty() && fingerprints.size() != capacity) {
        log_w("Ignoring runtime max_fingerprints change from %u to %u; restart required",
              static_cast<unsigned>(fingerprints.size()), static_cast<unsigned>(capacity));
        return;
    }

    if (fingerprints.empty())
        fingerprints.resize(capacity);
}

FingerprintLease acquireSlot(size_t index) {
    auto &slot = fingerprints[index];
    if (slot.fingerprint == nullptr)
        return {};

    ++slot.refs;
    return {slot.fingerprint, index};
}

FingerprintLease findByAddress(const ble_addr_t &mac) {
    for (size_t i = fingerprints.size(); i-- > 0;) {
        auto &slot = fingerprints[i];
        if (slot.fingerprint != nullptr && Ble::addrEq(slot.fingerprint->getAddress(), mac))
            return acquireSlot(i);
    }
    return {};
}

BleFingerprint *findById(const std::string &id) {
    for (auto &slot : fingerprints)
        if (slot.fingerprint != nullptr && slot.fingerprint->getId() == id)
            return slot.fingerprint;
    return nullptr;
}

}  // namespace

// Public (externed)
std::string include{DEFAULT_INCLUDE},
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
    requeryMs = DEFAULT_REQUERY_MS,
    maxFingerprints = DEFAULT_MAX_FINGERPRINTS;
std::vector<DeviceConfig> deviceConfigs;
std::vector<uint8_t *> irks;
TCallbackBool onSeen = nullptr;
TCallbackFingerprint onAdd = nullptr;
TCallbackFingerprint onDel = nullptr;
TCallbackFingerprint onClose = nullptr;
TCallbackFingerprint onLeft = nullptr;
TCallbackFingerprint onCountAdd = nullptr;
TCallbackFingerprint onCountDel = nullptr;

// Private
const TickType_t MAX_WAIT = pdMS_TO_TICKS(100);

unsigned long lastCleanup = 0;
SemaphoreHandle_t fingerprintMutex;
SemaphoreHandle_t deviceConfigMutex;

// Holds fingerprintMutex for the scope, then runs the deferred onDel callbacks
// *after* releasing it. onDel logs to serial and TCP, which blocks for
// milliseconds per line — doing that under the lock starved the other task's
// MAX_WAIT acquire and dropped BLE advertisements.
struct FingerprintLock {
    const bool ok;

    FingerprintLock() : ok(xSemaphoreTake(fingerprintMutex, MAX_WAIT) == pdTRUE) {}

    ~FingerprintLock() {
        if (!ok) return;

        std::vector<BleFingerprint *> doomed;
        doomed.swap(dying);  // hand off while we still hold the lock
        xSemaphoreGive(fingerprintMutex);

        for (auto *f : doomed) {
            if (onDel) onDel(f);
            delete f;
        }
    }

    FingerprintLock(const FingerprintLock &) = delete;
    FingerprintLock &operator=(const FingerprintLock &) = delete;

    explicit operator bool() const { return ok; }
};

void Setup() {
    BleFingerprint::InitLocks();
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

void Seen(const Ble::Advert *advertisedDevice) {

    if (onSeen) onSeen(true);
    auto lease = GetFingerprint(advertisedDevice);
    if (lease && lease.fingerprint->seen(advertisedDevice) && onAdd)
        onAdd(lease.fingerprint);
    Release(lease);
    if (onSeen) onSeen(false);
}

enum class AddOrReplaceResult {
    Failed,
    Added,
    Replaced,
};

/**
 * @brief Add a device configuration or replace an existing one with the same id.
 *
 * If the provided config has a non-empty alias, any existing configs with the same alias but a different id
 * are removed and their ids are scheduled for deletion (deletion occurs after the function returns).
 *
 * @param config DeviceConfig to add or use to replace an existing entry with the same `id`.
 * @return Added for a new config, Replaced when an existing config was updated, Failed if the mutex
 *         could not be acquired (shared state left unchanged).
 */
AddOrReplaceResult addOrReplace(DeviceConfig config) {
    if (xSemaphoreTake(deviceConfigMutex, MAX_WAIT) != pdTRUE) {
        log_e("Couldn't take deviceConfigMutex in addOrReplace!");
        return AddOrReplaceResult::Failed;
    }

    std::vector<std::string> idsToDelete;
    bool isReplacement = false;

    if (!config.alias.empty()) {
        for (auto it = deviceConfigs.begin(); it != deviceConfigs.end();) {
            if (it->alias == config.alias && it->id != config.id) {
                idsToDelete.push_back(it->id);
                it = deviceConfigs.erase(it);
            } else {
                ++it;
            }
        }
    }

    for (auto &it : deviceConfigs) {
        if (it.id == config.id) {
            it = config;
            isReplacement = true;
            break;
        }
    }

    if (!isReplacement) {
        deviceConfigs.push_back(config);
    }

    xSemaphoreGive(deviceConfigMutex);

    // Call deleteConfig outside the critical section to avoid mutex re-entrance
    for (const std::string &id : idsToDelete) {
        deleteConfig(id);
    }

    return isReplacement ? AddOrReplaceResult::Replaced : AddOrReplaceResult::Added;
}

bool removeConfig(const std::string &id) {
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

bool Config(std::string &id, std::string &json) {
    if (json.empty()) {
        return removeConfig(id);
    }

    DynamicJsonDocument doc(512);
    deserializeJson(doc, json);

    DeviceConfig config = {};
    config.id = id;
    if (doc.containsKey("id")) {
        auto alias = doc["id"].as<std::string>();
        if (alias != id) config.alias = alias;
    }
    if (doc.containsKey("rssi@1m"))
        config.calRssi = doc["rssi@1m"].as<int8_t>();
    if (doc.containsKey("name"))
        config.name = doc["name"].as<std::string>();
    if (doc.containsKey("connect") && doc["connect"].is<bool>())
        config.allowConnect = doc["connect"].as<bool>();
    auto result = addOrReplace(config);
    if (result == AddOrReplaceResult::Failed)
        return false;

    if (result == AddOrReplaceResult::Added) {
        if (startsWith(id, "irk:")) {
            auto irk_hex = id.substr(4);
            auto *irk = new uint8_t[16];
            if (!hextostr(irk_hex, irk, 16))
                return false;
            irks.push_back(irk);
        }
    }

    FingerprintLock lock;
    if (!lock) {
        log_e("Couldn't take fingerprintMutex in Config!");
        return false;
    }
    for (auto &slot : fingerprints) {
        if (slot.fingerprint == nullptr)
            continue;

        auto *fingerprint = slot.fingerprint;
        auto it_id = fingerprint->getId();
        if (it_id == id || it_id == config.alias) {
            fingerprint->setName(config.name);
            fingerprint->setId(config.alias.length() > 0 ? config.alias : config.id, ID_TYPE_ALIAS, config.name);
            if (config.calRssi != NO_RSSI)
                fingerprint->set1mRssi(config.calRssi);
        } else
            fingerprint->fingerprintAddress();
    }

    return true;
}

void ConnectToWifi(bool updating) {
    knownMacs = Settings::string("known_macs", DEFAULT_KNOWN_MACS, "Known BLE mac addresses (no colons, space separated)");
    knownIrks = Settings::string("known_irks", DEFAULT_KNOWN_IRKS, "Known BLE identity resolving keys, should be 32 hex chars space separated");

    query = Settings::string("query", DEFAULT_QUERY, "Query device ids for characteristics (eg. flora:)");
    requeryMs = Settings::integer("requery_ms", 30, 3600, DEFAULT_REQUERY_MS / 1000, "Requery interval in seconds") * 1000;

    countIds = Settings::string("count_ids", DEFAULT_COUNT_IDS, "Include id prefixes (space separated)");
    countEnter = Settings::floating("count_enter", 0, 100, DEFAULT_COUNT_ENTER, "Start counting devices less than distance (in meters)");
    countExit = Settings::floating("count_exit", 0, 100, DEFAULT_COUNT_EXIT, "Stop counting devices greater than distance (in meters)");
    countMs = Settings::integer("count_ms", 0, 3000000, DEFAULT_COUNT_MS, "Include devices with age less than (in ms)");

    include = Settings::string("include", DEFAULT_INCLUDE, "Include only sending these ids to mqtt (eg. apple:iphone10-6 apple:iphone13-2)");
    exclude = Settings::string("exclude", DEFAULT_EXCLUDE, "Exclude sending these ids to mqtt (eg. exp:20 apple:iphone10-6)");
    maxDistance = Settings::floating("max_dist", 0, 100, DEFAULT_MAX_DISTANCE, "Maximum distance to report (in meters)");
    skipDistance = Settings::floating("skip_dist", 0, 10, DEFAULT_SKIP_DISTANCE, "Report early if beacon has moved more than this distance (in meters)");
    skipMs = Settings::integer("skip_ms", 0, 3000000, DEFAULT_SKIP_MS, "Skip reporting if message age is less that this (in milliseconds)");
    maxFingerprints = Settings::integer("max_fingerprints", 16, 2048, DEFAULT_MAX_FINGERPRINTS, "Maximum BLE fingerprints to track");

    rxRefRssi = Settings::integer("ref_rssi", -100, 100, DEFAULT_RX_REF_RSSI, "Rssi expected from a 0dBm transmitter at 1 meter (NOT used for iBeacons or Eddystone)");
    rxAdjRssi = Settings::integer("rx_adj_rssi", -100, 100, DEFAULT_RX_ADJ_RSSI, "Rssi adjustment for receiver (use only if you know this device has a weak antenna)");
    absorption = Settings::floating("absorption", 1, 5, DEFAULT_ABSORPTION, "Factor used to account for absorption, reflection, or diffraction");
    forgetMs = Settings::integer("forget_ms", 0, 3000000, DEFAULT_FORGET_MS, "Forget beacon if not seen for (in milliseconds)");
    txRefRssi = Settings::integer("tx_ref_rssi", -100, 0, DEFAULT_TX_REF_RSSI, "Rssi expected from this tx power at 1m (used for node iBeacon)");
    maxDivisor = Settings::integer("max_divisor", 2, 10, DEFAULT_MAX_DIVISOR, "Max divisor for reporting interval");
    if (!updating)
        configureSlots(maxFingerprints);

    size_t start = 0;
    while (start < static_cast<size_t>(knownIrks.length())) {
        while (start < static_cast<size_t>(knownIrks.length()) && std::isspace(static_cast<unsigned char>(knownIrks[start])))
            ++start;
        if (start >= static_cast<size_t>(knownIrks.length()))
            break;

        size_t end = start;
        while (end < static_cast<size_t>(knownIrks.length()) && !std::isspace(static_cast<unsigned char>(knownIrks[end])))
            ++end;

        auto irk_hex = knownIrks.substr(start, end - start);
        start = end;
        auto *irk = new uint8_t[16];
        if (!hextostr(irk_hex, irk, 16)) {
            delete[] irk;
            continue;
        }
        irks.push_back(irk);
    }
}

bool Command(std::string &command, std::string &pay) {
    if (command == "skip_ms") {
        BleFingerprintCollection::skipMs = pay.empty() ? DEFAULT_SKIP_MS : toInt(pay);
        spurt("/skip_ms", toStr(skipMs));
    } else if (command == "skip_distance") {
        BleFingerprintCollection::skipDistance = pay.empty() ? DEFAULT_SKIP_DISTANCE : toFloat(pay);
        spurt("/skip_dist", toStr(skipDistance));
    } else if (command == "max_distance") {
        maxDistance = pay.empty() ? DEFAULT_MAX_DISTANCE : toFloat(pay);
        spurt("/max_dist", toStr(maxDistance));
    } else if (command == "absorption") {
        absorption = pay.empty() ? DEFAULT_ABSORPTION : toFloat(pay);
        spurt("/absorption", toStr(absorption));
    } else if (command == "rx_adj_rssi") {
        rxAdjRssi = pay.empty() ? DEFAULT_RX_ADJ_RSSI : (int8_t)toInt(pay);
        spurt("/rx_adj_rssi", toStr(rxAdjRssi));
    } else if (command == "ref_rssi") {
        rxRefRssi = pay.empty() ? DEFAULT_RX_REF_RSSI : (int8_t)toInt(pay);
        spurt("/ref_rssi", toStr(rxRefRssi));
    } else if (command == "tx_ref_rssi") {
        txRefRssi = pay.empty() ? DEFAULT_TX_REF_RSSI : (int8_t)toInt(pay);
        spurt("/tx_ref_rssi", toStr(txRefRssi));
    } else if (command == "query") {
        query = pay.empty() ? DEFAULT_QUERY : pay;
        spurt("/query", query);
    } else if (command == "include") {
        include = pay.empty() ? DEFAULT_INCLUDE : pay;
        spurt("/include", include);
    } else if (command == "exclude") {
        exclude = pay.empty() ? DEFAULT_EXCLUDE : pay;
        spurt("/exclude", exclude);
    } else if (command == "known_macs") {
        knownMacs = pay.empty() ? DEFAULT_KNOWN_MACS : pay;
        spurt("/known_macs", knownMacs);
    } else if (command == "known_irks") {
        knownIrks = pay.empty() ? DEFAULT_KNOWN_IRKS : pay;
        spurt("/known_irks", knownIrks);
    } else if (command == "count_ids") {
        countIds = pay.empty() ? DEFAULT_COUNT_IDS : pay;
        spurt("/count_ids", countIds);
    } else if (command == "max_divisor") {
        maxDivisor = pay.empty() ? DEFAULT_MAX_DIVISOR : toInt(pay);
        spurt("/max_divisor", toStr(maxDivisor));
    } else if (command == "forget_ms") {
        forgetMs = pay.empty() ? DEFAULT_FORGET_MS : toInt(pay);
        spurt("/forget_ms", toStr(forgetMs));
    } else if (command == "requery_ms") {
        // Persisted as seconds (HeadlessWiFiSettings reads back seconds on next boot
        // and multiplies by 1000 — see line 322); in memory the variable is ms.
        requeryMs = pay.empty() ? DEFAULT_REQUERY_MS : toInt(pay);
        spurt("/requery_ms", toStr(requeryMs / 1000));
    } else if (command == "count_enter") {
        countEnter = pay.empty() ? DEFAULT_COUNT_ENTER : toFloat(pay);
        spurt("/count_enter", toStr(countEnter));
    } else if (command == "count_exit") {
        countExit = pay.empty() ? DEFAULT_COUNT_EXIT : toFloat(pay);
        spurt("/count_exit", toStr(countExit));
    } else if (command == "count_ms") {
        countMs = pay.empty() ? DEFAULT_COUNT_MS : toInt(pay);
        spurt("/count_ms", toStr(countMs));
    } else
        return false;
    return true;
}

/**
 * @brief Removes stale Bluetooth fingerprints and performs end-of-life actions.
 *
 * Runs at most once every 5 seconds; each fingerprint whose time since last seen
 * exceeds `forgetMs` is detached from its slot and queued for `onDel` notification,
 * which FingerprintLock runs once it releases fingerprintMutex. If no
 * fingerprints remain and the system uptime exceeds `ALLOW_BLE_CONTROLLER_RESTART_AFTER_SECS`,
 * the function logs a message and calls `ESP.restart()`.
 *
 * Must be called with fingerprintMutex held.
 */
void CleanupOldFingerprints() {
    auto now = millis();
    if (now - lastCleanup < 5000) return;
    lastCleanup = now;
    bool any = false;
    for (size_t i = 0; i < fingerprints.size(); ++i) {
        auto &slot = fingerprints[i];
        if (slot.fingerprint == nullptr)
            continue;

        // A leased fingerprint still counts as present; removeSlot() won't touch it
        // and it gets reaped on a later pass once the reader releases.
        if (slot.refs != 0 || slot.fingerprint->getMsSinceLastSeen() <= forgetMs)
            any = true;
        else
            removeSlot(i);
    }
    if (!any) {
        auto uptime = (unsigned long)(esp_timer_get_time() / 1000000ULL);
        if (uptime > ALLOW_BLE_CONTROLLER_RESTART_AFTER_SECS) {
            Log.println("Bluetooth controller seems stuck, restarting");
            esp_restart();
        }
    }
}

/**
 * @brief Obtain the fingerprint associated with an advertised BLE device.
 *
 * Returns an existing fingerprint that matches the device's MAC address or creates
 * and registers a new fingerprint if none exists. When a new fingerprint is created
 * and an existing fingerprint with the same logical ID is found, the new fingerprint
 * inherits the existing fingerprint's initial state and the existing fingerprint may
 * be expired depending on its ID type.
 *
 * @param advertisedDevice Advertised device used to identify or construct the fingerprint.
 * @return FingerprintLease Lease for the existing or newly created fingerprint stored in the collection.
 */
FingerprintLease getFingerprintInternal(const Ble::Advert *advertisedDevice) {
    if (auto existing = findByAddress(advertisedDevice->getAddress()))
        return existing;

    CleanupOldFingerprints();

    auto slotIndex = findAvailableSlot();
    if (slotIndex == static_cast<size_t>(-1)) {
        log_w("Dropping BLE fingerprint; max_fingerprints=%d is exhausted", maxFingerprints);
        return {};
    }

    auto *created = new (std::nothrow) BleFingerprint(advertisedDevice);
    if (created == nullptr) {
        log_e("Failed to allocate fingerprint");
        return {};
    }

    if (auto *found = findById(created->getId())) {
        created->setInitial(*found);
        if (found->getIdType() > ID_TYPE_UNIQUE)
            found->expire();
    }

    auto &slot = fingerprints[slotIndex];
    slot.fingerprint = created;
    slot.refs = 1;
    ++activeFingerprints;
    return {created, slotIndex};
}

FingerprintLease GetFingerprint(const Ble::Advert *advertisedDevice) {
    FingerprintLock lock;
    if (!lock) {
        log_e("Couldn't take semaphore!");
        return {};
    }
    return getFingerprintInternal(advertisedDevice);
}

FingerprintLease AcquireNext(size_t &cursor, bool cleanup) {
    FingerprintLock lock;
    if (!lock) {
        log_e("Couldn't take fingerprintMutex!");
        return {};
    }
    if (cleanup) CleanupOldFingerprints();

    while (cursor < fingerprints.size())
        if (auto lease = acquireSlot(cursor++))
            return lease;

    return {};
}

void Release(FingerprintLease &lease) {
    if (!lease)
        return;

    FingerprintLock lock;
    if (!lock) {
        log_e("Couldn't take fingerprintMutex!");
        // Leak the lease rather than touch shared state without the mutex —
        // the slot's refs stays elevated so the fingerprint isn't freed
        // from under another reader; acceptable trade-off vs corruption.
        return;
    }

    if (lease.slot < fingerprints.size()) {
        auto &slot = fingerprints[lease.slot];
        if (slot.fingerprint == lease.fingerprint && slot.refs > 0)
            --slot.refs;
    }

    lease = {};
}

size_t Size(bool cleanup) {
    FingerprintLock lock;
    if (!lock) {
        log_e("Couldn't take fingerprintMutex!");
        return 0;
    }
    if (cleanup) CleanupOldFingerprints();
    return activeFingerprints;
}

bool FindDeviceConfig(const std::string &id, DeviceConfig &config) {
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

bool FindDeviceConfigByAlias(const std::string &alias, DeviceConfig &config) {
    if (xSemaphoreTake(deviceConfigMutex, MAX_WAIT) == pdTRUE) {
        auto it = std::find_if(deviceConfigs.begin(), deviceConfigs.end(), [alias](DeviceConfig dc) { return dc.alias == alias; });
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
