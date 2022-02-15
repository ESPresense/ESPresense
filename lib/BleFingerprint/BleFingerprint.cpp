#include "BleFingerprint.h"
#include "BleFingerprintCollection.h"
#include "rssi.h"
#include "strings.h"
#include "util.h"

#define Sprintf(f, ...) (             \
    {                                 \
        char *s;                      \
        asprintf(&s, f, __VA_ARGS__); \
        String r = s;                 \
        free(s);                      \
        r;                            \
    })

#define SMacf(f) (                                                                                                                                       \
    {                                                                                                                                                    \
        auto nativeAddress = f.getNative();                                                                                                              \
        Sprintf("%02x%02x%02x%02x%02x%02x", nativeAddress[5], nativeAddress[4], nativeAddress[3], nativeAddress[2], nativeAddress[1], nativeAddress[0]); \
    })

bool prefixExists(String prefixes, String id)
{
    unsigned int start = 0;
    unsigned int space = 0;

    while ((space = prefixes.indexOf(" ", start)) != -1)
    {
        if (space > start)
        {
            auto sub = prefixes.substring(start, space);
            if (sub == "*" || id.indexOf(sub) != -1) return true;
        }
        start = space + 1;
    }
    auto sub = prefixes.substring(start);
    return (sub == "*" || id.indexOf(sub) != -1);
}

bool BleFingerprint::shouldHide(String id)
{
    auto include = _parent->getInclude();
    if (include.length() > 0 && !prefixExists(include, id)) return true;

    auto exclude = _parent->getExclude();
    return (exclude.length() > 0 && prefixExists(exclude, id));
}

