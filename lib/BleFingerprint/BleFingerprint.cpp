#include "BleFingerprint.h"

#define DT_COVARIANCE_RK 2
#define DT_COVARIANCE_QK 0.1

static const uint16_t beaconUUID = 0xFEAA;
static const uint16_t tileUUID = 0xFEED;
static const uint16_t exposureUUID = 0xFD6F;
#ifdef TX_DEFAULT
static const int defaultTxPower = TX_DEFAULT;
#else
static const int defaultTxPower = -59;
#endif

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))

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

void BleFingerprint::setCalRssi(int rssi)
{
    calRssi = rssi;
}

StaticJsonDocument<500> BleFingerprint::getJson()
{
    return doc;
}

BleFingerprint::BleFingerprint(BLEAdvertisedDevice *advertisedDevice, float initalDistance)
{
    String mac_address = advertisedDevice->getAddress().toString().c_str();
    mac_address.replace(":", "");
    mac_address.toLowerCase();

    Serial.print("MAC: ");
    Serial.print(mac_address);

    if (advertisedDevice->haveName())
        name = String(advertisedDevice->getName().c_str());

    doc["mac"] = mac_address;

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
            doc["url"] = oBeacon.getDecodedURL().c_str();
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
                doc["sid" + String(i)] = sid;
            }
        }
        if (advertisedDevice->haveManufacturerData() == true)
        {
            std::string strManufacturerData = advertisedDevice->getManufacturerData();

            uint8_t cManufacturerData[100];
            strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);
            char *mdHex = NimBLEUtils::buildHexData(nullptr, (uint8_t *)strManufacturerData.data(), strManufacturerData.length());

            doc["md"] = mdHex;

            if (strManufacturerData.length() > 2 && cManufacturerData[0] == 0x4C && cManufacturerData[1] == 0x00) // Apple
            {
                if (strManufacturerData.length() == 25 && cManufacturerData[2] == 0x02 && cManufacturerData[3] == 0x15)
                {
                    BLEBeacon oBeacon = BLEBeacon();
                    oBeacon.setData(strManufacturerData);

                    String proximityUUID = getProximityUUIDString(oBeacon);

                    int major = ENDIAN_CHANGE_U16(oBeacon.getMajor());
                    int minor = ENDIAN_CHANGE_U16(oBeacon.getMinor());

                    doc["major"] = major;
                    doc["minor"] = minor;

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
            ;
    }

    //if (id == "2c96d71f47569faddd487c93cc9dac2e-0-0")
    //Serial.printf("%-36s %d %5.1f %5.1f %5.1f %5.1f\n", id.c_str(), inter2.timestamp, distFl, inter2.value, output.value.speed * 1e6, output.value.acceleration * 1e12);
}

void BleFingerprint::report(BLEAdvertisedDevice *advertisedDevice)
{
    //Serial.printf("%s", id.c_str());
    //Serial.printf(", RSSI: %d (@1m %d)", rssi, calRssi);
    //Serial.printf(", Dist: %.1f (orig %.1f)", distance, original);
    //Serial.println();
    if (id == nullptr && name == nullptr)
        return;

    if (id != nullptr)
        doc["id"] = id;
    if (name != nullptr)
        doc["name"] = name;
    doc["rssi@1m"] = calRssi;
    doc["rssi"] = rssi;
    doc["raw"] = round(raw * 100.0f) / 100.0f;
    doc["distance"] = round(output.value.position * 100.0f) / 100.0f;
}
