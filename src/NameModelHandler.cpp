#include "NameModelHandler.h"
#include "util.h"
#include "Logger.h"

namespace NameModelHandler {
/**
 * @brief Extracts the BLE device name or model from the connected client and assigns a corresponding identifier to the fingerprint.
 *
 * Prefers the device name when present and not equal to "Apple Watch"; otherwise uses the device model if available.
 *
 * @param pClient BLE client used to read the Device Information and Generic Access characteristics.
 * @param f Fingerprint object to which the identifier will be assigned.
 * @return true if either a name or model value was found and an identifier assignment was attempted, `false` if neither value was available.
 */
bool requestData(NimBLEClient* pClient, BleFingerprint* f) {
    std::string sMdl = pClient->getValue(deviceInformationService, modelChar);
    std::string sName = pClient->getValue(genericAccessService, nameChar);
    if (!sName.empty() && sMdl.find(sName) == std::string::npos && sName != "Apple Watch") {
        if (f->setId(String("name:") + kebabify(sName).c_str(), ID_TYPE_QUERY_NAME, String(sName.c_str()))) {
            Log.printf("\u001b[38;5;104m%u Name   | %s | %-58s%.1fdBm %s\u001b[0m\r\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), sName.c_str());
        }
        return true;
    }

    if (!sMdl.empty()) {
        if (f->setId(String("apple:") + kebabify(sMdl).c_str(), ID_TYPE_QUERY_MODEL, String(sMdl.c_str()))) {
            Log.printf("\u001b[38;5;136m%u Model  | %s | %-58s%.1fdBm %s\u001b[0m\r\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), sMdl.c_str());
        }
        return true;
    }

    return false;
}
}  // namespace NameModelHandler