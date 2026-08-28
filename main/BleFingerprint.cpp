#include "BleFingerprint.h"

#include <math.h>
#include <stdint.h>

#include <algorithm>

#include "BleFingerprintCollection.h"
#include "BleUuids.h"
#include "Logger.h"
#include "MiFloraHandler.h"
#include "NameModelHandler.h"
#include "defaults.h"
#include "globals.h"
#include "mbedtls/aes.h"
#include "rssi.h"
#include "string_utils.h"
#include "util.h"

BleFingerprint::BleFingerprint(const Ble::Advert *advertisedDevice) {
    firstSeenMillis = millis();
    address = advertisedDevice->getAddress();
    addressType = advertisedDevice->getAddressType();
    raw = advertisedDevice->getRSSI();
    rssi = raw - BleFingerprintCollection::rxAdjRssi;
    dist = pow(10, ((float)get1mRssi() - rssi) / (10.0f * BleFingerprintCollection::absorption));
    seenCount = 1;
    queryReport = nullptr;
    fingerprintAddress();
}

void BleFingerprint::setInitial(const BleFingerprint &other) {
    rssi = other.rssi;
    rssiVar = other.rssiVar;
    dist = other.dist;
    distVar = other.distVar;
    raw = other.raw;
    if (other.adaptivePercentileRSSI)
        adaptivePercentileRSSI = std::unique_ptr<AdaptivePercentileRSSI>(new AdaptivePercentileRSSI(*other.adaptivePercentileRSSI));
    else
        adaptivePercentileRSSI.reset();
}

bool BleFingerprint::shouldHide(const std::string &s) {
    if (BleFingerprintCollection::include.length() > 0 && !prefixExists(BleFingerprintCollection::include, s)) return true;
    return (BleFingerprintCollection::exclude.length() > 0 && prefixExists(BleFingerprintCollection::exclude, s));
}

bool BleFingerprint::setId(const std::string &newId, short newIdType, const std::string &newName) {
    if (idType < 0 && newIdType < 0 && newIdType >= idType) return false;
    if (idType > 0 && newIdType <= idType) return false;

    ignore = newIdType < 0;
    idType = newIdType;

    DeviceConfig dc;
    if (BleFingerprintCollection::FindDeviceConfig(newId, dc)) {
        if (dc.calRssi != NO_RSSI)
            calRssi = dc.calRssi;
        if (!dc.alias.empty())
            return setId(dc.alias, ID_TYPE_ALIAS, dc.name);
        if (!dc.name.empty())
            name = dc.name;
    } else if (!newName.empty() && name != newName)
        name = newName;

    // Compute new allowQuery based on final newId (after alias recursion)
    {
        bool baseAllow = !ignore;
        bool newQuery = false;
        if (baseAllow) {
            DeviceConfig dcCheck;
            bool found = BleFingerprintCollection::FindDeviceConfig(newId, dcCheck);
            if (!found) found = BleFingerprintCollection::FindDeviceConfigByAlias(newId, dcCheck);
            if (found && dcCheck.allowConnect) {
                newQuery = true;
            } else {
                newQuery = !BleFingerprintCollection::query.empty() && prefixExists(BleFingerprintCollection::query, newId);
            }
        }
        if (newQuery != allowQuery) {
            allowQuery = newQuery;
            if (allowQuery) {
                qryAttempts = 0;
                if (rssi < -80) {
                    qryDelayMillis = 30000;
                    lastQryMillis = millis();
                } else if (rssi < -70) {
                    qryDelayMillis = 5000;
                    lastQryMillis = millis();
                }
            }
        }
    }

    if (id != newId) {
        bool newHidden = shouldHide(newId);
        countable = !ignore && !hidden && !BleFingerprintCollection::countIds.empty() && prefixExists(BleFingerprintCollection::countIds, newId);
        hidden = newHidden;
        added = false;
        auto timeSlot = calculateTimeSlot();
        uint64_t baseInterval = (uint64_t)BleFingerprintCollection::skipMs;
        uint64_t offset_ms = ((uint64_t)timeSlot * (baseInterval / MAX_TIME_SLOTS));
        uint64_t now_ms = getNowMs();
        uint64_t current_interval_start = baseInterval ? (now_ms / baseInterval) * baseInterval : now_ms;
        nextReportMs = current_interval_start + offset_ms;
        id = newId;
        isNode = startsWith(newId, "node:");
    }

    return true;
}

