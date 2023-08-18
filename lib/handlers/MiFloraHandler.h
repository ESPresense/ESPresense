#pragma once
#include<NimBLEClient.h>
#include <NimBLEDevice.h>
#include <ArduinoJson.h>
#include <sstream>
#include <SoftFilters.h>
#include <AsyncMqttClient.h>
#include <BleFingerprint.h>
#include <AsyncWiFiSettings.h>
namespace MiFloraHandler{

    static BLEUUID serviceUUID("00001204-0000-1000-8000-00805f9b34fb");
    static BLEUUID uuid_version_battery("00001a02-0000-1000-8000-00805f9b34fb");
    static BLEUUID uuid_sensor_data("00001a01-0000-1000-8000-00805f9b34fb");
    static BLEUUID uuid_write_mode("00001a00-0000-1000-8000-00805f9b34fb");

    bool requestData(NimBLEClient* pClient, BleFingerprint* fingerprint);
}
