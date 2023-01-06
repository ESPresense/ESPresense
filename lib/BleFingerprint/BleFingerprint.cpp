#include "BleFingerprint.h"

#include "BleFingerprintCollection.h"
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

bool BleFingerprint::shouldHide(const String &s) {
    if (BleFingerprintCollection::include.length() > 0 && !prefixExists(BleFingerprintCollection::include, s)) return true;
    return (BleFingerprintCollection::exclude.length() > 0 && prefixExists(BleFingerprintCollection::exclude, s));
}

bool BleFingerprint::setId(const String &newId, short newIdType, const String &newName) {
    if (idType < 0 && newIdType < 0 && newIdType >= idType) return false;
    if (idType > 0 && newIdType <= idType) return false;
    // Serial.printf("setId: %s %d %s OLD idType: %d\n", newId.c_str(), newIdType, newName.c_str(), idType);

    ignore = newIdType < 0;
    idType = newIdType;

    DeviceConfig dc;
    if (BleFingerprintCollection::FindDeviceConfig(newId, dc)) {
        if (!dc.alias.isEmpty())
            return setId(dc.alias, ID_TYPE_ALIAS, dc.name);
        if (!dc.name.isEmpty())
            name = dc.name;
    } else if (!newName.isEmpty() && name != newName)
        name = newName;

    if (id != newId) {
        hidden = shouldHide(newId);
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
        added = false;
    }

    return true;
}

int BleFingerprint::get1mRssi() const {
    if (calRssi != NO_RSSI) return calRssi + BleFingerprintCollection::rxAdjRssi;
    if (bcnRssi != NO_RSSI) return bcnRssi + BleFingerprintCollection::rxAdjRssi;
    if (mdRssi != NO_RSSI) return mdRssi + BleFingerprintCollection::rxAdjRssi;
    if (asRssi != NO_RSSI) return asRssi + BleFingerprintCollection::rxAdjRssi;
    return BleFingerprintCollection::rxRefRssi + DEFAULT_TX + BleFingerprintCollection::rxAdjRssi;
}

BleFingerprint::BleFingerprint(BLEAdvertisedDevice *advertisedDevice, float fcmin, float beta, float dcutoff) : oneEuro{OneEuroFilter<float, unsigned long>(1, fcmin, beta, dcutoff)} {
    firstSeenMillis = millis();
    address = NimBLEAddress(advertisedDevice->getAddress());
    addressType = advertisedDevice->getAddressType();
    newest = recent = oldest = rssi = advertisedDevice->getRSSI();
    seenCount = 1;

    fingerprintAddress();
}

void BleFingerprint::fingerprint(NimBLEAdvertisedDevice *advertisedDevice) {
    if (advertisedDevice->haveName()) {
        std::string name = advertisedDevice->getName();
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
    mbedtls_aes_context s = {0};
    mbedtls_aes_init(&s);

    if (mbedtls_aes_setkey_enc(&s, key, 128) != 0) {
        mbedtls_aes_free(&s);
        return BLE_HS_EUNKNOWN;
    }

    if (mbedtls_aes_crypt_ecb(&s, MBEDTLS_AES_ENCRYPT, plaintext, enc_data) != 0) {
        mbedtls_aes_free(&s);
        return BLE_HS_EUNKNOWN;
    }

    mbedtls_aes_free(&s);
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

    auto err = bt_encrypt_be(ecb.key, ecb.plain_text, ecb.cipher_text);

    if (ecb.cipher_text[15] != rpa[0] || ecb.cipher_text[14] != rpa[1] || ecb.cipher_text[13] != rpa[2]) return false;

    // Serial.printf("RPA resolved %d %02x%02x%02x %02x%02x%02x\n", err, rpa[0], rpa[1], rpa[2], ecb.cipher_text[15], ecb.cipher_text[14], ecb.cipher_text[13]);

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
            case BLE_ADDR_RANDOM: {
                auto naddress = address.getNative();
                auto irks = BleFingerprintCollection::irks;
                auto it = std::find_if(irks.begin(), irks.end(), [naddress](uint8_t *irk) { return ble_ll_resolv_rpa(naddress, irk); });
                if (it != irks.end()) {
                    auto irk_hex = hexStr(*it, 16);
                    setId(String("irk:") + irk_hex.c_str(), ID_TYPE_KNOWN_IRK);
                    break;
                }
            }
            default:
                setId(mac, ID_TYPE_RAND_MAC);
                break;
        }
    }
}