const std::string BleFingerprint::getMac() const {
    const auto *nativeAddress = address.val;
    return Sprintf("%02x%02x%02x%02x%02x%02x", nativeAddress[5], nativeAddress[4], nativeAddress[3], nativeAddress[2], nativeAddress[1], nativeAddress[0]);
}

const int BleFingerprint::get1mRssi() const {
    if (calRssi != NO_RSSI) return calRssi;
    if (bcnRssi != NO_RSSI) return bcnRssi;
    if (mdRssi != NO_RSSI) return mdRssi;
    if (asRssi != NO_RSSI) return asRssi;
    return BleFingerprintCollection::rxRefRssi + DEFAULT_TX;
}

void BleFingerprint::fingerprint(const Ble::Advert *advertisedDevice) {
    if (advertisedDevice->haveName()) {
        const std::string name = advertisedDevice->getName();
        if (!name.empty()) setId(std::string("name:") + kebabify(name), ID_TYPE_NAME, name);
    }

    if (advertisedDevice->getAdvType() > 0)
        connectable = true;

    size_t serviceAdvCount = advertisedDevice->getServiceUUIDCount();
    size_t serviceDataCount = advertisedDevice->getServiceDataCount();
    bool haveTxPower = advertisedDevice->haveTXPower();
    int8_t txPower = advertisedDevice->getTXPower();

    if (serviceAdvCount > 0) fingerprintServiceAdvertisements(advertisedDevice, serviceAdvCount, haveTxPower, txPower);
    if (serviceDataCount > 0) fingerprintServiceData(advertisedDevice, serviceDataCount, haveTxPower, txPower);
    if (advertisedDevice->haveManufacturerData()) fingerprintManufactureData(advertisedDevice, haveTxPower, txPower);
}

static int bt_encrypt_be(const uint8_t *key, const uint8_t *plaintext, uint8_t *enc_data) {
    mbedtls_aes_context ctx;
    mbedtls_aes_init(&ctx);
    if (mbedtls_aes_setkey_enc(&ctx, key, 128) != 0) {
        mbedtls_aes_free(&ctx);
        return BLE_HS_EUNKNOWN;
    }
    if (mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, plaintext, enc_data) != 0) {
        mbedtls_aes_free(&ctx);
        return BLE_HS_EUNKNOWN;
    }
    mbedtls_aes_free(&ctx);
    return 0;
}

// Was a Resolvable Private Address generated from this IRK? (BLE Core spec RPA resolution)
static bool ble_ll_resolv_rpa(const uint8_t *rpa, const uint8_t *irk) {
    uint8_t key[16], plain[16] = {0}, cipher[16];
    memcpy(key, irk, 16);
    plain[15] = rpa[3];
    plain[14] = rpa[4];
    plain[13] = rpa[5];
    bt_encrypt_be(key, plain, cipher);
    return !(cipher[15] != rpa[0] || cipher[14] != rpa[1] || cipher[13] != rpa[2]);
}

