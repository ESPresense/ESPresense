#ifdef SENSORS
#include "UARTSensor.h"

#include "Logger.h"
#include "defaults.h"
#include <HeadlessWiFiSettings.h>
#include <algorithm>
#include <vector>

namespace UARTSensor {
namespace {
    bool enabled = false;
    int rxPin = -1;
    int txPin = -1;
    int baudRate = 9600;
    int protocol = 0;

    const char *protocolName() {
        switch (protocol) {
            case 1: return "PM1006";
            case 2: return "PMSx003";
            case 3: return "MH-Z19";
            default: return "none";
        }
    }
}

void ConnectToWifi() {
    enabled = HeadlessWiFiSettings.checkbox("uart_sensor_enabled", false, "Enable UART sensor scaffold (experimental)");
    rxPin = HeadlessWiFiSettings.integer("uart_sensor_rx_pin", -1, "UART sensor RX pin (-1 for unset)");
    txPin = HeadlessWiFiSettings.integer("uart_sensor_tx_pin", -1, "UART sensor TX pin (-1 for unset)");

    std::vector<String> baudOptions = {"1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"};
    int baudIndex = HeadlessWiFiSettings.dropdown("uart_sensor_baud", baudOptions, 3, "UART sensor baud");
    baudRate = baudOptions[std::max(0, std::min((int)baudOptions.size() - 1, baudIndex))].toInt();

    std::vector<String> protocolOptions = {"None", "PM1006", "PMSx003", "MH-Z19"};
    protocol = HeadlessWiFiSettings.dropdown("uart_sensor_protocol", protocolOptions, 0, "UART sensor protocol (scaffold)");
}

void Setup() {
    if (!enabled) return;

    if (rxPin < 0 || txPin < 0 || protocol == 0) {
        Log.println("UART sensor scaffold enabled but incomplete config (rx/tx/protocol); parser disabled");
        return;
    }

    Log.printf("UART sensor scaffold configured: protocol=%s rx=%d tx=%d baud=%d\r\n", protocolName(), rxPin, txPin, baudRate);
    Log.println("UART sensor parser not implemented yet; no UART data is consumed in this build");
}

void SerialReport() {
    if (!enabled) return;

    Log.print("UART Sensor: ");
    Log.printf("enabled (protocol=%s rx=%d tx=%d baud=%d)\r\n", protocolName(), rxPin, txPin, baudRate);
}

void Loop() {
}

bool SendDiscovery() {
    return true;
}

} // namespace UARTSensor

#endif
