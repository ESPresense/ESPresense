#include "BleFingerprint.h"
#include "BleFingerprintCollection.h"
#include "rssi.h"
#include "strings.h"
#include "util.h"

bool BleFingerprint::shouldHide(const String& s)
{
    if (BleFingerprintCollection::include.length() > 0 && !prefixExists(BleFingerprintCollection::include, s)) return true;
    return (BleFingerprintCollection::exclude.length() > 0 && prefixExists(BleFingerprintCollection::exclude, s));
}

bool BleFingerprint::setId(const String& newId, short newIdType, const String& newName)
{
    if (newIdType < idType && idType > 0) return false;

    hidden = shouldHide(newId);
    ignore = newIdType < 0;

    if (!allowQuery && !ignore)
    {
        if (!BleFingerprintCollection::query.isEmpty() && prefixExists(BleFingerprintCollection::query, newId))
        {
            allowQuery = true;
            qryAttempts = 0;
            if (rssi < -60)
            {
                qryDelayMillis = 5000;
                lastQryMillis = millis();
            }
        }
    }

    countable = !ignore && !BleFingerprintCollection::countIds.isEmpty() && prefixExists(BleFingerprintCollection::countIds, newId);
    id = newId;
    idType = newIdType;
    if (!newName.isEmpty()) name = newName;
    return true;
}

int BleFingerprint::get1mRssi() const
{
    if (calRssi != NO_RSSI) return calRssi;
    if (mdRssi != NO_RSSI) return mdRssi;
    if (asRssi != NO_RSSI) return asRssi;
    return BleFingerprintCollection::refRssi + DEFAULT_TX;
}

BleFingerprint::BleFingerprint(const BleFingerprintCollection *parent, BLEAdvertisedDevice *advertisedDevice, float fcmin, float beta, float dcutoff) : oneEuro{OneEuroFilter<float, unsigned long>(1, fcmin, beta, dcutoff)}
{
    firstSeenMillis = millis();
    address = NimBLEAddress(advertisedDevice->getAddress());
    newest = recent = oldest = rssi = advertisedDevice->getRSSI();
    seenCount = 1;

    auto mac = getMac();
    if (!BleFingerprintCollection::knownMacs.isEmpty() && prefixExists(BleFingerprintCollection::knownMacs, mac))
        setId("known:" + mac, ID_TYPE_KNOWN_MAC);
    else
    {
        switch (advertisedDevice->getAddressType())
        {
        case BLE_ADDR_PUBLIC:
        case BLE_ADDR_PUBLIC_ID:
            setId(mac, ID_TYPE_PUBLIC_MAC);
            break;
        default:
            setId(mac, ID_TYPE_MAC);
            break;
        }
    }
}