void BleFingerprint::setId(String newId, short int newIdType)
{
    if (newIdType < idType) return;

    hidden = shouldHide(newId);

    if (!allowQuery)
    {
        auto query = _parent->getQuery();
        if (query.length() > 0 && prefixExists(query, newId))
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

    id = newId;
    idType = newIdType;
}

String BleFingerprint::getMac() { return SMacf(address); }

int BleFingerprint::get1mRssi()
{
    if (calRssi != NO_RSSI) return calRssi;
    if (mdRssi != NO_RSSI) return mdRssi;
    if (asRssi != NO_RSSI) return asRssi;
    return _parent->getRefRssi() + DEFAULT_TX;
}

BleFingerprint::BleFingerprint(BleFingerprintCollection *parent, BLEAdvertisedDevice *advertisedDevice, float fcmin, float beta, float dcutoff) : oneEuro{OneEuroFilter<float, long long>(1, fcmin, beta, dcutoff)}
{
    _parent = parent;

    firstSeenMillis = millis();
    address = advertisedDevice->getAddress();
    macPublic = advertisedDevice->getAddressType() == BLE_ADDR_PUBLIC;
    newest = recent = oldest = rssi = advertisedDevice->getRSSI();
}

void BleFingerprint::fingerprint(BLEAdvertisedDevice *advertisedDevice)
{
    if (advertisedDevice->haveName())
        name = String(advertisedDevice->getName().c_str());

    if (advertisedDevice->getAdvType() > 0)
        connectable = true;

    if (advertisedDevice->haveServiceUUID())
    {

#ifdef VERBOSE
        for (int i = 0; i < advertisedDevice->getServiceUUIDCount(); i++)
            Serial.printf("Verbose | %-58sAD: %s\n", getId().c_str(), advertisedDevice->getServiceUUID(i).toString().c_str());
#endif

        if (advertisedDevice->isAdvertisingService(tileUUID))
        {
            calRssi = _parent->getRefRssi() + TILE_TX;
            setId("tile:" + getMac(), ID_TYPE_TILE);
        }
        else if (advertisedDevice->isAdvertisingService(sonosUUID))
        {
            asRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
            setId("sonos:" + getMac(), ID_TYPE_SONOS);
        }
        else if (advertisedDevice->isAdvertisingService(itagUUID))
        {
            asRssi = _parent->getRefRssi() + (advertisedDevice->haveTXPower() ? advertisedDevice->getTXPower() : ITAG_TX);
            setId("itag:" + getMac(), ID_TYPE_ITAG);
        }
        else if (advertisedDevice->isAdvertisingService(trackrUUID))
        {
            asRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
            setId("trackr:" + getMac(), ID_TYPE_TRACKR);
        }
        else if (advertisedDevice->isAdvertisingService(meaterService))
        {
            asRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
            setId("meater:" + getMac(), ID_TYPE_MEATER);
        }
        else if (advertisedDevice->isAdvertisingService(roomAssistantService))
        {
            asRssi = advertisedDevice->haveTXPower() ? advertisedDevice->getTXPower() - 65 : NO_RSSI;
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
        }
        else
        {
            asRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
            String fingerprint = "ad:";
            for (int i = 0; i < advertisedDevice->getServiceUUIDCount(); i++)
            {
                std::string sid = advertisedDevice->getServiceUUID(i).toString();
                fingerprint = fingerprint + String(sid.c_str());
            }
            if (advertisedDevice->haveTXPower()) fingerprint = fingerprint + String(-advertisedDevice->getTXPower());
            setId(fingerprint, ID_TYPE_AD);
        }
    }

    if (advertisedDevice->haveServiceData())
    {
        asRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
        String fingerprint = "sd:";
        for (int i = 0; i < advertisedDevice->getServiceDataCount(); i++)
        {
            BLEUUID uuid = advertisedDevice->getServiceDataUUID(i);
            std::string strServiceData = advertisedDevice->getServiceData(i);
#ifdef VERBOSE
            Serial.printf("Verbose | %-58sSD: %s/%s\n", getId().c_str(), uuid.toString().c_str(), hexStr(strServiceData).c_str());
#endif

            if (uuid == exposureUUID)
            { // found covid exposure tracker
                std::string strServiceData = advertisedDevice->getServiceData(exposureUUID);
                calRssi = _parent->getRefRssi() + EXPOSURE_TX;
                setId("exp:" + String(strServiceData.length()), ID_TYPE_EXPOSURE);
                disc = hexStr(strServiceData).c_str();
            }
            else if (uuid == miThermUUID)
            {
                asRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
                if (strServiceData.length() == 15)
                { // custom format
                    auto serviceData = strServiceData.c_str();
                    temp = *(int16_t *)(serviceData + 6) / 100.0f;
                    humidity = *(uint16_t *)(serviceData + 8) / 100.0f;
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
                    temp = x / 10.0f;
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
        if (advertisedDevice->haveTXPower())
            fingerprint = fingerprint + String(-advertisedDevice->getTXPower());
        setId(fingerprint, ID_TYPE_SD);
    }

    if (advertisedDevice->haveManufacturerData())
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
                    ignore = false;
                    {
                        String pid;
                        if (advertisedDevice->haveTXPower())
                            pid = Sprintf("apple:%02x%02x:%d%d", strManufacturerData[2], strManufacturerData[3], strManufacturerData.length(), -advertisedDevice->getTXPower());
                        else
                            pid = Sprintf("apple:%02x%02x:%d", strManufacturerData[2], strManufacturerData[3], strManufacturerData.length());
                        setId(pid, ID_TYPE_APPLE_NEARBY);
                    }
                    disc = hexStr(strManufacturerData.substr(4)).c_str();
                    mdRssi = _parent->getRefRssi() + APPLE_TX;
                }
                else
                {
                    if (advertisedDevice->haveTXPower())
                        setId(Sprintf("apple:%02x%02x:%d%d", strManufacturerData[2], strManufacturerData[3], strManufacturerData.length(), -advertisedDevice->getTXPower()), ID_TYPE_MISC_APPLE + ID_TYPE_TX_POW);
                    else
                        setId(Sprintf("apple:%02x%02x:%d", strManufacturerData[2], strManufacturerData[3], strManufacturerData.length()), ID_TYPE_MISC_APPLE);
                    mdRssi = _parent->getRefRssi() + APPLE_TX;
                    ignore = true;
                }
            }
            else if (manuf == "05a7") //Sonos
            {
                mdRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
                setId("sonos:" + getMac(), ID_TYPE_SONOS);
            }
            else if (manuf == "0157") //Mi-fit
            {
                mdRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
                setId("mifit:" + getMac(), ID_TYPE_MIFIT);
            }
            else if (manuf == "0006" && strManufacturerData.length() == 29) //microsoft
            {
                mdRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
                setId(Sprintf("msft:cdp:%02x%02x", strManufacturerData[3], strManufacturerData[5]), ID_TYPE_MSFT);
                disc = Sprintf("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                               strManufacturerData[6], strManufacturerData[7], strManufacturerData[8], strManufacturerData[9], strManufacturerData[10],
                               strManufacturerData[11], strManufacturerData[12], strManufacturerData[13], strManufacturerData[14], strManufacturerData[15],
                               strManufacturerData[16], strManufacturerData[17], strManufacturerData[18], strManufacturerData[19], strManufacturerData[20],
                               strManufacturerData[21], strManufacturerData[22], strManufacturerData[23], strManufacturerData[24], strManufacturerData[25]);
            }
            else if (manuf == "0075") //samsung
            {
                mdRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
                setId("samsung:" + getMac(), ID_TYPE_MISC);
            }
            else if (strManufacturerData.length() == 26 && strManufacturerData[2] == 0xBE && strManufacturerData[3] == 0xAC)
            {
                BLEBeacon oBeacon = BLEBeacon();
                oBeacon.setData(strManufacturerData.substr(0, 25));
                setId(Sprintf("altBeacon:%s-%d-%d", std::string(oBeacon.getProximityUUID()).c_str(), ENDIAN_CHANGE_U16(oBeacon.getMajor()), ENDIAN_CHANGE_U16(oBeacon.getMinor())), ID_TYPE_ABEACON);
                calRssi = oBeacon.getSignalPower();
            }
            else if (manuf != "0000")
            {
                mdRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
                String fingerprint = Sprintf("md:%s:%d", manuf.c_str(), strManufacturerData.length());
                if (advertisedDevice->haveTXPower())
                    fingerprint = fingerprint + String(-advertisedDevice->getTXPower());
                setId(fingerprint, ID_TYPE_MD);
            }
        }
    }
}