void BleFingerprint::fingerprintAddress() {
    auto mac = getMac();
    if (!BleFingerprintCollection::knownMacs.empty() && prefixExists(BleFingerprintCollection::knownMacs, mac))
        setId("known:" + mac, ID_TYPE_KNOWN_MAC);
    else {
        switch (addressType) {
            case BLE_ADDR_PUBLIC:
            case BLE_ADDR_PUBLIC_ID:
                setId(mac, ID_TYPE_PUBLIC_MAC);
                break;
            case BLE_ADDR_RANDOM:
            case BLE_ADDR_RANDOM_ID: {
                const auto *naddress = address.val;
                if ((naddress[5] & 0xc0) == 0xc0)
                    setId(mac, ID_TYPE_RAND_STATIC_MAC);
                else {
                    const auto &knownIrks = BleFingerprintCollection::irks;
                    auto it = std::find_if(knownIrks.begin(), knownIrks.end(), [naddress](uint8_t *irk) { return ble_ll_resolv_rpa(naddress, irk); });
                    if (it != knownIrks.end()) {
                        setId(std::string("irk:") + hexStr(*it, 16), ID_TYPE_KNOWN_IRK);
                        break;
                    }
                    setId(mac, ID_TYPE_RAND_MAC);
                }
                break;
            }
            default:
                setId(mac, ID_TYPE_RAND_MAC);
                break;
        }
    }
}

void BleFingerprint::fingerprintServiceAdvertisements(const Ble::Advert *advertisedDevice, size_t serviceAdvCount, bool haveTxPower, int8_t txPower) {
    for (size_t i = 0; i < serviceAdvCount; i++) {
        auto uuid = advertisedDevice->getServiceUUID(i);
#ifdef VERBOSE
        Log.printf("Verbose | %s | %-58s%.1fdBm AD: %s\r\n", getMac().c_str(), getId().c_str(), rssi, Ble::uuidStr(uuid).c_str());
#endif
        if (Ble::uuidEq(uuid, tileUUID)) {
            asRssi = BleFingerprintCollection::rxRefRssi + TILE_TX;
            setId("tile:" + getMac(), ID_TYPE_TILE);
            return;
        } else if (Ble::uuidEq(uuid, sonosUUID)) {
            asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("sonos:" + getMac(), ID_TYPE_SONOS);
            return;
        } else if (Ble::uuidEq(uuid, itagUUID)) {
            asRssi = BleFingerprintCollection::rxRefRssi + (haveTxPower ? txPower : ITAG_TX);
            setId("itag:" + getMac(), ID_TYPE_ITAG);
            return;
        } else if (Ble::uuidEq(uuid, trackrUUID)) {
            asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("trackr:" + getMac(), ID_TYPE_TRACKR);
            return;
        } else if (Ble::uuidEq(uuid, tractiveUUID)) {
            asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("tractive:" + getMac(), ID_TYPE_TRACTIVE);
            return;
        } else if (Ble::uuidEq(uuid, vanmoofUUID)) {
            asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("vanmoof:" + getMac(), ID_TYPE_VANMOOF);
            return;
        } else if (Ble::uuidEq(uuid, meaterService)) {
            asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("meater:" + getMac(), ID_TYPE_MEATER);
            return;
        } else if (Ble::uuidEq(uuid, nutUUID)) {
            asRssi = BleFingerprintCollection::rxRefRssi + (haveTxPower ? txPower : NUT_TX);
            setId("nut:" + getMac(), ID_TYPE_NUT);
            return;
        } else if (Ble::uuidEq(uuid, miFloraUUID)) {
            asRssi = BleFingerprintCollection::rxRefRssi + (haveTxPower ? txPower : FLORA_TX);
            setId("flora:" + getMac(), ID_TYPE_FLORA);
            return;
        } else if (Ble::uuidEq(uuid, dexaUUID)) {
            asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("dexa:" + getMac(), ID_TYPE_DEXA);
            return;
        }
    }

    std::string fingerprint = "ad:";
    asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
    for (size_t i = 0; i < serviceAdvCount; i++)
        fingerprint += Ble::uuidStr(advertisedDevice->getServiceUUID(i));
    if (haveTxPower) fingerprint += toStr(-txPower);
    setId(fingerprint, ID_TYPE_AD);
}

