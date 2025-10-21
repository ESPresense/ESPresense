#include "MiFloraHandler.h"
#include "Logger.h"

namespace MiFloraHandler {

std::vector<std::string> addresses;
/**
 * @brief Read main Mi Flora sensor values and store them in the provided JSON document.
 *
 * Reads the device's primary sensor characteristic and writes the following keys into
 * the provided DynamicJsonDocument: "temperature", "moisture", "light", and "conductivity".
 * The function also removes cached characteristics from the service before returning.
 *
 * @param floraService Pointer to the connected Mi Flora BLE service to read from.
 * @param doc JSON document to populate with sensor values.
 * @return true if sensor data was successfully read and stored in `doc`, `false` otherwise.
 */
bool readSensorData(BLERemoteService* floraService, DynamicJsonDocument* doc) {
    BLERemoteCharacteristic* floraCharacteristic = nullptr;

    // get the main device data characteristic
    floraCharacteristic = floraService->getCharacteristic(uuid_sensor_data);

    if (floraCharacteristic == nullptr) {
        Log.println("-- Can't read characteristics");
        return false;
    }

    // read characteristic value
    NimBLEAttValue value;

    value = floraCharacteristic->readValue();

    if (value.size() == 0) {
        Log.println("Reading Value failed");
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

/**
 * @brief Read the device battery value and store it in the provided JSON document.
 *
 * Reads the battery byte from the Mi Flora service's battery/version characteristic
 * and writes it to the JSON document under the key "battery". The function also
 * removes characteristics from the service before returning.
 *
 * @param floraService Pointer to the connected Mi Flora BLE service to read from.
 * @param doc JSON document to populate with the battery value.
 * @return true if the battery value was successfully read and stored, `false` otherwise.
 */
bool readBatteryData(BLERemoteService* floraService, DynamicJsonDocument* doc) {
    BLERemoteCharacteristic* floraCharacteristic = nullptr;

    floraCharacteristic = floraService->getCharacteristic(uuid_version_battery);

    if (floraCharacteristic == nullptr) {
        Log.println("-- Can't read characteristics");
        return false;
    }
    NimBLEAttValue val;

    val = floraCharacteristic->readValue();

    if (val.size() == 0) {
        Log.println("Reading Value failed");
        return false;
    }

    int8_t const battery = val.c_str()[0];
    (*doc)[F("battery")] = battery;

    floraService->deleteCharacteristics();
    return true;
}

/**
 * @brief Put a Mi Flora device into its data-reading mode by writing the required command to its mode characteristic.
 *
 * @param floraService Pointer to the connected Mi Flora BLE service to operate on.
 * @return true if the mode command was written and characteristics were cleaned up, `false` if the mode characteristic was not found.
 */
bool forceFloraServiceDataMode(BLERemoteService* floraService) {  // Setting the mi flora to data reading mode
    BLERemoteCharacteristic* floraCharacteristic;

    // get device mode characteristic, needs to be changed to read data
    // Log.println("- Force device in data mode");
    floraCharacteristic = nullptr;
    floraCharacteristic = floraService->getCharacteristic(uuid_write_mode);

    if (floraCharacteristic == nullptr) {
        // Log.println("-- Failed, skipping device");
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

/**
 * @brief Orchestrates reading and populating Mi Flora device data into a JSON document.
 *
 * Populates the provided JSON document with device identifiers, attempts to read battery
 * information, forces the device into data-reading mode, and attempts to read sensor data.
 * Failures during individual read steps are logged but do not stop the overall process.
 *
 * @param doc Pointer to a DynamicJsonDocument to receive device and sensor fields.
 * @param floraService Pointer to the BLERemoteService for the Mi Flora device.
 * @param f Pointer to the BleFingerprint identifying the device (used to fill id/mac).
 * @return bool `true` when orchestration completes (note: individual read failures are reported via logs). 
 */
bool getFloraData(DynamicJsonDocument* doc, BLERemoteService* floraService, BleFingerprint* f) {
    // Force miFlora to data mode

    fillDeviceData(doc, f);

    if (!MiFloraHandler::readBatteryData(floraService, doc))
        Log.println("Failed reading battery data");

    if (MiFloraHandler::forceFloraServiceDataMode(floraService)) {
    } else {
        Log.println("Failed to force data reading mode");
    }

    if (!MiFloraHandler::readSensorData(floraService, doc))
        Log.println("Failed reading sensor data");

    return true;
}

/**
 * @brief Request Mi Flora sensor and battery data for a device and attach it as a report to the fingerprint.
 *
 * Attempts to acquire the Mi Flora BLE service from the provided client, collect device data into a JSON
 * document, serialize that document, and store the resulting payload in the fingerprint's report.
 *
 * @param pClient Connected BLE client used to access the Mi Flora service.
 * @param fingerprint Fingerprint representing the target device; its report will be set on success.
 * @return true if the service was found, data collection succeeded, and the fingerprint's report was set; `false` otherwise.
 */
bool requestData(NimBLEClient* pClient, BleFingerprint* fingerprint)  // Getting mi flora data
{
    DynamicJsonDocument document(256);

    NimBLERemoteService* floraService = pClient->getService(serviceUUID);

    if (floraService == nullptr) {
        Log.println("Getting MiFlora service failed");
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