void BleFingerprint::fingerprintServiceAdvertisements(NimBLEAdvertisedDevice *advertisedDevice, size_t serviceAdvCount, bool haveTxPower, int8_t txPower) {
    for (size_t i = 0; i < serviceAdvCount; i++) {
        auto uuid = advertisedDevice->getServiceUUID(i);
#ifdef VERBOSE
        Serial.printf("Verbose | MAC: %s, ID: %-58s%ddBm AD: %s\n", getMac().c_str(), getId().c_str(), rssi, advertisedDevice->getServiceUUID(i).toString().c_str());
#endif
        if (uuid == roomAssistantService) {
            asRssi = BleFingerprintCollection::rxRefRssi + RM_ASST_TX;
            if (!rmAsst) {
                rmAsst = true;
                if (didQuery) {
                    qryDelayMillis = 0;
                    qryAttempts = 0;
                    didQuery = false;
                }
            }
            return;
        } else if (uuid == tileUUID) {
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
        Serial.printf("Verbose | MAC: %s, ID: %-58s%ddBm SD: %s/%s\n", getMac().c_str(), getId().c_str(), rssi, uuid.toString().c_str(), hexStr(strServiceData).c_str());
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
                Serial.printf("Temp: %.1f°, Humidity: %.1f%%, mV: %hu, Battery: %hhu%%, flg: 0x%02hhx, cout: %hhu\n", temp, humidity, mv, battery, serviceData[14], serviceData[13]);
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
                Serial.printf("Temp: %.1f°, Humidity: %.1f%%, mV: %hu, Battery: %hhu%%, cout: %hhu\n", temp, humidity, mv, battery, serviceData[12]);
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
    Serial.printf("Verbose | MAC: %s, ID: %-58s%ddBm MD: %s\n", getMac().c_str(), getId().c_str(), rssi, hexStr(strManufacturerData).c_str());
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
                disc = hexStr(strManufacturerData.substr(4)).c_str();
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

bool BleFingerprint::filter() {
    Reading<float, unsigned long> inter1, inter2;
    inter1.timestamp = millis();
    inter1.value = raw;

    return oneEuro.push(&inter1, &inter2) && diffFilter.push(&inter2, &output);
}

bool BleFingerprint::seen(BLEAdvertisedDevice *advertisedDevice) {
    lastSeenMillis = millis();
    reported = false;

    seenCount++;

    fingerprint(advertisedDevice);

    if (ignore) return false;

    oldest = recent;
    recent = newest;
    newest = advertisedDevice->getRSSI();
    rssi = median_of_3(oldest, recent, newest);

    float ratio = (get1mRssi() - rssi) / (10.0f * BleFingerprintCollection::absorption);
    raw = pow(10, ratio);
    if (filter()) hasValue = true;

    if (!close && newest > CLOSE_RSSI) {
        BleFingerprintCollection::Close(this, true);
        close = true;
    } else if (close && newest < LEFT_RSSI) {
        BleFingerprintCollection::Close(this, false);
        close = false;
    }

    if (!added) {
        added = true;
        return true;
    }

    return false;
}

void BleFingerprint::setInitial(int initalRssi, float initalDistance) {
    newest = recent = oldest = rssi = initalRssi;
    raw = initalDistance;
    hasValue = filter() || filter();
}

void BleFingerprint::fill(JsonObject *doc) {
    (*doc)[F("id")] = id;
    if (!name.isEmpty()) (*doc)[F("name")] = name;
    if (!disc.isEmpty()) (*doc)[F("disc")] = disc;
    if (idType) (*doc)[F("idType")] = idType;

    (*doc)[F("rssi@1m")] = get1mRssi();
    (*doc)[F("rssi")] = rssi;

    (*doc)[F("raw")] = serialized(String(raw, 2));
    (*doc)[F("distance")] = serialized(String(output.value.position, 2));
    (*doc)[F("speed")] = serialized(String(output.value.speed * 1e3f, 2));
    (*doc)[F("mac")] = SMacf(address);

    (*doc)[F("interval")] = (millis() - firstSeenMillis) / seenCount;

    if (mv) (*doc)[F("mV")] = mv;
    if (battery != 0xFF) (*doc)[F("batt")] = battery;
    if (temp) (*doc)[F("temp")] = serialized(String(temp, 1));
    if (humidity) (*doc)[F("rh")] = serialized(String(humidity, 1));
}

bool BleFingerprint::report(JsonObject *doc) {
    if (ignore || idType <= ID_TYPE_RAND_MAC || hidden)
        return false;

    if (reported || !hasValue)
        return false;

    auto maxDistance = BleFingerprintCollection::maxDistance;
    if (maxDistance > 0 && output.value.position > maxDistance)
        return false;

    auto now = millis();
    if ((abs(output.value.position - lastReported) < BleFingerprintCollection::skipDistance) && (lastReportedMillis > 0) && (now - lastReportedMillis < BleFingerprintCollection::skipMs))
        return false;

    lastReportedMillis = now;
    lastReported = output.value.position;
    reported = true;
    fill(doc);
    return true;
}

bool BleFingerprint::query() {
    if (!(allowQuery || rmAsst) || didQuery) return false;
    if (rssi < -90) return false;
    auto now = millis();

    if (now - lastSeenMillis > 5) return false;

    if (now - lastQryMillis < qryDelayMillis) return false;
    didQuery = true;
    lastQryMillis = now;

    bool success = false;

    Serial.printf("%u Query | MAC: %s, ID: %-58s%ddBm %lums\n", xPortGetCoreID(), getMac().c_str(), id.c_str(), rssi, now - lastSeenMillis);

    NimBLEClient *pClient = NimBLEDevice::getClientListSize() ? NimBLEDevice::getClientByPeerAddress(address) : nullptr;
    if (!pClient) pClient = NimBLEDevice::getDisconnectedClient();
    if (!pClient) pClient = NimBLEDevice::createClient();
    pClient->setClientCallbacks(&clientCB, false);
    pClient->setConnectionParams(12, 12, 0, 48);
    pClient->setConnectTimeout(5);
    NimBLEDevice::getScan()->stop();
    if (pClient->connect(address)) {
        bool iphone = true;
        if (allowQuery) {
            std::string sMdl = pClient->getValue(deviceInformationService, modelChar);
            std::string sName = pClient->getValue(genericAccessService, nameChar);
            iphone = sMdl.find("iPhone") == 0;
            if (!sName.empty() && sMdl.find(sName) == std::string::npos && sName != "Apple Watch") {
                if (setId(String("name:") + kebabify(sName).c_str(), ID_TYPE_QUERY_NAME, String(sName.c_str()))) {
                    Serial.printf("\u001b[38;5;104m%u Name  | MAC: %s, ID: %-58s%ddBm %s\u001b[0m\n", xPortGetCoreID(), getMac().c_str(), id.c_str(), rssi, sName.c_str());
                }
                success = true;
            }

            if (!sMdl.empty()) {
                if (setId(String("apple:") + kebabify(sMdl).c_str(), ID_TYPE_QUERY_MODEL, String(sMdl.c_str()))) {
                    Serial.printf("\u001b[38;5;136m%u Model | MAC: %s, ID: %-58s%ddBm %s\u001b[0m\n", xPortGetCoreID(), getMac().c_str(), id.c_str(), rssi, sMdl.c_str());
                }
                success = true;
            }
        }

        if (rmAsst || iphone)  // For some reason we often don't get room assistant's service advertisement
        {
            std::string sRmAst = pClient->getValue(roomAssistantService, rootAssistantCharacteristic);
            if (!sRmAst.empty()) {
                if (setId(String("roomAssistant:") + kebabify(sRmAst).c_str(), ID_TYPE_RM_ASST)) {
                    Serial.printf("\u001b[38;5;129m%u RmAst | MAC: %s, ID: %-58s%ddBm %s\u001b[0m\n", xPortGetCoreID(), getMac().c_str(), id.c_str(), rssi, sRmAst.c_str());
                }
                success = true;
            }
        }
    }

    NimBLEDevice::deleteClient(pClient);

    if (success) return true;

    qryAttempts++;
    qryDelayMillis = min(int(pow(10, qryAttempts)), 60000);
    Serial.printf("%u QryErr| MAC: %s, ID: %-58s%ddBm Try %d, retry after %dms\n", xPortGetCoreID(), getMac().c_str(), id.c_str(), rssi, qryAttempts, qryDelayMillis);

    didQuery = false;

    return true;
}

bool BleFingerprint::shouldCount() {
    bool prevCounting = counting;
    if (ignore || !countable || !hasValue)
        counting = false;
    else if (getMsSinceLastSeen() > BleFingerprintCollection::countMs)
        counting = false;
    else if (counting && output.value.position > BleFingerprintCollection::countExit)
        counting = false;
    else if (!counting && output.value.position <= BleFingerprintCollection::countEnter)
        counting = true;

    if (prevCounting != counting) {
        BleFingerprintCollection::Count(this, counting);
    }

    return counting;
}

void BleFingerprint::expire() {
    lastSeenMillis = 0;
}
