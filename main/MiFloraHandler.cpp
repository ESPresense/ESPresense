#include "MiFloraHandler.h"

#include <ArduinoJson.h>

#include "Logger.h"
#include "util.h"

namespace MiFloraHandler {

static const ble_uuid_any_t serviceUUID = Ble::uuid128(0x00001204, 0x0000, 0x1000, 0x800000805f9b34fb);
static const ble_uuid_any_t uuid_version_battery = Ble::uuid128(0x00001a02, 0x0000, 0x1000, 0x800000805f9b34fb);
static const ble_uuid_any_t uuid_sensor_data = Ble::uuid128(0x00001a01, 0x0000, 0x1000, 0x800000805f9b34fb);
static const ble_uuid_any_t uuid_write_mode = Ble::uuid128(0x00001a00, 0x0000, 0x1000, 0x800000805f9b34fb);

bool requestData(Ble::Client& client, BleFingerprint* f) {
    DynamicJsonDocument document(256);
    document["id"] = f->getId();
    document["mac"] = f->getMac();

    std::string bat = client.read(serviceUUID, uuid_version_battery);
    if (bat.empty())
        Log.println("Failed reading battery data");
    else
        document["battery"] = (int8_t)bat[0];

    // Force the Mi Flora into data-reading mode.
    uint8_t mode[2] = {0xA0, 0x1F};
    if (!client.write(serviceUUID, uuid_write_mode, mode, 2))
        Log.println("Failed to force data reading mode");
    delay(500);

    std::string val = client.read(serviceUUID, uuid_sensor_data);
    if (val.size() < 10) {
        Log.println("Failed reading sensor data");
    } else {
        const uint8_t* v = (const uint8_t*)val.data();
        document["temperature"] = (v[0] + v[1] * 256) / 10.0f;
        document["moisture"] = v[7];
        document["light"] = (uint32_t)(v[3] + v[4] * 256);
        document["conductivity"] = (float)(v[8] + v[9] * 256);
    }

    std::string buf;
    serializeJson(document, buf);
    f->setReport(QueryReport{"miflora", buf});
    return true;
}

}  // namespace MiFloraHandler
