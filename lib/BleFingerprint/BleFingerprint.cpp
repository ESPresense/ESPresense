#include "BleFingerprint.h"
#include "util.h"

BleFingerprint::~BleFingerprint()
{
    Serial.printf("%d Del   | MAC: %s, ID: %s\n", xPortGetCoreID(), SMacf(address).c_str(), id.c_str());
}

BleFingerprint::BleFingerprint(BLEAdvertisedDevice *advertisedDevice, float fcmin, float beta, float dcutoff) : oneEuro{one_euro_filter<double, unsigned long>(1, fcmin, beta, dcutoff)}
{
    firstSeenMicros = esp_timer_get_time();
    address = advertisedDevice->getAddress();
    newest = recent = oldest = rssi = advertisedDevice->getRSSI();

    String mac_address = SMacf(address);

    Serial.printf("%d New   | MAC: %s", xPortGetCoreID(), mac_address.c_str());

    if (advertisedDevice->haveName())
        name = String(advertisedDevice->getName().c_str());

    std::string strServiceData = advertisedDevice->getServiceData();
    uint8_t cServiceData[100];
    strServiceData.copy((char *)cServiceData, strServiceData.length(), 0);

    if (advertisedDevice->haveServiceUUID() && advertisedDevice->getServiceDataUUID().equals(BLEUUID(tileUUID)) == true)
    {
        id = "tile:" + mac_address;
        Serial.printf(", ID: %s", id.c_str());
        calRssi = advertisedDevice->haveTXPower() ? (-advertisedDevice->getTXPower()) - 41 : 0;
    }
    else if (advertisedDevice->haveServiceUUID() && advertisedDevice->getServiceDataUUID().equals(BLEUUID(exposureUUID)) == true)
    { // found covid exposure tracker
        id = "exp:" + String(strServiceData.length());
        Serial.printf(", ID: %s", id.c_str());
        calRssi = advertisedDevice->haveTXPower() ? (-advertisedDevice->getTXPower()) - 41 : 0;

        //char *sdHex = NimBLEUtils::buildHexData(nullptr, (uint8_t *)strServiceData.data(), strServiceData.length());
        //doc["tek"] = String(sdHex).substring(4, 20);
        //free(sdHex);
    }
    else if (advertisedDevice->haveServiceUUID() && advertisedDevice->getServiceDataUUID().equals(BLEUUID(beaconUUID)) == true)
    { // found Eddystone UUID
        Serial.print(", Eddystone");
        if (cServiceData[0] == 0x10)
        {
            BLEEddystoneURL oBeacon = BLEEddystoneURL();
            oBeacon.setData(strServiceData);
            // Serial.printf("Eddystone Frame Type (Eddystone-URL) ");
            url = String(oBeacon.getDecodedURL().c_str());
            Serial.print(" URL: ");
            Serial.print(url.c_str());
            calRssi = oBeacon.getPower();
        }
        else if (cServiceData[0] == 0x20)
        {
            BLEEddystoneTLM oBeacon = BLEEddystoneTLM();
            oBeacon.setData(strServiceData);
            Serial.printf(" TLM: ");
            Serial.printf(oBeacon.toString().c_str());
        }
    }
    else
    {
        if (advertisedDevice->haveServiceUUID())
        {
            for (int i = 0; i < advertisedDevice->getServiceUUIDCount(); i++)
            {
                std::string sid = advertisedDevice->getServiceUUID(i).toString();
                Serial.printf(", sID: %s", sid.c_str());
            }
        }
        if (advertisedDevice->haveManufacturerData() == true)
        {
            std::string strManufacturerData = advertisedDevice->getManufacturerData();

            uint8_t cManufacturerData[100];
            strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);
            char *mdHex = NimBLEUtils::buildHexData(nullptr, (uint8_t *)strManufacturerData.data(), strManufacturerData.length());

            if (strManufacturerData.length() > 2 && cManufacturerData[0] == 0x4C && cManufacturerData[1] == 0x00) // Apple
            {
                if (strManufacturerData.length() == 25 && cManufacturerData[2] == 0x02 && cManufacturerData[3] == 0x15)
                {
                    BLEBeacon oBeacon = BLEBeacon();
                    oBeacon.setData(strManufacturerData);

                    String proximityUUID = getProximityUUIDString(oBeacon);

                    int major = ENDIAN_CHANGE_U16(oBeacon.getMajor());
                    int minor = ENDIAN_CHANGE_U16(oBeacon.getMinor());

                    id = "iBeacon:" + proximityUUID + "-" + String(major) + "-" + String(minor);
                    Serial.printf(", ID: %s", id.c_str());
                    calRssi = oBeacon.getSignalPower();
                }
                else
                {
                    String fingerprint = "apple:" + String(mdHex).substring(4, 8) + ":" + String(strManufacturerData.length());
                    if (advertisedDevice->haveTXPower())
                        fingerprint = fingerprint + String(-advertisedDevice->getTXPower());

                    id = fingerprint;
                    Serial.printf(", ID: %s", id.c_str());

                    calRssi = advertisedDevice->haveTXPower() ? (-advertisedDevice->getTXPower()) - 41 : 0;
                }
            }
            else
            {
                if (strManufacturerData.length() > 2)
                {
                    String fingerprint = "md:" + String(mdHex).substring(2, 4) + String(mdHex).substring(0, 2) + ":" + String(strManufacturerData.length());
                    if (advertisedDevice->haveTXPower())
                        fingerprint = fingerprint + String(-advertisedDevice->getTXPower());
                    id = fingerprint;
                    Serial.printf(", ID: %s", id.c_str());
                }

                calRssi = advertisedDevice->haveTXPower() ? (-advertisedDevice->getTXPower()) - 41 : 0;
            }
            free(mdHex);
        }
        else
        {
            calRssi = (advertisedDevice->haveTXPower() ? (-advertisedDevice->getTXPower()) - 41 : 0);
        }
    }
    Serial.println();
}

