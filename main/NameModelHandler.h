#pragma once
#include "Ble.h"
#include "BleFingerprint.h"

namespace NameModelHandler {
bool requestData(Ble::Client& client, BleFingerprint* fingerprint);
}
