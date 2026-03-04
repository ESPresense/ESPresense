#pragma once
#include <NimBLEClient.h>
#include <NimBLEDevice.h>
#include <ArduinoJson.h>
#include <PsychicMqttClient.h>
#include "BleFingerprint.h"
#include <HeadlessWiFiSettings.h>

namespace NameModelHandler {

bool requestData(NimBLEClient* pClient, BleFingerprint* fingerprint);
}
