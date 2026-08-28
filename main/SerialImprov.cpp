#include "SerialImprov.h"

#include <fcntl.h>
#include <unistd.h>

#include <cstdio>
#include <string>

#include "Network.h"
#include "SerialImprovPackets.h"
#include "Settings.h"
#include "defaults.h"
#include "globals.h"
#include "sdkconfig.h"
#include "util.h"

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
#include "driver/usb_serial_jtag.h"
#include "driver/usb_serial_jtag_vfs.h"
#else
#include "driver/uart.h"
#include "driver/uart_vfs.h"
#endif

namespace SerialImprov {

#define IMPROV_VERSION 1

uint8_t improvActive = 0;
uint8_t improvError = 0;

namespace {
bool isWifiProvisioned() {
    return Network::isOnline();
}

// Console is stdin/stdout (UART0 or USB-Serial-JTAG per sdkconfig); stdin is non-blocking.
int readByte() {
    uint8_t c;
    int n = read(STDIN_FILENO, &c, 1);
    return n == 1 ? c : -1;
}

void writeBytes(const uint8_t* data, size_t len) {
    fwrite(data, 1, len, stdout);
    fputc('\n', stdout);
    fflush(stdout);
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
void sendImprovRPCResponse(uint8_t commandId);
void sendImprovInfoResponse();

/**
 * @brief Parse and handle a single Improv Serial packet from the console.
 *
 * Validates the Improv packet header, version, type, length, and checksum, then dispatches
 * the contained RPC command (e.g., WiFi provisioning, state request, info request) and
 * emits the appropriate responses and state updates. Returns after a full packet is handled
 * or on validation timeout/failure.
 *
 * @param provisioning When true, state responses for request-state commands will indicate
 *                     provisioning rather than authorized or provisioned states.
 * @param first The header byte ('I') Loop() already consumed; fed to the parser as byte 0.
 */
void handleImprovPacket(bool provisioning, uint8_t first) {
    uint8_t header[6] = {'I', 'M', 'P', 'R', 'O', 'V'};

    bool timeout = false;
    uint8_t waitTime = 25;
    uint16_t packetByte = 0;
    uint8_t packetLen = 9;
    uint8_t checksum = 0;

    uint8_t rpcCommandType = 0;
    char rpcData[128];
    rpcData[0] = 0;

    int pending = first;

    while (!timeout) {
        if (pending < 0) pending = readByte();
        if (pending < 0) {
            delay(1);
            waitTime--;
            if (!waitTime) timeout = true;
            continue;
        }
        uint8_t next = (uint8_t)pending;
        pending = -1;

        switch (packetByte) {
            case ImprovPacketByte::Version: {
                if (next != IMPROV_VERSION) {
                    return;
                }
                break;
            }
            case ImprovPacketByte::PacketType: {
                if (next != ImprovPacketType::RPC_Command) {
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
                        sendImprovStateResponse(0x01, true);
                        return;
                    }

                    switch (rpcCommandType) {
                        case ImprovRPCType::Command_Wifi:
                            Log.println("[Improv] Command: WIFI");
                            parseWiFiCommand(rpcData);
                            break;
                        case ImprovRPCType::Request_State: {
                            Log.println("[Improv] Command: REQUEST_STATE");
                            uint8_t improvState = 0x02;                   // authorized
                            if (provisioning) improvState = 0x03;         // provisioning
                            if (isWifiProvisioned()) improvState = 0x04;  // provisioned
                            sendImprovStateResponse(improvState, false);
                            if (improvState == 0x04) sendImprovRPCResponse(ImprovRPCType::Request_State);
                            break;
                        }
                        case ImprovRPCType::Request_Info:
                            Log.println("[Improv] Command: REQUEST_INFO");
                            sendImprovInfoResponse();
                            break;
                        default: {
                            sendImprovStateResponse(0x02, true);
                        }
                    }
                    return;
                }
                if (packetByte < 6) {  // check header
                    if (next != header[packetByte]) {
                        return;
                    }
                } else if (packetByte > 9) {  // RPC data
                    const size_t rpcIndex = packetByte - 10;
                    if (rpcIndex >= sizeof(rpcData)) {
                        sendImprovStateResponse(0x01, true);
                        return;  // prevent buffer overflow
                    }
                    rpcData[rpcIndex] = next;
                }
            }
        }

        checksum += next;
        packetByte++;
    }
}

/**
 * @brief Sends an Improv protocol state packet over the console.
 *
 * Sends a state response packet followed by a newline, updates the global
 * improvError when `error` is true, and may emit an immediate error state
 * (0x00) if a prior transient error is recorded.
 */
void sendImprovStateResponse(uint8_t state, bool error) {
    if (!error && improvError > 0 && improvError < 3) sendImprovStateResponse(0x00, true);
    if (error) improvError = state;

    SerialImprov::Packets::PacketBuffer packet = SerialImprov::Packets::BuildStateResponse(state, error);
    writeBytes(packet.data, packet.size);
    Log.printf("[Improv] State %u (error=%s)\n", state, error ? "yes" : "no");
}

/**
 * @brief Builds and sends an Improv RPC response for the given command and marks provisioning complete.
 *
 * When the network is connected, the device's local HTTP URL is included in the response.
 */
void sendImprovRPCResponse(uint8_t commandId) {
    if (improvError > 0 && improvError < 3) sendImprovStateResponse(0x00, true);

    char url[32] = {0};
    bool includeUrl = false;
    if (isWifiProvisioned()) {
        std::string ip = Network::localIP();
        int len = snprintf(url, sizeof(url), "http://%s", ip.c_str());
        if (len > 0 && len < (int)sizeof(url)) includeUrl = true;
    }

    SerialImprov::Packets::PacketBuffer packet =
        SerialImprov::Packets::BuildRPCResponse(commandId, url, includeUrl);
    writeBytes(packet.data, packet.size);
    Log.printf("[Improv] RPC response %u (url=%s)\n", commandId, includeUrl ? url : "");
    improvActive = 1;  // no longer provisioning
}

/**
 * @brief Send an Improv "info" response (device name, firmware version, device type, room) over the console.
 */
void sendImprovInfoResponse() {
    if (improvError > 0 && improvError < 3) sendImprovStateResponse(0x00, true);

    const char* versionString = "Dev";
#ifdef VERSION
    char versionBuffer[32];
    snprintf(versionBuffer, sizeof(versionBuffer), "%s", VERSION);
    versionString = versionBuffer;
#endif

    SerialImprov::Packets::PacketBuffer packet =
        SerialImprov::Packets::BuildInfoResponse("ESPresense", versionString, "esp32", room.c_str());

    writeBytes(packet.data, packet.size);
    Log.println("[Improv] Info response sent");
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

    spurt("/wifi-ssid", ssid);
    spurt("/wifi-password", password);

    esp_restart();
}

bool setup = false;
void Setup() {
    if (setup) return;
    setup = true;
    // Route the console through the interrupt-driven driver so stdin reads see incoming bytes,
    // then make stdin non-blocking so Loop() can poll it.
#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    usb_serial_jtag_driver_config_t cfg = {};
    cfg.tx_buffer_size = 256;
    cfg.rx_buffer_size = 256;
    usb_serial_jtag_driver_install(&cfg);  // ESP_ERR_INVALID_STATE if already installed: fine
    usb_serial_jtag_vfs_use_driver();
#else
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);  // ESP_FAIL if already installed: fine
    uart_vfs_dev_use_driver(0);
#endif
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
}

void Loop(bool provisioning) {
    if (!setup) {
        setup = true;
        Setup();
    }

    // No peek on a fd: consume bytes until an 'I' shows up and hand that byte to the packet parser.
    int next;
    while ((next = readByte()) >= 0) {
        if (next == 'I') {
            handleImprovPacket(provisioning, (uint8_t)next);
            return;
        }
    }
}

}  // namespace SerialImprov
