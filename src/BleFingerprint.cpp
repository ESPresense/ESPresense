#include "BleFingerprint.h"

#include <math.h>
#include <stdint.h>

#include "BleFingerprintCollection.h"
#include "MiFloraHandler.h"
#include "NameModelHandler.h"
#include "defaults.h"
#include "mbedtls/aes.h"
#include "rssi.h"
#include "string_utils.h"
#include "util.h"

class ClientCallbacks : public BLEClientCallbacks {
    bool onConnParamsUpdateRequest(NimBLEClient *pClient, const ble_gap_upd_params *params) {
        return true;
    };
};

static ClientCallbacks clientCB;

BleFingerprint::BleFingerprint(BLEAdvertisedDevice *advertisedDevice) {
    firstSeenMillis = millis();
    address = NimBLEAddress(advertisedDevice->getAddress());
    addressType = advertisedDevice->getAddressType();
    raw = advertisedDevice->getRSSI();
    rssi = raw - BleFingerprintCollection::rxAdjRssi;
    adaptivePercentileRSSI.addMeasurement(rssi);
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
    adaptivePercentileRSSI = other.adaptivePercentileRSSI;
}

bool BleFingerprint::shouldHide(const String &s) {
    if (BleFingerprintCollection::include.length() > 0 && !prefixExists(BleFingerprintCollection::include, s)) return true;
    return (BleFingerprintCollection::exclude.length() > 0 && prefixExists(BleFingerprintCollection::exclude, s));
}

bool BleFingerprint::setId(const String &newId, short newIdType, const String &newName) {
    if (idType < 0 && newIdType < 0 && newIdType >= idType) return false;
    if (idType > 0 && newIdType <= idType) return false;
    // Serial.printf("setId: %s %d %s OLD idType: %d\r\n", newId.c_str(), newIdType, newName.c_str(), idType);

    ignore = newIdType < 0;
    idType = newIdType;

    DeviceConfig dc;
    if (BleFingerprintCollection::FindDeviceConfig(newId, dc)) {
        if (dc.calRssi != NO_RSSI)
            calRssi = dc.calRssi;
        if (!dc.alias.isEmpty())
            return setId(dc.alias, ID_TYPE_ALIAS, dc.name);
        if (!dc.name.isEmpty())
            name = dc.name;
    } else if (!newName.isEmpty() && name != newName)
        name = newName;

    if (id != newId) {
        bool newHidden = shouldHide(newId);
        countable = !ignore && !hidden && !BleFingerprintCollection::countIds.isEmpty() && prefixExists(BleFingerprintCollection::countIds, newId);
        bool newQuery = !ignore && !BleFingerprintCollection::query.isEmpty() && prefixExists(BleFingerprintCollection::query, newId);
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
        id = newId;
        isNode = newId.startsWith("node:");
        hidden = newHidden;
        added = false;
        auto timeSlot = calculateTimeSlot();
        uint64_t baseInterval = (uint64_t)BleFingerprintCollection::skipMs;
        uint64_t offset_ms = ((uint64_t)timeSlot * (baseInterval / MAX_TIME_SLOTS));

        uint64_t now_ms = getNowMs();

        // Find the start of the *current* reporting interval boundary
        uint64_t current_interval_start = (now_ms / baseInterval) * baseInterval;

        // Calculate the target time within the current or next interval
        nextReportMs = current_interval_start + offset_ms;
    }

    return true;
}

const String BleFingerprint::getMac() const {
    const auto nativeAddress = address.getNative();
    return Sprintf("%02x%02x%02x%02x%02x%02x", nativeAddress[5], nativeAddress[4], nativeAddress[3], nativeAddress[2], nativeAddress[1], nativeAddress[0]);
}

const int BleFingerprint::get1mRssi() const {
    if (calRssi != NO_RSSI) return calRssi;
    if (bcnRssi != NO_RSSI) return bcnRssi;
    if (mdRssi != NO_RSSI) return mdRssi;
    if (asRssi != NO_RSSI) return asRssi;
    return BleFingerprintCollection::rxRefRssi + DEFAULT_TX;
}