bool BleFingerprint::filter()
{
    Reading<float> inter1, inter2;

    if (tsFilter.push(&raw, &inter1))
    {
        inter2.timestamp = inter1.timestamp;
        inter2.value = oneEuro(inter1.value);
        if (diffFilter.push(&inter2, &output))
            return true;
    }
    return false;
}

void BleFingerprint::seen(BLEAdvertisedDevice *advertisedDevice)
{
    lastSeenMicros = esp_timer_get_time();

    oldest = recent;
    recent = newest;
    newest = advertisedDevice->getRSSI();
    rssi = median_of_3(oldest, recent, newest);

    if (!calRssi) calRssi = defaultTxPower;

    float ratio = (calRssi - rssi) / 35.0f;
    raw = pow(10, ratio);

    if (filter())
    {
        hasValue = true;
        reported = false;
    }
}

void BleFingerprint::setInitial(int initalRssi, float initalDistance)
{
    newest = recent = oldest = rssi = initalRssi;
    raw = initalDistance;
    hasValue = filter() || filter();
}

bool BleFingerprint::report(JsonDocument *doc, int maxDistance)
{
    if (id == nullptr && name == nullptr)
        return false;

    if (!hasValue)
        return false;

    if (maxDistance > 0 && output.value.position > maxDistance)
        return false;

    if (reported)
        return false;

    auto now = esp_timer_get_time();

    if (abs(output.value.position - lastReported) < 0.05f && abs(now - lastReportedMicros) < 5000000)
        return false;

    lastReportedMicros = now;
    lastReported = output.value.position;
    reported = true;

    String mac = SMacf(address);
    if (output.value.position < 0.5)
    {
        if (!enroll)
        {
            Serial.printf("%d Enter | MAC: %s, ID: %-50s %lu %5.1f %5.1f %5.1f\n", xPortGetCoreID(), mac.c_str(), id.c_str(), output.timestamp, output.value.position, output.value.speed * 1e6, output.value.acceleration * 1e12);
            enroll = true;
        }
    }
    else if (enroll && output.value.position > 1.5)
    {
        Serial.printf("%d Left  | MAC: %s, ID: %-50s %lu %5.1f %5.1f %5.1f\n", xPortGetCoreID(), mac.c_str(), id.c_str(), output.timestamp, output.value.position, output.value.speed * 1e6, output.value.acceleration * 1e12);
        enroll = false;
    }

    if (id != nullptr)
        (*doc)[F("id")] = id;
    if (name != nullptr)
        (*doc)[F("name")] = name;

    (*doc)[F("rssi@1m")] = calRssi;
    (*doc)[F("rssi")] = rssi;

    (*doc)[F("mac")] = mac;
    (*doc)[F("raw")] = round(raw * 100.0f) / 100.0f;
    (*doc)[F("distance")] = round(output.value.position * 100.0f) / 100.0f;
    (*doc)[F("speed")] = round(output.value.speed * 1e7f) / 10.0f;

    return true;
}
