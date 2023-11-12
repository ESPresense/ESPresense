#pragma once
#include <ArduinoJson.h>
#include <AsyncMqttClient.h>
#include <AsyncWiFiSettings.h>
#include <BleFingerprint.h>
#include <NimBLEClient.h>
#include <NimBLEDevice.h>

namespace MiFloraHandler
{
    static BLEUUID serviceUUID(0x00001204, 0x0000, 0x1000, 0x800000805f9b34fb);
    static BLEUUID uuid_version_battery(0x00001a02, 0x0000, 0x1000, 0x800000805f9b34fb);
    static BLEUUID uuid_sensor_data(0x00001a01, 0x0000, 0x1000, 0x800000805f9b34fb);
    static BLEUUID uuid_write_mode(0x00001a00, 0x0000, 0x1000, 0x800000805f9b34fb);

    bool requestData(NimBLEClient* pClient, BleFingerprint* fingerprint);
}  // namespace MiFloraHandler