void BleFingerprint::fingerprint(NimBLEAdvertisedDevice *advertisedDevice) {
    if (advertisedDevice->haveName()) {
        const std::string name = advertisedDevice->getName();
        if (!name.empty()) setId(String("name:") + kebabify(name).c_str(), ID_TYPE_NAME, String(name.c_str()));
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

int bt_encrypt_be(const uint8_t *key, const uint8_t *plaintext, uint8_t *enc_data) {
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

struct encryption_block {
    uint8_t key[16];
    uint8_t plain_text[16];
    uint8_t cipher_text[16];
};

bool ble_ll_resolv_rpa(const uint8_t *rpa, const uint8_t *irk) {
    struct encryption_block ecb;

    auto irk32 = (const uint32_t *)irk;
    auto key32 = (uint32_t *)&ecb.key[0];
    auto pt32 = (uint32_t *)&ecb.plain_text[0];

    key32[0] = irk32[0];
    key32[1] = irk32[1];
    key32[2] = irk32[2];
    key32[3] = irk32[3];

    pt32[0] = 0;
    pt32[1] = 0;
    pt32[2] = 0;
    pt32[3] = 0;

    ecb.plain_text[15] = rpa[3];
    ecb.plain_text[14] = rpa[4];
    ecb.plain_text[13] = rpa[5];

    bt_encrypt_be(ecb.key, ecb.plain_text, ecb.cipher_text);

    if (ecb.cipher_text[15] != rpa[0] || ecb.cipher_text[14] != rpa[1] || ecb.cipher_text[13] != rpa[2]) return false;

    // Serial.printf("RPA resolved %d %02x%02x%02x %02x%02x%02x\r\n", err, rpa[0], rpa[1], rpa[2], ecb.cipher_text[15], ecb.cipher_text[14], ecb.cipher_text[13]);

    return true;
}

void BleFingerprint::fingerprintAddress() {
    auto mac = getMac();
    if (!BleFingerprintCollection::knownMacs.isEmpty() && prefixExists(BleFingerprintCollection::knownMacs, mac))
        setId("known:" + mac, ID_TYPE_KNOWN_MAC);
    else {
        switch (addressType) {
            case BLE_ADDR_PUBLIC:
            case BLE_ADDR_PUBLIC_ID:
                setId(mac, ID_TYPE_PUBLIC_MAC);
                break;
            case BLE_ADDR_RANDOM:
            case BLE_ADDR_RANDOM_ID: {
                const auto *naddress = address.getNative();
                if ((naddress[5] & 0xc0) == 0xc0)
                    setId(mac, ID_TYPE_RAND_STATIC_MAC);
                else {
                    auto irks = BleFingerprintCollection::irks;
                    auto it = std::find_if(irks.begin(), irks.end(), [naddress](uint8_t *irk) { return ble_ll_resolv_rpa(naddress, irk); });
                    if (it != irks.end()) {
                        auto irk_hex = hexStr(*it, 16);
                        setId(String("irk:") + irk_hex.c_str(), ID_TYPE_KNOWN_IRK);
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

void BleFingerprint::fingerprintServiceAdvertisements(NimBLEAdvertisedDevice *advertisedDevice, size_t serviceAdvCount, bool haveTxPower, int8_t txPower) {
    for (auto i = 0; i < serviceAdvCount; i++) {
        auto uuid = advertisedDevice->getServiceUUID(i);
#ifdef VERBOSE
        Serial.printf("Verbose | %s | %-58s%.1fdBm AD: %s\r\n", getMac().c_str(), getId().c_str(), rssi, advertisedDevice->getServiceUUID(i).toString().c_str());
#endif
        if (uuid == tileUUID) {
            asRssi = BleFingerprintCollection::rxRefRssi + TILE_TX;
            setId("tile:" + getMac(), ID_TYPE_TILE);
            return;
        } else if (uuid == sonosUUID) {
            asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("sonos:" + getMac(), ID_TYPE_SONOS);
            return;
        } else if (uuid == itagUUID) {
            asRssi = BleFingerprintCollection::rxRefRssi + (haveTxPower ? txPower : ITAG_TX);
            setId("itag:" + getMac(), ID_TYPE_ITAG);
            return;
        } else if (uuid == trackrUUID) {
            asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("trackr:" + getMac(), ID_TYPE_TRACKR);
            return;
        } else if (uuid == tractiveUUID) {
            asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("tractive:" + getMac(), ID_TYPE_TRACTIVE);
            return;
        } else if (uuid == vanmoofUUID) {
            asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("vanmoof:" + getMac(), ID_TYPE_VANMOOF);
            return;
        } else if (uuid == (meaterService)) {
            asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("meater:" + getMac(), ID_TYPE_MEATER);
            return;
        } else if (uuid == nutUUID) {
            asRssi = BleFingerprintCollection::rxRefRssi + (haveTxPower ? txPower : NUT_TX);
            setId("nut:" + getMac(), ID_TYPE_NUT);
            return;
        } else if (uuid == miFloraUUID) {
            asRssi = BleFingerprintCollection::rxRefRssi + (haveTxPower ? txPower : FLORA_TX);
            setId("flora:" + getMac(), ID_TYPE_FLORA);
            return;
        }
    }

    String fingerprint = "ad:";
    asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
    for (int i = 0; i < serviceAdvCount; i++) {
        std::string sid = advertisedDevice->getServiceUUID(i).toString();
        fingerprint = fingerprint + sid.c_str();
    }
    if (haveTxPower) fingerprint = fingerprint + String(-txPower);
    setId(fingerprint, ID_TYPE_AD);
}

void BleFingerprint::fingerprintServiceData(NimBLEAdvertisedDevice *advertisedDevice, size_t serviceDataCount, bool haveTxPower, int8_t txPower) {
    asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
    String fingerprint = "";
    for (int i = 0; i < serviceDataCount; i++) {
        BLEUUID uuid = advertisedDevice->getServiceDataUUID(i);
        std::string strServiceData = advertisedDevice->getServiceData(i);
#ifdef VERBOSE
        Serial.printf("Verbose | %s | %-58s%.1fdBm SD: %s/%s\r\n", getMac().c_str(), getId().c_str(), rssi, uuid.toString().c_str(), hexStr(strServiceData).c_str());
#endif

        if (uuid == exposureUUID) {  // found COVID-19 exposure tracker
            bcnRssi = BleFingerprintCollection::rxRefRssi + EXPOSURE_TX;
            setId("exp:" + String(strServiceData.length()), ID_TYPE_EXPOSURE);
            // disc = hexStr(strServiceData).c_str();
        } else if (uuid == smartTagUUID) {  // found Samsung smart tag
            asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("smarttag:" + String(strServiceData.length()), ID_TYPE_SMARTTAG);
        } else if (uuid == miThermUUID) {
            asRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            if (strServiceData.length() == 15) {  // custom format
                auto serviceData = strServiceData.c_str();
                temp = float(*(int16_t *)(serviceData + 6)) / 100.0f;
                humidity = float(*(uint16_t *)(serviceData + 8)) / 100.0f;
                mv = *(uint16_t *)(serviceData + 10);
                battery = serviceData[12];
#ifdef VERBOSE
                Serial.printf("Temp: %.2f°, Humidity: %.2f%%, mV: %hu, Battery: %hhu%%, flg: 0x%02hhx, cout: %hhu\r\n", temp, humidity, mv, battery, serviceData[14], serviceData[13]);
#endif
                setId("miTherm:" + getMac(), ID_TYPE_MITHERM);
            } else if (strServiceData.length() == 13) {  // format atc1441
                auto serviceData = strServiceData.c_str();
                int16_t x = (serviceData[6] << 8) | serviceData[7];
                temp = float(x) / 10.0f;
                humidity = serviceData[8];
                mv = x = (serviceData[10] << 8) | serviceData[11];
                battery = serviceData[9];

#ifdef VERBOSE
                Serial.printf("Temp: %.2f°, Humidity: %.2f%%, mV: %hu, Battery: %hhu%%, cout: %hhu\r\n", temp, humidity, mv, battery, serviceData[12]);
#endif
                setId("miTherm:" + getMac(), ID_TYPE_MITHERM);
            }
        } else if (uuid == eddystoneUUID && strServiceData.length() > 0) {
            if (strServiceData[0] == EDDYSTONE_URL_FRAME_TYPE && strServiceData.length() <= 18) {
                BLEEddystoneURL oBeacon = BLEEddystoneURL();
                oBeacon.setData(strServiceData);
                bcnRssi = EDDYSTONE_ADD_1M + oBeacon.getPower();
            } else if (strServiceData[0] == EDDYSTONE_TLM_FRAME_TYPE) {
                BLEEddystoneTLM oBeacon = BLEEddystoneTLM();
                oBeacon.setData(strServiceData);
                temp = oBeacon.getTemp();
                mv = oBeacon.getVolt();
#ifdef VERBOSE
                Serial.println(oBeacon.toString().c_str());
#endif
            } else if (strServiceData[0] == 0x00) {
                auto serviceData = strServiceData.c_str();
                int8_t rss0m = *(int8_t *)(serviceData + 1);
                bcnRssi = EDDYSTONE_ADD_1M + rss0m;
                setId(Sprintf("eddy:%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x-%02x%02x%02x%02x%02x%02x",
                              strServiceData[2], strServiceData[3], strServiceData[4], strServiceData[5], strServiceData[6],
                              strServiceData[6], strServiceData[7], strServiceData[8], strServiceData[9], strServiceData[10],
                              strServiceData[11], strServiceData[12], strServiceData[13], strServiceData[14], strServiceData[15],
                              strServiceData[16], strServiceData[17]),
                      ID_TYPE_EBEACON);
            }
        } else {
            fingerprint = fingerprint + uuid.toString().c_str();
        }
    }
    if (!fingerprint.isEmpty()) {
        if (haveTxPower) fingerprint = fingerprint + String(-txPower);
        setId("sd:" + fingerprint, ID_TYPE_SD);
    }
}

void BleFingerprint::fingerprintManufactureData(NimBLEAdvertisedDevice *advertisedDevice, bool haveTxPower, int8_t txPower) {
    std::string strManufacturerData = advertisedDevice->getManufacturerData();
#ifdef VERBOSE
    Serial.printf("Verbose | %s | %-58s%.1fdBm MD: %s\r\n", getMac().c_str(), getId().c_str(), rssi, hexStr(strManufacturerData).c_str());
#endif
    if (strManufacturerData.length() >= 2) {
        String manuf = Sprintf("%02x%02x", strManufacturerData[1], strManufacturerData[0]);

        if (manuf == "004c")  // Apple
        {
            if (strManufacturerData.length() == 25 && strManufacturerData[2] == 0x02 && strManufacturerData[3] == 0x15) {
                BLEBeacon oBeacon = BLEBeacon();
                oBeacon.setData(strManufacturerData);
                bcnRssi = oBeacon.getSignalPower();
                setId(Sprintf("iBeacon:%s-%u-%u", std::string(oBeacon.getProximityUUID()).c_str(), ENDIAN_CHANGE_U16(oBeacon.getMajor()), ENDIAN_CHANGE_U16(oBeacon.getMinor())), bcnRssi != 3 ? ID_TYPE_IBEACON : ID_TYPE_ECHO_LOST);
            } else if (strManufacturerData.length() >= 4 && strManufacturerData[2] == 0x10) {
                String pid = Sprintf("apple:%02x%02x:%u", strManufacturerData[2], strManufacturerData[3], strManufacturerData.length());
                if (haveTxPower) pid += -txPower;
                setId(pid, ID_TYPE_APPLE_NEARBY);
                mdRssi = BleFingerprintCollection::rxRefRssi + APPLE_TX;
            } else if (strManufacturerData.length() >= 4 && strManufacturerData[2] == 0x12 && strManufacturerData.length() == 29) {
                String pid = "apple:findmy";
                setId(pid, ID_TYPE_FINDMY);
                mdRssi = BleFingerprintCollection::rxRefRssi + APPLE_TX;
            } else if (strManufacturerData.length() >= 4) {
                String pid = Sprintf("apple:%02x%02x:%u", strManufacturerData[2], strManufacturerData[3], strManufacturerData.length());
                if (haveTxPower) pid += -txPower;
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
            setId(Sprintf("msft:cdp:%02x%02x", strManufacturerData[3], strManufacturerData[5]), ID_TYPE_MSFT);
            /*disc = Sprintf("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                           strManufacturerData[6], strManufacturerData[7], strManufacturerData[8], strManufacturerData[9], strManufacturerData[10],
                           strManufacturerData[11], strManufacturerData[12], strManufacturerData[13], strManufacturerData[14], strManufacturerData[15],
                           strManufacturerData[16], strManufacturerData[17], strManufacturerData[18], strManufacturerData[19], strManufacturerData[20],
                           strManufacturerData[21], strManufacturerData[22], strManufacturerData[23], strManufacturerData[24], strManufacturerData[25]);
                           */
        } else if (manuf == "0075")  // samsung
        {
            mdRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            setId("samsung:" + getMac(), ID_TYPE_MISC);
        } else if (manuf == "beac" && strManufacturerData.length() == 26) {
            BLEBeacon oBeacon = BLEBeacon();
            oBeacon.setData(strManufacturerData.substr(0, 25));
            setId(Sprintf("altBeacon:%s-%u-%u", std::string(oBeacon.getProximityUUID()).c_str(), ENDIAN_CHANGE_U16(oBeacon.getMajor()), ENDIAN_CHANGE_U16(oBeacon.getMinor())), ID_TYPE_ABEACON);
            bcnRssi = oBeacon.getSignalPower();
        } else if (manuf != "0000") {
            mdRssi = haveTxPower ? BleFingerprintCollection::rxRefRssi + txPower : NO_RSSI;
            String fingerprint = Sprintf("md:%s:%u", manuf.c_str(), strManufacturerData.length());
            if (haveTxPower) fingerprint = fingerprint + String(-txPower);
            setId(fingerprint, ID_TYPE_MD);
        }
    }
}

bool BleFingerprint::seen(BLEAdvertisedDevice *advertisedDevice) {
    lastSeenMillis = millis();
    reported = false;

    seenCount++;

    fingerprint(advertisedDevice);

    if (ignore || hidden) return false;

    raw = advertisedDevice->getRSSI();
    adaptivePercentileRSSI.addMeasurement(raw - BleFingerprintCollection::rxAdjRssi);
    rssi = adaptivePercentileRSSI.getMedianIQR();
    rssiVar = adaptivePercentileRSSI.getRSSIVariance();
    dist = pow(10, float(get1mRssi() - rssi) / (10.0f * BleFingerprintCollection::absorption));
    distVar = adaptivePercentileRSSI.getDistanceVariance(get1mRssi(), BleFingerprintCollection::absorption);

    if (!added) {
        added = true;
        return true;
    }

    return false;
}

bool BleFingerprint::fill(JsonObject *doc) {
    (*doc)[F("mac")] = getMac();
    (*doc)[F("id")] = id;
    if (!name.isEmpty()) (*doc)[F("name")] = name;

    auto refRssi = get1mRssi();
    if (refRssi > NO_RSSI) (*doc)[F("rssi@1m")] = refRssi;
    if (rssi > NO_RSSI && (isnormal(rssi) || rssi == 0)) (*doc)[F("rssi")] = serialized(String(rssi, 2));
    (*doc)[F("rxAdj")] = BleFingerprintCollection::rxAdjRssi;
    if (isnormal(rssiVar)) (*doc)[F("rssiVar")] = serialized(String(rssiVar, 2));

    if (isnormal(dist) || dist == 0) (*doc)[F("distance")] = serialized(String(dist, 2));
    if (isnormal(distVar) || distVar == 0) (*doc)[F("var")] = serialized(String(distVar, 2));
    if (close) (*doc)[F("close")] = true;

    (*doc)[F("int")] = (millis() - firstSeenMillis) / seenCount;

    if (mv) (*doc)[F("mV")] = mv;
    if (battery != 0xFF) (*doc)[F("batt")] = battery;
    if (temp) (*doc)[F("temp")] = serialized(String(temp, 2));
    if (humidity) (*doc)[F("rh")] = serialized(String(humidity, 2));
    return true;
}

uint8_t BleFingerprint::calculateTimeSlot() {
    if (id.isEmpty()) {
        return 0;
    }

    // Use a better hash function that distributes across many more slots
    uint32_t hash = 5381;  // djb2 hash initial value
    for (int i = 0; i < id.length(); i++) {
        hash = ((hash << 5) + hash) + id[i];  // hash * 33 + character
    }

    // Calculate timeslot between 0 and (numTimeSlots-1)
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
        auto movement = abs(dist - lastReported);
        auto skipDistance = BleFingerprintCollection::skipDistance;
        if (skipDistance <= 0.0f || movement < skipDistance) return false;

        int64_t rounded_log2 = log2f(roundf(powf(2.0f, movement / skipDistance)));
        u_int64_t earlyReportMs = (u_int64_t)BleFingerprintCollection::skipMs / max((int64_t)2, min((int64_t)BleFingerprintCollection::maxDivisor, (int64_t)BleFingerprintCollection::maxDivisor - rounded_log2));
        if (now_ms < nextReportMs - earlyReportMs)
            return false;

        lastReported = dist;
    }

    if (!fill(doc)) return false;
    nextReportMs = now_ms + (BleFingerprintCollection::skipMs - (now_ms % BleFingerprintCollection::skipMs)) % BleFingerprintCollection::skipMs;
    lastReportedMs = now_ms;
    lastReported = dist;
    reported = true;
    return true;
}

bool BleFingerprint::query() {
    if (!allowQuery || isQuerying) return false;
    if (rssi < -90) return false; // Too far away

    auto now = millis();
    if (now - lastSeenMillis > 5) return false; // Haven't seen lately
    if (now - lastQryMillis < qryDelayMillis) return false; // Too soon

    isQuerying = true;
    lastQryMillis = now;

    bool success = false;

    Serial.printf("%u Query  | %s | %-58s%.1fdBm %lums\r\n", xPortGetCoreID(), getMac().c_str(), id.c_str(), rssi, now - lastSeenMillis);

    NimBLEClient *pClient = NimBLEDevice::getClientListSize() ? NimBLEDevice::getClientByPeerAddress(address) : nullptr;
    if (!pClient) pClient = NimBLEDevice::getDisconnectedClient();
    if (!pClient) pClient = NimBLEDevice::createClient();
    pClient->setClientCallbacks(&clientCB, false);
    pClient->setConnectionParams(12, 12, 0, 48);
    pClient->setConnectTimeout(5);
    NimBLEDevice::getScan()->stop();
    if (pClient->connect(address)) {
        if (allowQuery) {
            if (id.startsWith("flora:"))
                success = MiFloraHandler::requestData(pClient, this);
            else
                success = NameModelHandler::requestData(pClient, this);
        }
    }

    NimBLEDevice::deleteClient(pClient);

    if (success) {
        qryAttempts = 0;
        qryDelayMillis = BleFingerprintCollection::requeryMs;
    } else {
        qryAttempts++;
        qryDelayMillis = min(int(pow(10, qryAttempts)), 60000);
        Serial.printf("%u QryErr | %s | %-58s%.1fdBm Try %d, retry after %dms\r\n", xPortGetCoreID(), getMac().c_str(), id.c_str(), rssi, qryAttempts, qryDelayMillis);
    }
    isQuerying = false;
    return true;
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

    if (prevCounting != counting) {
        BleFingerprintCollection::Count(this, counting);
    }

    return counting;
}

void BleFingerprint::expire() {
    lastSeenMillis = 0;
}
