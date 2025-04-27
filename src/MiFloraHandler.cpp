#include "MiFloraHandler.h"

namespace MiFloraHandler {

std::vector<std::string> addresses;
bool readSensorData(BLERemoteService* floraService, DynamicJsonDocument* doc) {
    BLERemoteCharacteristic* floraCharacteristic = nullptr;

    // get the main device data characteristic
    floraCharacteristic = floraService->getCharacteristic(uuid_sensor_data);

    if (floraCharacteristic == nullptr) {
        Serial.println("-- Can't read characteristics");
        return false;
    }

    // read characteristic value
    NimBLEAttValue value;

    value = floraCharacteristic->readValue();

    if (value.size() == 0) {
        Serial.println("Reading Value failed");
        return false;
    }

    const char* val = value.c_str();

    float const temperature = (val[0] + val[1] * 256) / ((float)10.0);
    uint8_t const moisture = val[7];
    uint32_t const brightness = val[3] + val[4] * 256;
    float const conductivity = val[8] + val[9] * 256;

    (*doc)[F("temperature")] = temperature;
    (*doc)[F("moisture")] = moisture;
    (*doc)[F("light")] = brightness;
    (*doc)[F("conductivity")] = conductivity;

    floraService->deleteCharacteristics();

    return true;
}

bool readBatteryData(BLERemoteService* floraService, DynamicJsonDocument* doc) {
    BLERemoteCharacteristic* floraCharacteristic = nullptr;

    floraCharacteristic = floraService->getCharacteristic(uuid_version_battery);

    if (floraCharacteristic == nullptr) {
        Serial.println("-- Can't read characteristics");
        return false;
    }
    NimBLEAttValue val;

    val = floraCharacteristic->readValue();

    if (val.size() == 0) {
        Serial.println("Reading Value failed");
        return false;
    }

    int8_t const battery = val.c_str()[0];
    (*doc)[F("battery")] = battery;

    floraService->deleteCharacteristics();
    return true;
}

bool forceFloraServiceDataMode(BLERemoteService* floraService) {  // Setting the mi flora to data reading mode
    BLERemoteCharacteristic* floraCharacteristic;

    // get device mode characteristic, needs to be changed to read data
    // Serial.println("- Force device in data mode");
    floraCharacteristic = nullptr;
    floraCharacteristic = floraService->getCharacteristic(uuid_write_mode);

    if (floraCharacteristic == nullptr) {
        // Serial.println("-- Failed, skipping device");
        return false;
    }

    uint8_t buf[2] = {0xA0, 0x1F};
    floraCharacteristic->writeValue(buf, 2, true);

    delay(500);
    floraService->deleteCharacteristics();

    return true;
}

void fillDeviceData(DynamicJsonDocument* doc, BleFingerprint* f) {
    (*doc)[F("id")] = f->getId();
    (*doc)[F("mac")] = f->getMac();
}

bool getFloraData(DynamicJsonDocument* doc, BLERemoteService* floraService, BleFingerprint* f) {
    // Force miFlora to data mode

    fillDeviceData(doc, f);

    if (!MiFloraHandler::readBatteryData(floraService, doc))
        Serial.println("Failed reading battery data");

    if (MiFloraHandler::forceFloraServiceDataMode(floraService)) {
    } else {
        Serial.println("Failed to force data reading mode");
    }

    if (!MiFloraHandler::readSensorData(floraService, doc))
        Serial.println("Failed reading sensor data");

    return true;
}

bool requestData(NimBLEClient* pClient, BleFingerprint* fingerprint)  // Getting mi flora data
{
    DynamicJsonDocument document(256);

    NimBLERemoteService* floraService = pClient->getService(serviceUUID);

    if (floraService == nullptr) {
        Serial.println("Getting MiFlora service failed");
        return false;
    }

    // Retriving the actual data
    if (!getFloraData(&document, floraService, fingerprint))  // Getting flora data
        return false;
    String buf = String();
    serializeJson(document, buf);
    // Sending buffer over mqtt
    fingerprint->setReport(QueryReport{"miflora", buf});
    return true;
}

}  // namespace MiFloraHandler
