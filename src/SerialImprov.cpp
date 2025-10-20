#include <AsyncMqttClient.h>
#include <HeadlessWiFiSettings.h>

#include <cstdio>
#include <pgmspace.h>
#include <string>

#include "Network.h"
#include "SPIFFS.h"
#include "SerialImprovPackets.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"

namespace SerialImprov {
#ifdef DEBUG_IMPROV
#define DIMPROV_PRINT(x) Serial2.print(x)
#define DIMPROV_PRINTLN(x) Serial2.println(x)
#define DIMPROV_PRINTF(x...) Serial2.printf(x)
#else
#define DIMPROV_PRINT(x)
#define DIMPROV_PRINTLN(x)
#define DIMPROV_PRINTF(x...)
#endif

#define IMPROV_VERSION 1

byte improvActive = 0;
byte improvError = 0;

namespace {
#ifdef ARDUINO
Stream* improvSerial = &Serial;
#else
Stream* improvSerial = nullptr;
#endif

Stream* EnsureSerial() {
#ifdef ARDUINO
    if (!improvSerial) improvSerial = &Serial;
#endif
    return improvSerial;
}
}  // namespace

void parseWiFiCommand(char* rpcData);

enum ImprovPacketType {
    Current_State = 0x01,
    Error_State = 0x02,
    RPC_Command = 0x03,
    RPC_Response = 0x04
};

enum ImprovPacketByte {
    Version = 6,
    PacketType = 7,
    Length = 8,
    RPC_CommandType = 9
};

enum ImprovRPCType {
    Command_Wifi = 0x01,
    Request_State = 0x02,
    Request_Info = 0x03
};

void sendImprovStateResponse(uint8_t state, bool error = false);
void sendImprovRPCResponse(byte commandId);
void sendImprovInfoResponse();

// blocking function to parse an Improv Serial packet
void handleImprovPacket(bool provisioning) {
    uint8_t header[6] = {'I', 'M', 'P', 'R', 'O', 'V'};

    bool timeout = false;
    uint8_t waitTime = 25;
    uint16_t packetByte = 0;
    uint8_t packetLen = 9;
    uint8_t checksum = 0;

    uint8_t rpcCommandType = 0;
    char rpcData[128];
    rpcData[0] = 0;

    Stream* serial = EnsureSerial();
    if (!serial) return;

    while (!timeout) {
        if (serial->available() < 1) {
            delay(1);
            waitTime--;
            if (!waitTime) timeout = true;
            continue;
        }
        byte next = serial->read();

        DIMPROV_PRINT("Received improv byte: ");
        DIMPROV_PRINTF("%x\r\n", next);
        switch (packetByte) {
            case ImprovPacketByte::Version: {
                if (next != IMPROV_VERSION) {
                    DIMPROV_PRINTLN(F("Invalid version"));
                    return;
                }
                break;
            }
            case ImprovPacketByte::PacketType: {
                if (next != ImprovPacketType::RPC_Command) {
                    DIMPROV_PRINTF("Non RPC-command improv packet type %i\n", next);
                    return;
                }
                if (!improvActive) improvActive = 1;
                break;
            }
            case ImprovPacketByte::Length:
                packetLen = 9 + next;
                break;
            case ImprovPacketByte::RPC_CommandType:
                rpcCommandType = next;
                break;
            default: {
                if (packetByte >= packetLen) {  // end of packet, check checksum match

                    if (checksum != next) {
                        DIMPROV_PRINTF("Got RPC checksum %i, expected %i", next, checksum);
                        sendImprovStateResponse(0x01, true);
                        return;
                    }

                    switch (rpcCommandType) {
                        case ImprovRPCType::Command_Wifi:
                            parseWiFiCommand(rpcData);
                            break;
                        case ImprovRPCType::Request_State: {
                            uint8_t improvState = 0x02;                     // authorized
                            if (provisioning) improvState = 0x03;   // provisioning
                            if (Network.isConnected()) improvState = 0x04;  // provisioned
                            sendImprovStateResponse(improvState, false);
                            if (improvState == 0x04) sendImprovRPCResponse(ImprovRPCType::Request_State);
                            break;
                        }
                        case ImprovRPCType::Request_Info:
                            sendImprovInfoResponse();
                            break;
                        default: {
                            DIMPROV_PRINTF("Unknown RPC command %i\n", next);
                            sendImprovStateResponse(0x02, true);
                        }
                    }
                    return;
                }
                if (packetByte < 6) {  // check header
                    if (next != header[packetByte]) {
                        DIMPROV_PRINTLN(F("Invalid improv header"));
                        return;
                    }
                } else if (packetByte > 9) {  // RPC data
                    rpcData[packetByte - 10] = next;
                    if (packetByte > 137) return;  // prevent buffer overflow
                }
            }
        }

        checksum += next;
        packetByte++;
    }
}

void sendImprovStateResponse(uint8_t state, bool error) {
    if (!error && improvError > 0 && improvError < 3) sendImprovStateResponse(0x00, true);
    if (error) improvError = state;
    Stream* serial = EnsureSerial();
    if (!serial) return;

    SerialImprov::Packets::PacketBuffer packet = SerialImprov::Packets::BuildStateResponse(state, error);
    serial->write(packet.data, packet.size);
    serial->write('\n');
}

void sendImprovRPCResponse(byte commandId) {
    if (improvError > 0 && improvError < 3) sendImprovStateResponse(0x00, true);
    Stream* serial = EnsureSerial();
    if (!serial) return;

    char url[32] = {0};
    bool includeUrl = false;
    if (Network.isConnected()) {
        IPAddress localIP = Network.localIP();
        uint8_t len = snprintf(url, sizeof(url), "http://%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);
        if (len > 0 && len < sizeof(url)) includeUrl = true;
    }

    SerialImprov::Packets::PacketBuffer packet =
        SerialImprov::Packets::BuildRPCResponse(commandId, url, includeUrl);
    serial->write(packet.data, packet.size);
    serial->write('\n');
    improvActive = 1;  // no longer provisioning
}

void sendImprovInfoResponse() {
    if (improvError > 0 && improvError < 3) sendImprovStateResponse(0x00, true);
    Stream* serial = EnsureSerial();
    if (!serial) return;

    char versionBuffer[32] = {};
    const char* versionString = "Dev";
#ifdef VERSION
    snprintf_P(versionBuffer, sizeof(versionBuffer), VERSION);
    versionString = versionBuffer;
#endif

    SerialImprov::Packets::PacketBuffer packet =
        SerialImprov::Packets::BuildInfoResponse("ESPresense", versionString, "esp32", room.c_str());

    serial->write(packet.data, packet.size);
    serial->write('\n');
    DIMPROV_PRINT("Info checksum");
    DIMPROV_PRINTLN(packet.data[packet.size - 1]);
}

void parseWiFiCommand(char* data) {
    const uint8_t data_len = static_cast<uint8_t>(data[0]);
    std::string ssid;
    std::string password;
    if (!SerialImprov::Packets::DecodeWifiCredentials(
            reinterpret_cast<const uint8_t*>(data), data_len, ssid, password)) {
        return;
    }

    sendImprovStateResponse(0x03);  // provisioning
    improvActive = 2;

    spurt("/wifi-ssid", String(ssid.c_str()));
    spurt("/wifi-password", String(password.c_str()));

    ESP.restart();
}
bool setup = false;
void Setup() {
    #ifdef DEBUG_IMPROV
    Serial2.begin(115200)
    DIMPROV_PRINT("Setup");
    #endif
}

void Loop(bool provisioning) {
    if (!setup) {
        setup = true;
        Setup();
    }
    Stream* serial = EnsureSerial();
    if (!serial) return;

    while (serial->available() > 0) {
        yield();
        int next = serial->peek();
        if (next == 'I') {
            handleImprovPacket(provisioning);
            return;
        }
        serial->read();
    }
}

void SetSerial(Stream* stream) {
#ifdef ARDUINO
    improvSerial = stream ? stream : &Serial;
#else
    improvSerial = stream;
#endif
}

}  // namespace SerialImprov