void BleFingerprint::fingerprint(NimBLEAdvertisedDevice *advertisedDevice)
{
    if (advertisedDevice->haveName())
        setId(String("name:") + kebabify(advertisedDevice->getName()).c_str(), ID_TYPE_NAME, String(advertisedDevice->getName().c_str()));

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

void BleFingerprint::fingerprintServiceAdvertisements(NimBLEAdvertisedDevice *advertisedDevice, size_t serviceAdvCount, bool haveTxPower, int8_t txPower)
{
    for (size_t i = 0; i < serviceAdvCount; i++)
    {
        auto uuid = advertisedDevice->getServiceUUID(i);
#ifdef VERBOSE
        Serial.printf("Verbose | %-58sAD: %s\n", getId().c_str(), advertisedDevice->getServiceUUID(i).toString().c_str());
#endif
        if (uuid == roomAssistantService)
        {
            asRssi = BleFingerprintCollection::refRssi + RM_ASST_TX;
            if (!rmAsst)
            {
                rmAsst = true;
                if (didQuery)
                {
                    qryDelayMillis = 0;
                    qryAttempts = 0;
                    didQuery = false;
                }
            }
            return;
        }
        else if (uuid == tileUUID)
        {
            asRssi = BleFingerprintCollection::refRssi + TILE_TX;
            setId("tile:" + getMac(), ID_TYPE_TILE);
            return;
        }
        else if (uuid == sonosUUID)
        {
            asRssi = haveTxPower ? BleFingerprintCollection::refRssi + txPower : NO_RSSI;
            setId("sonos:" + getMac(), ID_TYPE_SONOS);
            return;
        }
        else if (uuid == itagUUID)
        {
            asRssi = BleFingerprintCollection::refRssi + (haveTxPower ? txPower : ITAG_TX);
            setId("itag:" + getMac(), ID_TYPE_ITAG);
            return;
        }
        else if (uuid == trackrUUID)
        {
            asRssi = haveTxPower ? BleFingerprintCollection::refRssi + txPower : NO_RSSI;
            setId("trackr:" + getMac(), ID_TYPE_TRACKR);
            return;
        }
        else if (uuid == vanmoofUUID)
        {
            asRssi = haveTxPower ? BleFingerprintCollection::refRssi + txPower : NO_RSSI;
            setId("vanmoof:" + getMac(), ID_TYPE_VANMOOF);
            return;
        }
        else if (uuid == (meaterService))
        {
            asRssi = haveTxPower ? BleFingerprintCollection::refRssi + txPower : NO_RSSI;
            setId("meater:" + getMac(), ID_TYPE_MEATER);
            return;
        }
    }

    String fingerprint = "ad:";
    asRssi = haveTxPower ? BleFingerprintCollection::refRssi + txPower : NO_RSSI;
    for (int i = 0; i < serviceAdvCount; i++)
    {
        std::string sid = advertisedDevice->getServiceUUID(i).toString();
        fingerprint = fingerprint + sid.c_str();
    }
    if (haveTxPower) fingerprint = fingerprint + String(-txPower);
    setId(fingerprint, ID_TYPE_AD);
}

void BleFingerprint::fingerprintServiceData(NimBLEAdvertisedDevice *advertisedDevice, size_t serviceDataCount, bool haveTxPower, int8_t txPower)
{
    asRssi = haveTxPower ? BleFingerprintCollection::refRssi + txPower : NO_RSSI;
    String fingerprint = "sd:";
    for (int i = 0; i < serviceDataCount; i++)
    {
        BLEUUID uuid = advertisedDevice->getServiceDataUUID(i);
        std::string strServiceData = advertisedDevice->getServiceData(i);
#ifdef VERBOSE
        Serial.printf("Verbose | %-58sSD: %s/%s\n", getId().c_str(), uuid.toString().c_str(), hexStr(strServiceData).c_str());
#endif

        if (uuid == exposureUUID)
        { // found COVID-19 exposure tracker
            calRssi = BleFingerprintCollection::refRssi + EXPOSURE_TX;
            setId("exp:" + String(strServiceData.length()), ID_TYPE_EXPOSURE);
            disc = hexStr(strServiceData).c_str();
        }
        else if (uuid == miThermUUID)
        {
            asRssi = haveTxPower ? BleFingerprintCollection::refRssi + txPower : NO_RSSI;
            if (strServiceData.length() == 15)
            { // custom format
                auto serviceData = strServiceData.c_str();
                temp = float(*(int16_t *)(serviceData + 6)) / 100.0f;
                humidity = float(*(uint16_t *)(serviceData + 8)) / 100.0f;
                mv = *(uint16_t *)(serviceData + 10);
#ifdef VERBOSE
                Serial.printf("Temp: %.2f°, Humidity: %.2f%%, Vbatt: %d, Battery: %d%%, flg: 0x%02x, cout: %d\n", temp, humidity, mv, serviceData[12], serviceData[14], serviceData[13]);
#endif
                setId("miTherm:" + getMac(), ID_TYPE_MITHERM);
            }
            else if (strServiceData.length() == 13)
            { // format atc1441
                auto serviceData = strServiceData.c_str();
                int16_t x = (serviceData[6] << 8) | serviceData[7];
                temp = float(x) / 10.0f;
                mv = x = (serviceData[10] << 8) | serviceData[11];
#ifdef VERBOSE
                Serial.printf("Temp: %.1f°, Humidity: %d%%, Vbatt: %d, Battery: %d%%, cout: %d\n", temp, serviceData[8], mv, serviceData[9], serviceData[12]);
#endif
                setId("miTherm:" + getMac(), ID_TYPE_MITHERM);
            }
        }
        else if (uuid == eddystoneUUID && strServiceData.length() > 0)
        {
            if (strServiceData[0] == EDDYSTONE_URL_FRAME_TYPE && strServiceData.length() <= 18)
            {
                BLEEddystoneURL oBeacon = BLEEddystoneURL();
                oBeacon.setData(strServiceData);
                calRssi = EDDYSTONE_ADD_1M + oBeacon.getPower();
            }
            else if (strServiceData[0] == EDDYSTONE_TLM_FRAME_TYPE)
            {
                BLEEddystoneTLM oBeacon = BLEEddystoneTLM();
                oBeacon.setData(strServiceData);
                temp = oBeacon.getTemp();
                mv = oBeacon.getVolt();
#ifdef VERBOSE
                Serial.println(oBeacon.toString().c_str());
#endif
            }
            else if (strServiceData[0] == 0x00)
            {
                auto serviceData = strServiceData.c_str();
                int8_t rss0m = *(int8_t *)(serviceData + 1);
                calRssi = EDDYSTONE_ADD_1M + rss0m;
                setId(Sprintf("eddy:%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x-%02x%02x%02x%02x%02x%02x",
                              strServiceData[2], strServiceData[3], strServiceData[4], strServiceData[5], strServiceData[6],
                              strServiceData[6], strServiceData[7], strServiceData[8], strServiceData[9], strServiceData[10],
                              strServiceData[11], strServiceData[12], strServiceData[13], strServiceData[14], strServiceData[15],
                              strServiceData[16], strServiceData[17]),
                      ID_TYPE_EBEACON);
            }
        }
        else
        {
            fingerprint = fingerprint + uuid.toString().c_str();
        }
    }
    if (haveTxPower)
        fingerprint = fingerprint + String(-txPower);
    setId(fingerprint, ID_TYPE_SD);
}

void BleFingerprint::fingerprintManufactureData(NimBLEAdvertisedDevice *advertisedDevice, bool haveTxPower, int8_t txPower)
{
    std::string strManufacturerData = advertisedDevice->getManufacturerData();
#ifdef VERBOSE
    Serial.printf("Verbose | %-58sMD: %s\n", getId().c_str(), hexStr(strManufacturerData).c_str());
#endif
    if (strManufacturerData.length() >= 2)
    {
        String manuf = Sprintf("%02x%02x", strManufacturerData[1], strManufacturerData[0]);

        if (manuf == "004c") // Apple
        {
            if (strManufacturerData.length() == 25 && strManufacturerData[2] == 0x02 && strManufacturerData[3] == 0x15)
            {
                BLEBeacon oBeacon = BLEBeacon();
                oBeacon.setData(strManufacturerData);
                setId(Sprintf("iBeacon:%s-%d-%d", std::string(oBeacon.getProximityUUID()).c_str(), ENDIAN_CHANGE_U16(oBeacon.getMajor()), ENDIAN_CHANGE_U16(oBeacon.getMinor())), ID_TYPE_IBEACON);
                calRssi = oBeacon.getSignalPower();
            }
            else if (strManufacturerData.length() >= 4 && strManufacturerData[2] == 0x10)
            {
                String pid = Sprintf("apple:%02x%02x:%d", strManufacturerData[2], strManufacturerData[3], strManufacturerData.length());
                if (haveTxPower) pid += -txPower;
                setId(pid, ID_TYPE_APPLE_NEARBY);
                disc = hexStr(strManufacturerData.substr(4)).c_str();
                mdRssi = BleFingerprintCollection::refRssi + APPLE_TX;
            }
            else if (strManufacturerData.length() >= 4)
            {
                if (haveTxPower)
                    setId(Sprintf("apple:%02x%02x:%d%d", strManufacturerData[2], strManufacturerData[3], strManufacturerData.length(), -txPower), ID_TYPE_MISC_APPLE);
                else
                    setId(Sprintf("apple:%02x%02x:%d", strManufacturerData[2], strManufacturerData[3], strManufacturerData.length()), ID_TYPE_MISC_APPLE);
                mdRssi = BleFingerprintCollection::refRssi + APPLE_TX;
            }
        }
        else if (manuf == "05a7") // Sonos
        {
            mdRssi = haveTxPower ? BleFingerprintCollection::refRssi + txPower : NO_RSSI;
            setId("sonos:" + getMac(), ID_TYPE_SONOS);
        }
        else if (manuf == "0157") // Mi-fit
        {
            mdRssi = haveTxPower ? BleFingerprintCollection::refRssi + txPower : NO_RSSI;
            setId("mifit:" + getMac(), ID_TYPE_MIFIT);
        }
        else if (manuf == "0006" && strManufacturerData.length() == 29) // microsoft
        {
            mdRssi = haveTxPower ? BleFingerprintCollection::refRssi + txPower : NO_RSSI;
            setId(Sprintf("msft:cdp:%02x%02x", strManufacturerData[3], strManufacturerData[5]), ID_TYPE_MSFT);
            disc = Sprintf("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                           strManufacturerData[6], strManufacturerData[7], strManufacturerData[8], strManufacturerData[9], strManufacturerData[10],
                           strManufacturerData[11], strManufacturerData[12], strManufacturerData[13], strManufacturerData[14], strManufacturerData[15],
                           strManufacturerData[16], strManufacturerData[17], strManufacturerData[18], strManufacturerData[19], strManufacturerData[20],
                           strManufacturerData[21], strManufacturerData[22], strManufacturerData[23], strManufacturerData[24], strManufacturerData[25]);
        }
        else if (manuf == "0075") // samsung
        {
            mdRssi = haveTxPower ? BleFingerprintCollection::refRssi + txPower : NO_RSSI;
            setId("samsung:" + getMac(), ID_TYPE_MISC);
        }
        else if (manuf == "beac" && strManufacturerData.length() == 26)
        {
            BLEBeacon oBeacon = BLEBeacon();
            oBeacon.setData(strManufacturerData.substr(0, 25));
            setId(Sprintf("altBeacon:%s-%d-%d", std::string(oBeacon.getProximityUUID()).c_str(), ENDIAN_CHANGE_U16(oBeacon.getMajor()), ENDIAN_CHANGE_U16(oBeacon.getMinor())), ID_TYPE_ABEACON);
            calRssi = oBeacon.getSignalPower();
        }
        else if (manuf != "0000")
        {
            mdRssi = haveTxPower ? BleFingerprintCollection::refRssi + txPower : NO_RSSI;
            String fingerprint = Sprintf("md:%s:%d", manuf.c_str(), strManufacturerData.length());
            if (haveTxPower)
                fingerprint = fingerprint + String(-txPower);
            setId(fingerprint, ID_TYPE_MD);
        }
    }
}

bool BleFingerprint::filter()
{
    Reading<float, unsigned long> inter1, inter2;
    inter1.timestamp = millis();
    inter1.value = raw;

    return oneEuro.push(&inter1, &inter2) && diffFilter.push(&inter2, &output);
}

bool BleFingerprint::seen(BLEAdvertisedDevice *advertisedDevice)
{
    lastSeenMillis = millis();
    reported = false;

    seenCount++;

    if (ignore) return false;

    oldest = recent;
    recent = newest;
    newest = advertisedDevice->getRSSI();
    rssi = median_of_3(oldest, recent, newest);

    fingerprint(advertisedDevice);

    if (ignore) return false;

    float ratio = (get1mRssi() - rssi) / (10.0f * BleFingerprintCollection::absorption);
    raw = pow(10, ratio);
    if (filter()) hasValue = true;

    if (!close && newest > CLOSE_RSSI)
    {
        GUI::close(this);
        close = true;
    }
    else if (close && newest < LEFT_RSSI)
    {
        GUI::left(this);
        close = false;
    }

    if (!added)
    {
        added = true;
        return true;
    }

    return false;
}

void BleFingerprint::setInitial(int initalRssi, float initalDistance)
{
    newest = recent = oldest = rssi = initalRssi;
    raw = initalDistance;
    hasValue = filter() || filter();
}

bool BleFingerprint::report(JsonDocument *doc)
{
    if (ignore || idType == 0 || hidden)
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

    (*doc)[F("id")] = id;
    if (!name.isEmpty()) (*doc)[F("name")] = name;
    if (!disc.isEmpty()) (*doc)[F("disc")] = disc;
    if (idType) (*doc)[F("idType")] = idType;

    (*doc)[F("rssi@1m")] = get1mRssi();
    (*doc)[F("rssi")] = rssi;

    (*doc)[F("raw")] = round(raw * 100.0f) / 100.0f;
    (*doc)[F("distance")] = round(output.value.position * 100.0f) / 100.0f;
    (*doc)[F("speed")] = round(output.value.speed * 1e5f) / 100.0f;
    (*doc)[F("mac")] = SMacf(address);

    if (mv) (*doc)[F("mV")] = mv;
    if (temp) (*doc)[F("temp")] = temp;
    if (humidity) (*doc)[F("rh")] = humidity;

    return true;
}

bool BleFingerprint::query()
{
    if (!(allowQuery || rmAsst) || didQuery) return false;
    if (rssi < -90) return false;
    auto now = millis();

    if (now - lastSeenMillis > 2) return false;

    if (now - lastQryMillis < qryDelayMillis) return false;
    didQuery = true;
    lastQryMillis = now;

    bool success = false;

    Serial.printf("%d Query | MAC: %s, ID: %-60s %lums\n", xPortGetCoreID(), getMac().c_str(), id.c_str(), now - lastSeenMillis);

    NimBLEClient *pClient = NimBLEDevice::getClientListSize() ? NimBLEDevice::getClientByPeerAddress(address) : nullptr;
    if (!pClient) pClient = NimBLEDevice::getDisconnectedClient();
    if (!pClient) pClient = NimBLEDevice::createClient();
    pClient->setConnectTimeout(5);
    if (pClient->connect(address))
    {
        bool iphone = true;
        if (allowQuery)
        {
            std::string sMdl = pClient->getValue(deviceInformationService, modelChar);
            std::string sName = pClient->getValue(genericAccessService, nameChar);
            iphone = sMdl.find("iPhone") == 0;
            if (!sName.empty() && sMdl.find(sName) == std::string::npos && sName != "Apple Watch")
            {
                if (setId(String("name:") + kebabify(sName).c_str(), ID_TYPE_QUERY_NAME, String(sName.c_str())))
                {
                    Serial.printf("\u001b[38;5;104m%d Name  | MAC: %s, ID: %-60s %s\u001b[0m\n", xPortGetCoreID(), getMac().c_str(), id.c_str(), sName.c_str());
                }
                success = true;
            }

            if (!sMdl.empty())
            {
                if (setId(String("apple:") + kebabify(sMdl).c_str(), ID_TYPE_QUERY_MODEL, String(sMdl.c_str())))
                {
                    Serial.printf("\u001b[38;5;136m%d Model | MAC: %s, ID: %-60s %s\u001b[0m\n", xPortGetCoreID(), getMac().c_str(), id.c_str(), sMdl.c_str());
                }
                success = true;
            }
        }

        if (rmAsst || iphone) // For some reason we often don't get room assistant's service advertisement
        {
            std::string sRmAst = pClient->getValue(roomAssistantService, rootAssistantCharacteristic);
            if (!sRmAst.empty())
            {
                if (setId(String("roomAssistant:") + kebabify(sRmAst).c_str(), ID_TYPE_RM_ASST))
                {
                    Serial.printf("\u001b[38;5;129m%d RmAst | MAC: %s, ID: %-60s %s\u001b[0m\n", xPortGetCoreID(), getMac().c_str(), id.c_str(), sRmAst.c_str());
                }
                success = true;
            }
        }
    }

    NimBLEDevice::deleteClient(pClient);

    if (success) return true;

    qryAttempts++;
    Serial.printf("%d QryErr| MAC: %s, ID: %-60s rssi %d, try %d, retry after %dms\n", xPortGetCoreID(), getMac().c_str(), id.c_str(), rssi, qryAttempts, qryDelayMillis);

    if (qryDelayMillis < 30000)
        qryDelayMillis += (1000 * qryAttempts * qryAttempts);
    else
        qryDelayMillis = 30000;
    didQuery = false;

    return true;
}

bool BleFingerprint::shouldCount()
{
    bool prevCounting = counting;
    if (ignore || !countable)
        counting = false;
    else if (getMsSinceFirstSeen() <= BleFingerprintCollection::countMs || getMsSinceLastSeen() > BleFingerprintCollection::countMs)
        counting = false;
    else if (counting && output.value.position > BleFingerprintCollection::countExit)
        counting = false;
    else if (!counting && output.value.position <= BleFingerprintCollection::countEnter)
        counting = true;

    if (prevCounting != counting)
    {
        counting ? GUI::plusOne(this) : GUI::minusOne(this);
    }

    return counting;
}
