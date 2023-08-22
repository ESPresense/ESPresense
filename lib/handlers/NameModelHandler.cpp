#include "NameModelHandler.h"
#include "util.h"

namespace NameModelHandler {
bool requestData(NimBLEClient* pClient, BleFingerprint* f) {
    std::string sMdl = pClient->getValue(deviceInformationService, modelChar);
    std::string sName = pClient->getValue(genericAccessService, nameChar);
    if (!sName.empty() && sMdl.find(sName) == std::string::npos && sName != "Apple Watch") {
        if (f->setId(String("name:") + kebabify(sName).c_str(), ID_TYPE_QUERY_NAME, String(sName.c_str()))) {
            Serial.printf("\u001b[38;5;104m%u Name   | %s | %-58s%ddBm %s\u001b[0m\r\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), sName.c_str());
        }
        return true;
    }

    if (!sMdl.empty()) {
        if (f->setId(String("apple:") + kebabify(sMdl).c_str(), ID_TYPE_QUERY_MODEL, String(sMdl.c_str()))) {
            Serial.printf("\u001b[38;5;136m%u Model  | %s | %-58s%ddBm %s\u001b[0m\r\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), sMdl.c_str());
        }
        return true;
    }

    return false;
}
}  // namespace NameModelHandler
