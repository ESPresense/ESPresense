#include <AsyncMqttClient.h>
#include <AsyncWiFiSettings.h>

#include "Network.h"
#include "SPIFFS.h"
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

    while (!timeout) {
        if (Serial.available() < 1) {
            delay(1);
            waitTime--;
            if (!waitTime) timeout = true;
            continue;
        }
        byte next = Serial.read();

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
    char out[11] = {'I', 'M', 'P', 'R', 'O', 'V'};
    out[6] = IMPROV_VERSION;
    out[7] = error ? ImprovPacketType::Error_State : ImprovPacketType::Current_State;
    out[8] = 1;
    out[9] = state;

    uint8_t checksum = 0;
    for (uint8_t i = 0; i < 10; i++) checksum += out[i];
    out[10] = checksum;
    Serial.write((uint8_t*)out, 11);
    Serial.write('\n');
}

void sendImprovRPCResponse(byte commandId) {
    if (improvError > 0 && improvError < 3) sendImprovStateResponse(0x00, true);
    uint8_t packetLen = 12;
    char out[64] = {'I', 'M', 'P', 'R', 'O', 'V'};
    out[6] = IMPROV_VERSION;
    out[7] = ImprovPacketType::RPC_Response;
    out[8] = 2;  // Length (set below)
    out[9] = commandId;
    out[10] = 0;     // Data len (set below)
    out[11] = '\0';  // URL len (set below)

    if (Network.isConnected()) {
        IPAddress localIP = Network.localIP();
        uint8_t len = sprintf(out + 12, "http://%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);
        if (len > 24) return;  // sprintf fail?
        out[11] = len;
        out[10] = 1 + len;
        out[8] = 3 + len;  // RPC command type + data len + url len + url
        packetLen = 13 + len;
    }

    uint8_t checksum = 0;
    for (uint8_t i = 0; i < packetLen - 1; i++) checksum += out[i];
    out[packetLen - 1] = checksum;
    Serial.write((uint8_t*)out, packetLen);
    Serial.write('\n');
    improvActive = 1;  // no longer provisioning
}

void sendImprovInfoResponse() {
    if (improvError > 0 && improvError < 3) sendImprovStateResponse(0x00, true);
    uint8_t packetLen = 12;
    char out[128] = {'I', 'M', 'P', 'R', 'O', 'V'};
    out[6] = IMPROV_VERSION;
    out[7] = ImprovPacketType::RPC_Response;
    // out[8] = 2; //Length (set below)
    out[9] = ImprovRPCType::Request_Info;
    // out[10] = 0; //Data len (set below)
    out[11] = 10;  // Firmware len ("WLED")
    out[12] = 'E';
    out[13] = 'S';
    out[14] = 'P';
    out[15] = 'r';
    out[16] = 'e';
    out[17] = 's';
    out[18] = 'e';
    out[19] = 'n';
    out[20] = 's';
    out[21] = 'e';
    uint8_t lengthSum = 23;
#ifdef VERSION
    uint8_t vlen = sprintf_P(out + lengthSum, VERSION);
#else
    uint8_t vlen = sprintf_P(out + lengthSum, "Dev");
#endif
    out[22] = vlen;
    lengthSum += vlen;
    uint8_t hlen = 7;
    hlen = 5;
    strcpy(out + lengthSum + 1, "esp32");
    out[lengthSum] = hlen;
    lengthSum += hlen + 1;
    strcpy(out + lengthSum + 1, room.c_str());
    uint8_t nlen = room.length();
    out[lengthSum] = nlen;
    lengthSum += nlen + 1;

    packetLen = lengthSum + 1;
    out[8] = lengthSum - 9;
    out[10] = lengthSum - 11;
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < packetLen - 1; i++) checksum += out[i];
    out[packetLen - 1] = checksum;
    Serial.write((uint8_t*)out, packetLen);
    Serial.write('\n');
    DIMPROV_PRINT("Info checksum");
    DIMPROV_PRINTLN(checksum);
}

void parseWiFiCommand(char* data) {
    uint8_t len = data[0];
    if (!len || len > 126) return;

    uint8_t ssid_length = data[1];
    uint8_t ssid_start = 2;
    size_t ssid_end = ssid_start + ssid_length;

    uint8_t pass_length = data[ssid_end];
    size_t pass_start = ssid_end + 1;
    size_t pass_end = pass_start + pass_length;

    std::string ssid(data + ssid_start, data + ssid_end);
    std::string password(data + pass_start, data + pass_end);

    sendImprovStateResponse(0x03);  // provisioning
    improvActive = 2;

    spurt("/wifi-ssid", ssid.c_str());
    spurt("/wifi-password", password.c_str());

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
    while (Serial.available() > 0) {
        yield();
        byte next = Serial.peek();
        if (next == 'I') {
            handleImprovPacket(provisioning);
            return;
        }
        Serial.read();
    }
}

}  // namespace SerialImprov