void BleFingerprint::fingerprintServiceData(const Ble::Advert *advertisedDevice, size_t serviceDataCount, bool haveTxPower, int8_t txPower) {
    asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
    std::string fingerprint = "";
    for (size_t i = 0; i < serviceDataCount; i++) {
        auto uuid = advertisedDevice->getServiceDataUUID(i);
        std::string strServiceData = advertisedDevice->getServiceData(i);
#ifdef VERBOSE
        Log.printf("Verbose | %s | %-58s%.1fdBm SD: %s/%s\r\n", getMac().c_str(), getId().c_str(), rssi, Ble::uuidStr(uuid).c_str(), hexStr(strServiceData).c_str());
#endif
        if (Ble::uuidEq(uuid, exposureUUID)) {  // found COVID-19 exposure tracker
            bcnRssi = BleFingerprintCollection::rxRefRssi + EXPOSURE_TX;
            setId("exp:" + toStr((unsigned)strServiceData.length()), ID_TYPE_EXPOSURE);
        } else if (Ble::uuidEq(uuid, smartTagUUID)) {  // found Samsung smart tag
            asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("smarttag:" + toStr((unsigned)strServiceData.length()), ID_TYPE_SMARTTAG);
        } else if (Ble::uuidEq(uuid, miThermUUID)) {
            asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            if (strServiceData.length() == 15) {  // custom format
                auto serviceData = strServiceData.c_str();
                int16_t t;
                uint16_t h, v;
                memcpy(&t, serviceData + 6, 2);
                memcpy(&h, serviceData + 8, 2);
                memcpy(&v, serviceData + 10, 2);
                temp = float(t) / 100.0f;
                humidity = float(h) / 100.0f;
                mv = v;
                battery = serviceData[12];
                setId("miTherm:" + getMac(), ID_TYPE_MITHERM);
            } else if (strServiceData.length() == 13) {  // format atc1441
                auto serviceData = strServiceData.c_str();
                int16_t x = (serviceData[6] << 8) | (uint8_t)serviceData[7];
                temp = float(x) / 10.0f;
                humidity = (uint8_t)serviceData[8];
                mv = x = ((uint8_t)serviceData[10] << 8) | (uint8_t)serviceData[11];
                battery = serviceData[9];
                setId("miTherm:" + getMac(), ID_TYPE_MITHERM);
            }
        } else if (Ble::uuidEq(uuid, eddystoneUUID) && strServiceData.length() > 0) {
            const auto *d = (const uint8_t *)strServiceData.data();
            if (d[0] == 0x10 && strServiceData.length() >= 2 && strServiceData.length() <= 18) {  // URL frame
                bcnRssi = EDDYSTONE_ADD_1M + (int8_t)d[1];
            } else if (d[0] == 0x20 && strServiceData.length() >= 6) {  // TLM frame
                mv = (d[2] << 8) | d[3];
                temp = (int8_t)d[4] + d[5] / 256.0f;
            } else if (d[0] == 0x00 && strServiceData.length() >= 18) {  // UID frame
                bcnRssi = EDDYSTONE_ADD_1M + (int8_t)d[1];
                setId(Sprintf("eddy:%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x-%02x%02x%02x%02x%02x%02x",
                              d[2], d[3], d[4], d[5], d[6], d[6], d[7], d[8], d[9], d[10], d[11], d[12], d[13], d[14], d[15], d[16], d[17]),
                      ID_TYPE_EBEACON);
            }
        } else {
            fingerprint += Ble::uuidStr(uuid);
        }
    }
    if (!fingerprint.empty()) {
        if (haveTxPower) fingerprint += toStr(-txPower);
        setId("sd:" + fingerprint, ID_TYPE_SD);
    }
}

