#include "SerialImprovPackets.h"

#include <cstdio>
#include <cstring>

namespace SerialImprov {
namespace Packets {

namespace {
constexpr uint8_t kHeader[6] = {'I', 'M', 'P', 'R', 'O', 'V'};
constexpr uint8_t kImprovVersion = 1;

size_t WriteHeader(uint8_t* out) {
    std::memcpy(out, kHeader, sizeof(kHeader));
    out[6] = kImprovVersion;
    return 7;  // number of bytes written so far
}

void AppendChecksum(uint8_t* data, size_t length) {
    uint8_t checksum = 0;
    for (size_t i = 0; i < length - 1; i++) {
        checksum += data[i];
    }
    data[length - 1] = checksum;
}
}  // namespace

PacketBuffer BuildStateResponse(uint8_t state, bool error) {
    PacketBuffer buffer = {};
    uint8_t* out = buffer.data;

    size_t index = WriteHeader(out);
    out[index++] = error ? 0x02 : 0x01;  // Packet type (Error_State / Current_State)
    out[index++] = 1;                    // Length
    out[index++] = state;
    buffer.size = index + 1;  // include checksum

    AppendChecksum(out, buffer.size);
    return buffer;
}

PacketBuffer BuildRPCResponse(uint8_t commandId, const char* url, bool includeUrl) {
    PacketBuffer buffer = {};
    uint8_t* out = buffer.data;

    size_t index = WriteHeader(out);
    out[index++] = 0x04;  // RPC_Response

    const char* urlToWrite = includeUrl ? url : nullptr;
    size_t urlLength = (urlToWrite != nullptr) ? std::strlen(urlToWrite) : 0;
    const bool hasUrl = (urlToWrite != nullptr) && (urlLength > 0);

    const uint8_t dataLength = hasUrl ? static_cast<uint8_t>(1 + urlLength) : 0;
    const uint8_t payloadLength = hasUrl ? static_cast<uint8_t>(3 + urlLength) : 2;

    out[index++] = payloadLength;
    out[index++] = commandId;
    out[index++] = dataLength;
    if (hasUrl) {
        out[index++] = static_cast<uint8_t>(urlLength);
        std::memcpy(out + index, urlToWrite, urlLength);
        index += urlLength;
    }

    buffer.size = index + 1;  // include checksum
    AppendChecksum(out, buffer.size);
    return buffer;
}

PacketBuffer BuildInfoResponse(const char* firmware, const char* version, const char* hardware, const char* room) {
    PacketBuffer buffer = {};
    uint8_t* out = buffer.data;

    size_t index = WriteHeader(out);
    out[index++] = 0x04;  // RPC_Response

    out[index++] = 0;  // placeholder for payload length
    out[index++] = 0x03;  // Request_Info command id
    out[index++] = 0;  // Data len placeholder

    const char* firmwareName = firmware ? firmware : "";
    const char* versionString = version ? version : "";
    const char* hardwareName = hardware ? hardware : "";
    const char* roomName = room ? room : "";

    const uint8_t firmwareLen = static_cast<uint8_t>(std::strlen(firmwareName));
    out[index++] = firmwareLen;
    std::memcpy(out + index, firmwareName, firmwareLen);
    index += firmwareLen;

    const uint8_t versionLen = static_cast<uint8_t>(std::strlen(versionString));
    out[index++] = versionLen;
    std::memcpy(out + index, versionString, versionLen);
    index += versionLen;

    const uint8_t hardwareLen = static_cast<uint8_t>(std::strlen(hardwareName));
    out[index++] = hardwareLen;
    std::memcpy(out + index, hardwareName, hardwareLen);
    index += hardwareLen;

    const uint8_t roomLen = static_cast<uint8_t>(std::strlen(roomName));
    out[index++] = roomLen;
    std::memcpy(out + index, roomName, roomLen);
    index += roomLen;

    const uint8_t payloadLength = static_cast<uint8_t>(index - 9);
    const uint8_t dataLength = static_cast<uint8_t>(index - 11);
    out[8] = payloadLength;
    out[10] = dataLength;

    buffer.size = index + 1;
    AppendChecksum(out, buffer.size);
    return buffer;
}

bool DecodeWifiCredentials(const uint8_t* data, size_t length, std::string& ssid, std::string& password) {
    if (!data || length < 2 || length > 126) return false;

    const uint8_t ssidLength = data[1];
    const size_t ssidStart = 2;
    const size_t ssidEnd = ssidStart + ssidLength;
    if (ssidLength == 0 || ssidEnd > length + 1) return false;

    const uint8_t passwordLength = data[ssidEnd];
    const size_t passStart = ssidEnd + 1;
    const size_t passEnd = passStart + passwordLength;
    if (passwordLength > 126 || passEnd > length + 1) return false;

    ssid.assign(reinterpret_cast<const char*>(data + ssidStart), ssidLength);
    password.assign(reinterpret_cast<const char*>(data + passStart), passwordLength);
    return true;
}

}  // namespace Packets
}  // namespace SerialImprov
