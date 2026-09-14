#pragma once
#include "Ble.h"
#include "BleFingerprint.h"

namespace MiFloraHandler {
bool requestData(Ble::Client& client, BleFingerprint* fingerprint);
}  // namespace MiFloraHandler
