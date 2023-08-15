#include <MiFloraHandler.h>

namespace MiFloraHandler {

std::vector<std::string> addresses;
std::string bluetoothAdresses{};
int interval{};
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

    float temperature = (val[0] + val[1] * 256) / ((float)10.0);
    uint8_t moisture = val[7];
    uint32_t brightness = val[3] + val[4] * 256;
    float conductivity = val[8] + val[9] * 256;

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

    int8_t battery = val.c_str()[0];
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
    (*doc)[F("rssi")] = f->getRssi();
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
void updateAdresses(std::string stringaddresses) {  // Updating addresses after config has been saved

    addresses.clear();
    std::stringstream ss(stringaddresses);
    std::string substr;
    while (ss.good()) {
        std::getline(ss, substr, ',');
        addresses.push_back(substr);
    }
}

bool checkValidAddress(std::string address) {  // Checking if address is in scanning whitelist

    return (std::find(addresses.begin(), addresses.end(), address) != addresses.end());
}


static char test_buffer[2048];
static DynamicJsonDocument document(1024);
bool requestData(NimBLEClient* pClient, BleFingerprint* fingerprint,QueryReport* report)  // Getting mi flora data
{
    NimBLERemoteService* floraService = pClient->getService(serviceUUID);

    if (floraService == nullptr) {
        Serial.println("Getting MiFlora service failed");
        return false;
    }

    document.clear();
    // Retriving the actual data
    if (!getFloraData(&document, floraService, fingerprint))  // Getting flora data
    {
        Serial.println("Failed reading flora data");
        return false;
    }
    serializeJson(document, test_buffer);

    // Deleting services
    pClient->deleteServices();
    // Sending buffer over mqtt
    report->UpdateBuffer(new std::string(test_buffer));
    return true;
}

void ConnectToWifi() {
    AsyncWiFiSettings.heading("MiFlora <a href='https://github.com/vrachieru/xiaomi-flower-care-api' target='_blank'>ℹ️</a> ", false);
    interval = AsyncWiFiSettings.integer("poll_interval", 0, 240, 10, "Poll interval in minutes") * 60 * 1000;
}
int getInterval() {
    return interval;
}
}  // namespace MiFloraHandler