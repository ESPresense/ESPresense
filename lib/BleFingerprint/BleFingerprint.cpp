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
    compositeFilter.append(&wuFilter);
    compositeFilter.append(&maFilter);
    compositeFilter.append(&tsFilter);
    compositeFilter.append(&oneEuro);
    compositeFilter.append(&diffFilter);

    String mac_address = advertisedDevice->getAddress().toString().c_str();
    mac_address.replace(":", "");
    mac_address.toLowerCase();

    Serial.print("MAC: ");
    Serial.print(mac_address);

    if (advertisedDevice->haveName())
    {
        String nameBLE = String(advertisedDevice->getName().c_str());
        Serial.print(", Name: ");
        Serial.print(nameBLE);
        doc["name"] = nameBLE;
    }

    id = mac_address;
    doc["mac"] = mac_address;

    std::string strServiceData = advertisedDevice->getServiceData();
    uint8_t cServiceData[100];
    strServiceData.copy((char *)cServiceData, strServiceData.length(), 0);

    if (advertisedDevice->haveServiceUUID())
    {
        for (int i = 0; i < advertisedDevice->getServiceUUIDCount(); i++)
        {
            std::string sid = advertisedDevice->getServiceUUID(i).toString();
            Serial.printf(", sID: %s", sid.c_str());
            doc["sid" + String(i)] = sid;
        }
    }

    if (advertisedDevice->haveServiceUUID() && advertisedDevice->getServiceDataUUID().equals(BLEUUID(tileUUID)) == true)
    {
        Serial.print(", Tile");
        doc["name"] = "Tile";
        setCalRssi(advertisedDevice->haveTXPower() ? (-advertisedDevice->getTXPower()) - 41 : 0);
    }
    else if (advertisedDevice->haveServiceUUID() && advertisedDevice->getServiceDataUUID().equals(BLEUUID(exposureUUID)) == true)
    { // found covid exposure tracker
        Serial.print(", Exposure");
        id = "exp:" + String(strServiceData.length());
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

                    Serial.print(", iBeacon: ");
                    Serial.print(proximityUUID);
                    Serial.printf("-%d-%d", major, minor);

                    doc["major"] = major;
                    doc["minor"] = minor;

                    id = proximityUUID + "-" + String(major) + "-" + String(minor);
                    setCalRssi(oBeacon.getSignalPower());
                }
                else
                {
                    String fingerprint = "apple:" + String(mdHex).substring(4, 8) + ":" + String(strManufacturerData.length());
                    if (advertisedDevice->haveTXPower())
                        fingerprint = fingerprint + String(-advertisedDevice->getTXPower());

                    id = fingerprint;

                    Serial.printf(", Fingerprint: %s", fingerprint.c_str());
                    setCalRssi(advertisedDevice->haveTXPower() ? (-advertisedDevice->getTXPower()) - 41 : 0);
                }
            }
            else
            {
                if (advertisedDevice->haveTXPower())
                    doc["txPower"] = -advertisedDevice->getTXPower();

                Serial.print(", MD: ");
                for (int x = 0; x < strManufacturerData.length(); x++)
                    Serial.print(strManufacturerData[x], HEX);

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

    if (compositeFilter.push(&distFl, &output))
    {
        raw = distFl;
        //        if (id == "2c96d71f47569faddd487c93cc9dac2e-0-0")
        //            Serial.printf("%-36s %5.1f %5.1f %5.1f %5.1f\n", id.c_str(), distFl, output.value.position, output.value.speed * 1e6, output.value.acceleration * 1e12);
    }
}

void BleFingerprint::report(BLEAdvertisedDevice *advertisedDevice)
{
    //Serial.printf("%s", id.c_str());
    //Serial.printf(", RSSI: %d (@1m %d)", rssi, calRssi);
    //Serial.printf(", Dist: %.1f (orig %.1f)", distance, original);
    //Serial.println();

    doc["id"] = id;
    doc["rssi@1m"] = calRssi;
    doc["rssi"] = rssi;
    doc["raw"] = round(raw * 100.0f) / 100.0f;
    doc["distance"] = round(output.value.position * 100.0f) / 100.0f;
}