bool BleFingerprint::filter()
{
    Reading<float, long long> inter1, inter2;
    inter1.timestamp = esp_timer_get_time();
    inter1.value = raw;

    return oneEuro.push(&inter1, &inter2) && diffFilter.push(&inter2, &output);
}

bool BleFingerprint::seen(BLEAdvertisedDevice *advertisedDevice)
{
    lastSeenMillis = millis();
    seenCount++;

    if (ignore) return false;

    oldest = recent;
    recent = newest;
    newest = advertisedDevice->getRSSI();
    rssi = median_of_3(oldest, recent, newest);

    fingerprint(advertisedDevice);

    if (ignore) return false;

    float ratio = (get1mRssi() - rssi) / 35.0f;
    raw = pow(10, ratio);

    bool hadValue = hasValue;

    if (filter())
    {
        hasValue = true;
        reported = false;

        if (!close && newest > CLOSE_RSSI)
        {
            Display.close(this);
            close = true;
        }
        else if (close && newest < LEFT_RSSI)
        {
            Display.left(this);
            close = false;
        }

        if (!hadValue) return true;
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
    if (ignore || (idType == 0 && !macPublic) || hidden)
        return false;

    if (reported || !hasValue)
        return false;

    auto maxDistance = _parent->getMaxDistance();
    if (maxDistance > 0 && output.value.position > maxDistance)
        return false;

    auto now = millis();
    if ((abs(output.value.position - lastReported) < _parent->getSkipDistance()) && (lastReportedMillis > 0) && (now - lastReportedMillis < _parent->getSkipMs()))
        return false;

    //Serial.printf("%f > %f || %d > %d\n", abs(output.value.position - lastReported), _parent->getSkipDistance(), now - lastReportedMillis, _parent->getSkipMs());

    lastReportedMillis = now;
    lastReported = output.value.position;
    reported = true;

    (*doc)[F("id")] = getId();
    if (!name.isEmpty()) (*doc)[F("name")] = name;
    if (!disc.isEmpty()) (*doc)[F("disc")] = disc;
    if (idType) (*doc)[F("idType")] = idType;

    (*doc)[F("rssi@1m")] = get1mRssi();
    (*doc)[F("rssi")] = rssi;

    (*doc)[F("mac")] = SMacf(address);
    (*doc)[F("raw")] = round(raw * 100.0f) / 100.0f;
    (*doc)[F("distance")] = round(output.value.position * 100.0f) / 100.0f;
    (*doc)[F("speed")] = round(output.value.speed * 1e7f) / 10.0f;

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

    if (now - lastSeenMillis > 5) return false;

    if (now - lastQryMillis < qryDelayMillis) return false;
    didQuery = true;
    lastQryMillis = now;

    bool success = false;

    Serial.printf("%d Query | MAC: %s, ID: %-60s rssi %d\n", xPortGetCoreID(), getMac().c_str(), getId().c_str(), rssi);

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
            if (!sName.empty() && !sMdl.empty() && sMdl.find(sName) == std::string::npos && sName.compare("Apple Watch") != 0)
            {
                Serial.printf("%d Name  | MAC: %s, ID: %-60s %s\n", xPortGetCoreID(), getMac().c_str(), getId().c_str(), sName.c_str());
                setId(String("name:") + kebabify(sName).c_str(), ID_TYPE_APPLE_NAME);
                success = true;
            }
            else if (!sMdl.empty())
            {
                if (name.isEmpty()) name = sMdl.c_str();
                Serial.printf("%d Model | MAC: %s, ID: %-60s %s\n", xPortGetCoreID(), getMac().c_str(), getId().c_str(), sMdl.c_str());
                setId(String("apple:") + kebabify(sMdl).c_str(), ID_TYPE_APPLE_MODEL);
                success = true;
            }
            else if (!sName.empty())
            {
                if (name.isEmpty()) name = sName.c_str();
            }
        }

        if (rmAsst || iphone) // For some reason we often don't get room asssistants service advertisement
        {
            std::string sRmAst = pClient->getValue(roomAssistantService, rootAssistantCharacteristic);
            if (!sRmAst.empty())
            {
                setId(String("roomAssistant:") + kebabify(sRmAst).c_str(), ID_TYPE_RM_ASST);
                Serial.printf("%d RmAst | MAC: %s, ID: %-60s %s\n", xPortGetCoreID(), getMac().c_str(), getId().c_str(), sRmAst.c_str());
                success = true;
            }
        }
    }

    NimBLEDevice::deleteClient(pClient);

    if (success) return true;

    qryAttempts++;
    Serial.printf("%d QryErr| MAC: %s, ID: %-60s rssi %d, try %d, retry after %dms\n", xPortGetCoreID(), getMac().c_str(), getId().c_str(), rssi, qryAttempts, qryDelayMillis);

    if (qryDelayMillis < 30000)
        qryDelayMillis += (1000 * qryAttempts * qryAttempts);
    else
        qryDelayMillis = 30000;
    didQuery = false;

    return true;
}
