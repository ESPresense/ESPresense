#include "BleFingerprint.h"

static const uint16_t beaconUUID = 0xFEAA;
static const uint16_t tileUUID = 0xFEED;
static const uint16_t exposureUUID = 0xFD6F;

#ifdef TX_DEFAULT
static const int defaultTxPower = TX_DEFAULT;
#else
static const int defaultTxPower = -59;
#endif

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))
#define Sprintf(f, ...) ({ char* s; asprintf(&s, f, __VA_ARGS__); String r = s; free(s); r; })
#define SDateTimef(f) ({ struct tm firstSeenTm; gmtime_r(&f, &firstSeenTm); Sprintf("%d/%d/%d %d:%.2d:%.2d", firstSeenTm.tm_mon, firstSeenTm.tm_mday, 1900 + firstSeenTm.tm_year, firstSeenTm.tm_hour, firstSeenTm.tm_min, firstSeenTm.tm_sec); })
#define SMacf(f) ({ auto nativeAddress = f.getNative(); Sprintf("%02x%02x%02x%02x%02x%02x", nativeAddress[5], nativeAddress[4], nativeAddress[3], nativeAddress[2], nativeAddress[1], nativeAddress[0]); })

static String getProximityUUIDString(BLEBeacon beacon)
{
    std::string serviceData = beacon.getProximityUUID().toString().c_str();
    int serviceDataLength = serviceData.length();
    String returnedString = "";
    int i = serviceDataLength;
    while (i > 0)
    {
        if (serviceData[i - 1] == '-')
        {
            i--;
        }
        char a = serviceData[i - 1];
        char b = serviceData[i - 2];
        returnedString += b;
        returnedString += a;

        i -= 2;
    }

    return returnedString;
}

BleFingerprint::BleFingerprint(BLEAdvertisedDevice *advertisedDevice, float initalDistance)
{
    firstSeen = time(nullptr);
    address = advertisedDevice->getAddress();

    auto nativeAddress = address.getNative();
    String mac_address = Sprintf("%02x%02x%02x%02x%02x%02x", nativeAddress[5], nativeAddress[4], nativeAddress[3], nativeAddress[2], nativeAddress[1], nativeAddress[0]);

    Serial.print("MAC: ");
    Serial.print(mac_address);

    if (advertisedDevice->haveName())
        name = String(advertisedDevice->getName().c_str());

    std::string strServiceData = advertisedDevice->getServiceData();
    uint8_t cServiceData[100];
    strServiceData.copy((char *)cServiceData, strServiceData.length(), 0);

    if (advertisedDevice->haveServiceUUID() && advertisedDevice->getServiceDataUUID().equals(BLEUUID(tileUUID)) == true)
    {
        id = "tile:" + mac_address;
        Serial.printf(", ID: %s", id.c_str());
        setCalRssi(advertisedDevice->haveTXPower() ? (-advertisedDevice->getTXPower()) - 41 : 0);
    }
    else if (advertisedDevice->haveServiceUUID() && advertisedDevice->getServiceDataUUID().equals(BLEUUID(exposureUUID)) == true)
    { // found covid exposure tracker
        id = "exp:" + String(strServiceData.length());
        Serial.printf(", ID: %s", id.c_str());
        setCalRssi(advertisedDevice->haveTXPower() ? (-advertisedDevice->getTXPower()) - 41 : 0);

        //char *sdHex = NimBLEUtils::buildHexData(nullptr, (uint8_t *)strServiceData.data(), strServiceData.length());
        //doc["tek"] = String(sdHex).substring(4, 20);
        //free(sdHex);
    }
    else if (advertisedDevice->haveServiceUUID() && advertisedDevice->getServiceDataUUID().equals(BLEUUID(beaconUUID)) == true)
    { // found Eddystone UUID
        Serial.print(", Eddystone");
        // Update distance v ariable for Eddystone BLE devices
        if (cServiceData[0] == 0x10)
        {
            BLEEddystoneURL oBeacon = BLEEddystoneURL();
            oBeacon.setData(strServiceData);
            // Serial.printf("Eddystone Frame Type (Eddystone-URL) ");
            // Serial.printf(oBeacon.getDecodedURL().c_str());
            Serial.print(" URL: ");
            Serial.print(oBeacon.getDecodedURL().c_str());
            setCalRssi(oBeacon.getPower());
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
                    setCalRssi(oBeacon.getSignalPower());
                }
                else
                {
                    String fingerprint = "apple:" + String(mdHex).substring(4, 8) + ":" + String(strManufacturerData.length());
                    if (advertisedDevice->haveTXPower())
                        fingerprint = fingerprint + String(-advertisedDevice->getTXPower());

                    id = fingerprint;
                    Serial.printf(", ID: %s", id.c_str());

                    setCalRssi(advertisedDevice->haveTXPower() ? (-advertisedDevice->getTXPower()) - 41 : 0);
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

                setCalRssi(advertisedDevice->haveTXPower() ? (-advertisedDevice->getTXPower()) - 41 : 0);
            }
            free(mdHex);
        }
        else
        {
            setCalRssi(advertisedDevice->haveTXPower() ? (-advertisedDevice->getTXPower()) - 41 : 0);
        }
    }
    Serial.println();
}

void BleFingerprint::seen(BLEAdvertisedDevice *advertisedDevice)
{
    lastSeen = time(nullptr);

    rssi = advertisedDevice->getRSSI();
    if (!calRssi)
        calRssi = defaultTxPower;

    float ratio = (calRssi - rssi) / 35.0f;
    float distFl = pow(10, ratio);
    raw = distFl;

    Reading<float> inter1, inter2;

    if (tsFilter.push(&distFl, &inter1))
    {
        inter2.timestamp = inter1.timestamp;
        inter2.value = oneEuro(inter1.value);
        if (diffFilter.push(&inter2, &output))
            hasValue = true;
    }
}

bool BleFingerprint::shouldReport()
{
    if (id == nullptr && name == nullptr)
        return false;

    if (!hasValue)
        return false;

    return true;
}

StaticJsonDocument<512> BleFingerprint::report()
{
    StaticJsonDocument<512> doc;
#if VERBOSE
    //   if (id == "iBeacon:2c96d71f47569faddd487c93cc9dac2e-0-0")
    //     Serial.printf("%-36s %lu %5.1f %5.1f %5.1f\n", id.c_str(), output.timestamp, output.value.position, output.value.speed * 1e6, output.value.acceleration * 1e12);
#endif

    if (id != nullptr)
        doc[F("id")] = id;
    if (name != nullptr)
        doc[F("name")] = name;

    doc[F("rssi@1m")] = calRssi;
    doc[F("rssi")] = rssi;

    doc[F("mac")] = SMacf(address);
    doc[F("raw")] = round(raw * 100.0f) / 100.0f;
    doc[F("distance")] = round(output.value.position * 100.0f) / 100.0f;
    doc[F("speed")] = round(output.value.speed * 1e7f) / 10.0f;

    doc[F("first")] = SDateTimef(firstSeen);
    doc[F("last")] = SDateTimef(lastSeen);
    return doc;
}
