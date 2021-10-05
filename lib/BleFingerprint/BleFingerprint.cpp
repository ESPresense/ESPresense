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

    if (advertisedDevice->haveServiceUUID())
    {

#ifdef VERBOSE
        for (int i = 0; i < advertisedDevice->getServiceUUIDCount(); i++)
            Serial.printf("Verbose | %-58sSD: %s\n", getId().c_str(), advertisedDevice->getServiceUUID(i).toString().c_str());
#endif

        if (advertisedDevice->isAdvertisingService(tileUUID))
        {
            calRssi = _parent->getRefRssi() + TILE_TX;
            if (!pidOverriden) pid = "tile:" + getMac();
        }
        else if (advertisedDevice->isAdvertisingService(exposureUUID))
        { // found covid exposure tracker
            std::string strServiceData = advertisedDevice->getServiceData(exposureUUID);
            calRssi = _parent->getRefRssi() + EXPOSURE_TX;
            if (!pidOverriden) pid = "exp:" + String(strServiceData.length());
        }
        else if (advertisedDevice->isAdvertisingService(sonosUUID))
        {
            asRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
            if (!pidOverriden) pid = "sonos:" + getMac();
        }
        else if (advertisedDevice->isAdvertisingService(itagUUID))
        {
            asRssi = _parent->getRefRssi() + (advertisedDevice->haveTXPower() ? advertisedDevice->getTXPower() : ITAG_TX);
            if (!pidOverriden) pid = "itag:" + getMac();
        }
        else if (advertisedDevice->isAdvertisingService(roomAssistantService))
        {
            asRssi = advertisedDevice->haveTXPower() ? advertisedDevice->getTXPower() - 65 : NO_RSSI;
            shouldQuery = true;
        }
        else if (advertisedDevice->isAdvertisingService(eddystoneUUID))
        {
            std::string strServiceData = advertisedDevice->getServiceData(eddystoneUUID);
            if (strServiceData[0] == EDDYSTONE_URL_FRAME_TYPE && strServiceData.length() <= 18)
            {
                BLEEddystoneURL oBeacon = BLEEddystoneURL();
                oBeacon.setData(strServiceData);
                url = String(oBeacon.getDecodedURL().c_str());
                calRssi = EDDYSTONE_ADD_1M + oBeacon.getPower();
            }
            else if (strServiceData[0] == EDDYSTONE_TLM_FRAME_TYPE)
            {
                BLEEddystoneTLM oBeacon = BLEEddystoneTLM();
                oBeacon.setData(strServiceData);
                temp = oBeacon.getTemp();
                volts = oBeacon.getVolt();
#ifdef VERBOSE
                Serial.println(oBeacon.toString().c_str());
#endif
            }
        }
        else
        {
            asRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
            String fingerprint = "sid:";
            for (int i = 0; i < advertisedDevice->getServiceUUIDCount(); i++)
            {
                std::string sid = advertisedDevice->getServiceUUID(i).toString();
                fingerprint = fingerprint + String(sid.c_str());
            }
            if (advertisedDevice->haveTXPower())
                fingerprint = fingerprint + String(-advertisedDevice->getTXPower());
            sid = fingerprint;
        }
    }

    if (advertisedDevice->haveManufacturerData())
    {
        std::string strManufacturerData = advertisedDevice->getManufacturerData();
#ifdef VERBOSE
        Serial.printf("Verbose | %-58sMD: %s\n", getId().c_str(), hexStr(strManufacturerData).c_str());
#endif
        if (strManufacturerData.length() > 2)
        {
            String manuf = Sprintf("%02x%02x", strManufacturerData[1], strManufacturerData[0]);

            if (manuf == "004c") // Apple
            {
                if (strManufacturerData.length() == 25 && strManufacturerData[2] == 0x02 && strManufacturerData[3] == 0x15)
                {
                    BLEBeacon oBeacon = BLEBeacon();
                    oBeacon.setData(strManufacturerData);
                    if (!pidOverriden) pid = Sprintf("iBeacon:%s-%d-%d", std::string(oBeacon.getProximityUUID()).c_str(), ENDIAN_CHANGE_U16(oBeacon.getMajor()), ENDIAN_CHANGE_U16(oBeacon.getMinor()));
                    calRssi = oBeacon.getSignalPower();
                }
                else if (strManufacturerData.length() >= 4 && strManufacturerData[2] == 0x10)
                {
                    shouldQuery = true;
                    ignore = false;
                    if (!pidOverriden)
                    {
                        if (advertisedDevice->haveTXPower())
                            pid = Sprintf("apple:%02x%02x:%d%d", strManufacturerData[2], strManufacturerData[3], strManufacturerData.length(), -advertisedDevice->getTXPower());
                        else
                            pid = Sprintf("apple:%02x%02x:%d", strManufacturerData[2], strManufacturerData[3], strManufacturerData.length());
                    }
                    mdRssi = _parent->getRefRssi() + APPLE_TX;
                }
                else
                {
                    if (advertisedDevice->haveTXPower())
                        sid = Sprintf("apple:%02x%02x:%d%d", strManufacturerData[2], strManufacturerData[3], strManufacturerData.length(), -advertisedDevice->getTXPower());
                    else
                        sid = Sprintf("apple:%02x%02x:%d", strManufacturerData[2], strManufacturerData[3], strManufacturerData.length());
                    mdRssi = _parent->getRefRssi() + APPLE_TX;
                    ignore = true;
                }
            }
            else if (manuf == "05a7") //Sonos
            {
                mdRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
                if (!pidOverriden) pid = "sonos:" + getMac();
            }
            else if (manuf == "0157") //Mi-fit
            {
                mdRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
                pid = "mifit:" + getMac();
            }
            else if (manuf == "0006" && strManufacturerData.length() == 29) //microsoft
            {
                mdRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
                sid = Sprintf("microsoft:%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                                                 strManufacturerData[6], strManufacturerData[7], strManufacturerData[8], strManufacturerData[9], strManufacturerData[10], strManufacturerData[11],
                                                 strManufacturerData[12], strManufacturerData[13], strManufacturerData[14], strManufacturerData[15], strManufacturerData[16], strManufacturerData[17],
                                                 strManufacturerData[18], strManufacturerData[19], strManufacturerData[20], strManufacturerData[21], strManufacturerData[22], strManufacturerData[23],
                                                 strManufacturerData[24], strManufacturerData[25], strManufacturerData[26], strManufacturerData[27], strManufacturerData[28]);
                ignore = true;
            }
            else if (manuf == "0075") //samsung
            {
                mdRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
                if (!pidOverriden) pid = "samsung:" + getMac();
            }
            else
            {
                mdRssi = advertisedDevice->haveTXPower() ? _parent->getRefRssi() + advertisedDevice->getTXPower() : NO_RSSI;
                String fingerprint = Sprintf("md:%s:%d", manuf.c_str(), strManufacturerData.length());
                if (advertisedDevice->haveTXPower())
                    fingerprint = fingerprint + String(-advertisedDevice->getTXPower());
                sid = fingerprint;
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

bool BleFingerprint::report(JsonDocument *doc, float maxDistance)
{
    if (pid.isEmpty() && sid.isEmpty() && !macPublic)
        return false;

    if (reported || !hasValue)
        return false;

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

    (*doc)[F("rssi@1m")] = get1mRssi();
    (*doc)[F("rssi")] = rssi;

    (*doc)[F("mac")] = SMacf(address);
    (*doc)[F("raw")] = round(raw * 100.0f) / 100.0f;
    (*doc)[F("distance")] = round(output.value.position * 100.0f) / 100.0f;
    (*doc)[F("speed")] = round(output.value.speed * 1e7f) / 10.0f;

    if (volts) (*doc)[F("volts")] = volts;
    if (temp) (*doc)[F("temp")] = temp;

    return true;
}

bool BleFingerprint::query()
{
    if (!shouldQuery || didQuery) return false;
    auto now = millis();
    if (now - lastQryMillis < 500) return false;
    didQuery = true;
    lastQryMillis = now;
    auto pClient = NimBLEDevice::getDisconnectedClient();
    if (!pClient) pClient = NimBLEDevice::createClient();

    //pClient->setClientCallbacks(this, false);
    NimBLEDevice::getScan()->setActiveScan(false);
    pClient->setConnectTimeout(5);
    if (pClient->connect(address))
    {
        auto sName = pClient->getValue(genericAccessService, nameChar);
        if (!sName.empty())
        {
            Serial.printf("%d Name  | MAC: %s, ID: %-50s%s\n", xPortGetCoreID(), getMac().c_str(), getId().c_str(), sName.c_str());
            if (name.length() == 0) name = sName.c_str();
        }

        auto sRmAst = pClient->getValue(roomAssistantService, rootAssistantCharacteristic);
        if (!sRmAst.empty())
        {
            Serial.printf("%d RmAst | MAC: %s, ID: %-50s%s\n", xPortGetCoreID(), getMac().c_str(), getId().c_str(), sRmAst.c_str());
            if (!pidOverriden) pid = String("roomAssistant:") + kebabify(sRmAst).c_str();
            pidOverriden = true;
        }
        else
        {
            auto sMdl = pClient->getValue(deviceInformationService, modelChar);
            if (!sMdl.empty())
            {
                Serial.printf("%d Model | MAC: %s, ID: %-50s%s\n", xPortGetCoreID(), getMac().c_str(), getId().c_str(), sMdl.c_str());
                if (!pidOverriden) pid = pid + String("-") + kebabify(sMdl).c_str();
                pidOverriden = true;
            }
            else
            {
                if (name.length() > 0 && !pidOverriden) pid = pid + String("-") + kebabify(name);
                pidOverriden = true;
            }
        }

        // auto sFwRevChar = pClient->getValue(deviceInformationService, fwRevChar);
        // Serial.printf("%d FwRev | MAC: %s, ID: %-50s%s\n", xPortGetCoreID(), getMac().c_str(), getId().c_str(), sFwRevChar.c_str());

        // auto sHwRevChar = pClient->getValue(deviceInformationService, hwRevChar);
        // Serial.printf("%d HwRev | MAC: %s, ID: %-50s%s\n", xPortGetCoreID(), getMac().c_str(), getId().c_str(), sHwRevChar.c_str());

        pClient->disconnect();
    }
    else
    {
        qryAttempts++;
        if (qryAttempts < 10) didQuery = false;
    }
    return true;
}
