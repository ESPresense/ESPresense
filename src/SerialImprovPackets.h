#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string>

namespace SerialImprov {
namespace Packets {

struct PacketBuffer {
    uint8_t data[128];
    size_t size;
};

PacketBuffer BuildStateResponse(uint8_t state, bool error);

PacketBuffer BuildRPCResponse(uint8_t commandId, const char* url, bool includeUrl);

PacketBuffer BuildInfoResponse(const char* firmware, const char* version, const char* hardware, const char* room);

bool DecodeWifiCredentials(const uint8_t* data, size_t length, std::string& ssid, std::string& password);

}  // namespace Packets
}  // namespace SerialImprov

