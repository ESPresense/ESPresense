#pragma once
#include <cstddef>
#include <cstdint>

// Parses Teltonika EYE beacon manufacturer-specific data (Company ID 0x089A, protocol
// version 0x01), as carried in the BLE Scan Response. `data`/`len` must point at the
// manufacturer-specific-data payload of a single AD structure (i.e. starting with the
// little-endian company ID), not the whole advertisement/scan-response buffer.
//
// Layout: 9A 08 (company id, LE) | 01 (protocol version) | flags | rawBattery
//   flags bit6 = low battery indicator, bit7 = battery voltage present
//   voltage_mV = 2000 + rawBattery * 10, only meaningful when bit7 is set
struct TeltonikaEyeData {
    bool valid = false;       // true if the 0x089A/v1 header matched
    bool hasFlags = false;    // true if the flags byte was present (header-only payloads leave lowBattery unset)
    bool hasVoltage = false;  // true if the voltage-present flag was set and a raw byte was available
    uint16_t mv = 0;
    bool lowBattery = false;
};

inline TeltonikaEyeData parseTeltonikaEye(const uint8_t *data, size_t len) {
    TeltonikaEyeData result;
    if (data == nullptr || len < 3) return result;
    if (data[0] != 0x9A || data[1] != 0x08) return result;  // Company ID 0x089A, little-endian
    if (data[2] != 0x01) return result;                     // Only protocol version 1 is supported

    result.valid = true;
    if (len < 4) return result;

    const uint8_t flags = data[3];
    result.hasFlags = true;
    result.lowBattery = (flags & 0x40) != 0;
    const bool voltagePresent = (flags & 0x80) != 0;
    if (voltagePresent && len >= 5) {
        const uint8_t rawBattery = data[4];
        result.mv = static_cast<uint16_t>(2000 + rawBattery * 10);
        result.hasVoltage = true;
    }
    return result;
}
