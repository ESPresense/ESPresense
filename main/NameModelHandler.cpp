#include "NameModelHandler.h"

#include "BleUuids.h"
#include "Logger.h"
#include "string_utils.h"
#include "util.h"

namespace NameModelHandler {
bool requestData(Ble::Client& client, BleFingerprint* f) {
    std::string sMdl = client.read(deviceInformationService, modelChar);
    std::string sName = client.read(genericAccessService, nameChar);
    if (!sName.empty() && sMdl.find(sName) == std::string::npos && sName != "Apple Watch") {
        if (f->setId(std::string("name:") + kebabify(sName), ID_TYPE_QUERY_NAME, sName))
            Log.printf("\x1b[38;5;104m%u Name   | %s | %-58s%.1fdBm %s\x1b[0m\r\n", (unsigned)xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), sName.c_str());
        return true;
    }
    if (!sMdl.empty()) {
        if (f->setId(std::string("apple:") + kebabify(sMdl), ID_TYPE_QUERY_MODEL, sMdl))
            Log.printf("\x1b[38;5;136m%u Model  | %s | %-58s%.1fdBm %s\x1b[0m\r\n", (unsigned)xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), sMdl.c_str());
        return true;
    }
    return false;
}
}  // namespace NameModelHandler