// Beacon payload (iBeacon and AltBeacon share the layout after the 4-byte prefix):
// [4..19] uuid big-endian, [20..21] major, [22..23] minor, [24] rssi@1m.
static std::string beaconUuid(const uint8_t *d) {
    return Sprintf("%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                   d[4], d[5], d[6], d[7], d[8], d[9], d[10], d[11], d[12], d[13], d[14], d[15], d[16], d[17], d[18], d[19]);
}

void BleFingerprint::fingerprintManufactureData(const Ble::Advert *advertisedDevice, bool haveTxPower, int8_t txPower) {
    std::string strManufacturerData = advertisedDevice->getManufacturerData();
#ifdef VERBOSE
    Log.printf("Verbose | %s | %-58s%.1fdBm MD: %s\r\n", getMac().c_str(), getId().c_str(), rssi, hexStr(strManufacturerData).c_str());
#endif
    if (strManufacturerData.length() >= 2) {
        const auto *d = (const uint8_t *)strManufacturerData.data();
        std::string manuf = Sprintf("%02x%02x", d[1], d[0]);

        if (manuf == "004c")  // Apple
        {
            if (strManufacturerData.length() == 25 && d[2] == 0x02 && d[3] == 0x15) {
                bcnRssi = (int8_t)d[24];
                unsigned major = (d[20] << 8) | d[21], minor = (d[22] << 8) | d[23];
                setId(Sprintf("iBeacon:%s-%u-%u", beaconUuid(d).c_str(), major, minor), bcnRssi != 3 ? ID_TYPE_IBEACON : ID_TYPE_ECHO_LOST);
            } else if (strManufacturerData.length() >= 4 && d[2] == 0x10) {
                std::string pid = Sprintf("apple:%02x%02x:%u", d[2], d[3], (unsigned)strManufacturerData.length());
                if (haveTxPower) pid += toStr(-txPower);
                setId(pid, ID_TYPE_APPLE_NEARBY);
                mdRssi = BleFingerprintCollection::rxRefRssi + APPLE_TX;
            } else if (strManufacturerData.length() >= 4 && d[2] == 0x12 && strManufacturerData.length() == 29) {
                setId("apple:findmy", ID_TYPE_FINDMY);
                mdRssi = BleFingerprintCollection::rxRefRssi + APPLE_TX;
            } else if (strManufacturerData.length() >= 4) {
                std::string pid = Sprintf("apple:%02x%02x:%u", d[2], d[3], (unsigned)strManufacturerData.length());
                if (haveTxPower) pid += toStr(-txPower);
                setId(pid, ID_TYPE_MISC_APPLE);
                mdRssi = BleFingerprintCollection::rxRefRssi + APPLE_TX;
            }
        } else if (manuf == "05a7")  // Sonos
        {
            mdRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("sonos:" + getMac(), ID_TYPE_SONOS);
        } else if (manuf == "0087")  // Garmin
        {
            mdRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("garmin:" + getMac(), ID_TYPE_GARMIN);
        } else if (manuf == "4d4b")  // iTrack
        {
            mdRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("iTrack:" + getMac(), ID_TYPE_ITRACK);
        } else if (manuf == "0157")  // Mi-fit
        {
            mdRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("mifit:" + getMac(), ID_TYPE_MIFIT);
        } else if (manuf == "0006" && strManufacturerData.length() == 29)  // microsoft
        {
            mdRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId(Sprintf("msft:cdp:%02x%02x", d[3], d[5]), ID_TYPE_MSFT);
        } else if (manuf == "0075")  // samsung
        {
            mdRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("samsung:" + getMac(), ID_TYPE_MISC);
        } else if (manuf == "beac" && strManufacturerData.length() == 26) {
            unsigned major = (d[20] << 8) | d[21], minor = (d[22] << 8) | d[23];
            setId(Sprintf("altBeacon:%s-%u-%u", beaconUuid(d).c_str(), major, minor), ID_TYPE_ABEACON);
            bcnRssi = (int8_t)d[24];
        } else if (manuf != "0000") {
            mdRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            std::string fingerprint = Sprintf("md:%s:%u", manuf.c_str(), (unsigned)strManufacturerData.length());
            if (haveTxPower) fingerprint += toStr(-txPower);
            setId(fingerprint, ID_TYPE_MD);
        }
    }
}

bool BleFingerprint::seen(const Ble::Advert *advertisedDevice) {
    lastSeenMillis = millis();
    reported = false;
    seenCount++;
    fingerprint(advertisedDevice);
    if (ignore || hidden) return false;

    raw = advertisedDevice->getRSSI();
    if (!adaptivePercentileRSSI)
        adaptivePercentileRSSI = std::unique_ptr<AdaptivePercentileRSSI>(new AdaptivePercentileRSSI());
    adaptivePercentileRSSI->addMeasurement(raw - BleFingerprintCollection::rxAdjRssi);
    rssi = adaptivePercentileRSSI->getMedianIQR();
    rssiVar = adaptivePercentileRSSI->getRSSIVariance();
    dist = pow(10, float(get1mRssi() - rssi) / (10.0f * BleFingerprintCollection::absorption));
    distVar = adaptivePercentileRSSI->getDistanceVariance(get1mRssi(), BleFingerprintCollection::absorption);

    if (!added) {
        added = true;
        return true;
    }
    return false;
}

bool BleFingerprint::fill(JsonObject *doc) {
    (*doc)["mac"] = getMac();
    (*doc)["id"] = id;
    if (!name.empty()) (*doc)["name"] = name;

    auto refRssi = get1mRssi();
    if (refRssi > NO_RSSI) (*doc)["rssi@1m"] = refRssi;
    if (rssi > NO_RSSI && (isnormal(rssi) || rssi == 0)) (*doc)["rssi"] = serialized(toStr(rssi));
    (*doc)["rxAdj"] = BleFingerprintCollection::rxAdjRssi;
    if (isnormal(rssiVar)) (*doc)["rssiVar"] = serialized(toStr(rssiVar));

    if (isnormal(dist) || dist == 0) (*doc)["distance"] = serialized(toStr(dist));
    if (isnormal(distVar) || distVar == 0) (*doc)["var"] = serialized(toStr(distVar));
    if (close) (*doc)["close"] = true;

    (*doc)["int"] = (millis() - firstSeenMillis) / seenCount;

    if (mv) (*doc)["mV"] = mv;
    if (battery != 0xFF) (*doc)["batt"] = battery;
    if (temp) (*doc)["temp"] = serialized(toStr(temp));
    if (humidity) (*doc)["rh"] = serialized(toStr(humidity));
    if (!discoveredIrk.empty()) (*doc)["irk"] = discoveredIrk;
    return true;
}

uint8_t BleFingerprint::calculateTimeSlot() {
    if (id.empty()) return 0;
    uint32_t hash = 5381;  // djb2
    for (size_t i = 0; i < id.length(); i++)
        hash = ((hash << 5) + hash) + id[i];
    return hash % MAX_TIME_SLOTS;
}

bool BleFingerprint::report(JsonObject *doc) {
    if (ignore || idType <= ID_TYPE_RAND_MAC || hidden) return false;
    if (reported) return false;

    auto maxDistance = BleFingerprintCollection::maxDistance;
    if (maxDistance > 0 && dist > maxDistance && !isNode)
        return false;

    uint64_t now_ms = getNowMs();

    if (now_ms < nextReportMs) {
        auto movement = fabsf(dist - lastReported);
        auto skipDistance = BleFingerprintCollection::skipDistance;
        if (skipDistance <= 0.0f || movement < skipDistance) return false;

        int64_t rounded_log2 = log2f(roundf(powf(2.0f, movement / skipDistance)));
        uint64_t earlyReportMs = (uint64_t)BleFingerprintCollection::skipMs / std::max((int64_t)2, std::min((int64_t)BleFingerprintCollection::maxDivisor, (int64_t)BleFingerprintCollection::maxDivisor - rounded_log2));
        if (now_ms < nextReportMs - earlyReportMs)
            return false;

        lastReported = dist;
    }

    if (!fill(doc)) return false;
    auto skipMs = (uint64_t)BleFingerprintCollection::skipMs;
    nextReportMs = now_ms + (skipMs ? (skipMs - (now_ms % skipMs)) % skipMs : 0);
    lastReportedMs = now_ms;
    lastReported = dist;
    reported = true;
    return true;
}

bool BleFingerprint::query() {
    if (!allowQuery || isQuerying || enrolling) return false;
    if (rssi < -90) return false;  // Too far away

    auto now = millis();
    if (now - lastSeenMillis > 5) return false;        // Haven't seen lately
    if (now - lastQryMillis < qryDelayMillis) return false;  // Too soon

    isQuerying = true;
    lastQryMillis = now;

    bool success = false;

    Log.printf("%u Query  | %s | %-58s%.1fdBm %lums\r\n", (unsigned)xPortGetCoreID(), getMac().c_str(), id.c_str(), rssi, now - lastSeenMillis);

    {
        Ble::Client client;
        if (client.connect(address, 5000)) {
            if (allowQuery) {
                if (startsWith(id, "flora:"))
                    success = MiFloraHandler::requestData(client, this);
                else
                    success = NameModelHandler::requestData(client, this);
            }

            // Still connected and no IRK yet: try the Resolving Key characteristic.
            if (client.isConnected() && discoveredIrk.empty()) {
                std::string irkBytes = client.read(genericAccessService, resolvingKeyChar);
                if (irkBytes.length() == 16) {
                    discoveredIrk = hexStr(irkBytes);
                    Log.printf("%u IRK    | %s | discovered IRK: %s\r\n", (unsigned)xPortGetCoreID(), getMac().c_str(), discoveredIrk.c_str());
                }
            }
        }
    }

    if (success) {
        qryAttempts = 0;
        qryDelayMillis = BleFingerprintCollection::requeryMs;
    } else {
        qryAttempts++;
        qryDelayMillis = std::min(int(pow(10, qryAttempts)), 60000);
        Log.printf("%u QryErr | %s | %-58s%.1fdBm Try %d, retry after %dms\r\n", (unsigned)xPortGetCoreID(), getMac().c_str(), id.c_str(), rssi, qryAttempts, qryDelayMillis);
    }
    isQuerying = false;
    return true;
}

bool BleFingerprint::queryBatteryIfDue() {
    if (batteryQueryInterval == 0 || isBatteryQuerying) return false;
    auto now = millis();
    if (now - lastBatteryQueryMillis < batteryQueryInterval) return false;
    if (now - lastSeenMillis > 10000) return false;  // not seen in last 10s
    if (rssi < -90) return false;                     // too weak
    queryBattery();
    return true;
}

bool BleFingerprint::queryBattery() {
    isBatteryQuerying = true;
    bool success = false;
    Log.printf("%u Battery| %s | %-58s%.1fdBm\r\n", (unsigned)xPortGetCoreID(), getMac().c_str(), id.c_str(), rssi);
    {
        Ble::Client client;
        if (client.connect(address, 5000)) {
            std::string value = client.read(batteryService, batteryLevelChar);
            if (!value.empty()) {
                uint8_t battery = (uint8_t)value[0];
                if (battery > 100) battery = 100;
                setReport(QueryReport{"battery", "{\"battery\":" + toStr((unsigned)battery) + "}"});
                success = true;
            }
        }
    }
    if (success) {
        lastBatteryQueryMillis = millis();
    } else {
        Log.printf("%u BatteryErr | %s | %-58s%.1fdBm\r\n", (unsigned)xPortGetCoreID(), getMac().c_str(), id.c_str(), rssi);
    }
    isBatteryQuerying = false;
    return success;
}

bool BleFingerprint::shouldCount() {
    if (!close && rssi > CLOSE_RSSI + BleFingerprintCollection::rxAdjRssi) {
        BleFingerprintCollection::Close(this, true);
        close = true;
    } else if (close && rssi < LEFT_RSSI + BleFingerprintCollection::rxAdjRssi) {
        BleFingerprintCollection::Close(this, false);
        close = false;
    }

    bool prevCounting = counting;
    if (ignore || !countable)
        counting = false;
    else if (getMsSinceLastSeen() > BleFingerprintCollection::countMs)
        counting = false;
    else if (counting && dist > BleFingerprintCollection::countExit)
        counting = false;
    else if (!counting && dist <= BleFingerprintCollection::countEnter)
        counting = true;

    if (prevCounting != counting) BleFingerprintCollection::Count(this, counting);
    return counting;
}

void BleFingerprint::expire() { lastSeenMillis = 0; }
