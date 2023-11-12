#pragma once
#include<NimBLEClient.h>
#include <NimBLEDevice.h>
#include <ArduinoJson.h>
#include <sstream>
#include <AsyncMqttClient.h>
#include <BleFingerprint.h>
#include <AsyncWiFiSettings.h>

namespace NameModelHandler {

bool requestData(NimBLEClient* pClient, BleFingerprint* fingerprint);
}
