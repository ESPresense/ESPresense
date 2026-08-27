#pragma once

#include <cstdint>
#include <string>

// MikroTik TG-BT5 tags advertise a vendor-specific manufacturer-data frame alongside their
// optional iBeacon/Eddystone modes. Recognizing the native frame lets ESPresense give the tag
// a stable `mikrotik:{mac}` id without requiring iBeacon mode.
//
// Frame layout (manufacturer data, i.e. including the 2-byte company id as returned by
// NimBLE's getManufacturerData()):
//   [0..1]  company id 0x094f (MikroTik BVI Ltd), little-endian on air -> bytes 4f 09
//   [2]     format version (0x01 in every payload documented so far)
//   [3]     user data / FLAG_ENCRYPTED marker
//   [4..5]  salt
//   [6..17] "secret" fields: acceleration xyz, temperature, uptime, flags
//   [18]    secret flags
//   [19]    battery percentage
// Total length is 20 bytes. Only identification is done here; the sensor payload is a
// separable follow-up. See
// https://help.mikrotik.com/docs/display/UM/MikroTik+Tag+advertisement+formats
inline bool isMikrotikTagManufacturerData(const std::string& manufacturerData) {
    return manufacturerData.length() == 20 && static_cast<uint8_t>(manufacturerData[0]) == 0x4f && static_cast<uint8_t>(manufacturerData[1]) == 0x09 &&
           static_cast<uint8_t>(manufacturerData[2]) == 0x01;
}
