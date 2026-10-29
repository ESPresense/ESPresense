#pragma once
// Pure parser for Teltonika EYE sensor/beacon manufacturer data, shared by the fingerprint code
// and host tests. Only "EYE Sensors" mode puts it in the advertisement itself. iBeacon/Eddystone +
// EYE Sensors moves it to the scan response, which a passive scan never requests.
//
// Layout (multi-byte values big-endian): [0..1] 0x089a LE, [2] protocol 0x01, [3] flags, then
// one field per set flag in this order:
//   bit0 temperature  2 bytes, signed, °C x 100
//   bit1 humidity     1 byte, %
//   bit4 movement     2 bytes (state + count)
//   bit5 angle        3 bytes (pitch, roll)
//   bit7 battery      1 byte, mV = 2000 + 10 x value
// bit2/bit3 (magnet present/state) and bit6 (low battery) carry no bytes.
#include <cstddef>
#include <cstdint>

struct TeltonikaEye {
    bool hasTemp = false, hasHumidity = false, hasVoltage = false;
    bool lowBattery = false;
    float temp = 0;
    uint8_t humidity = 0;
    uint16_t mv = 0;
};

// False unless data is a protocol 1 EYE frame with a flags byte; a field cut short by the frame
// end is left unset rather than read past it.
inline bool parseTeltonikaEye(const uint8_t* d, size_t len, TeltonikaEye& out) {
    out = TeltonikaEye{};
    if (len < 4 || d[0] != 0x9a || d[1] != 0x08 || d[2] != 0x01) return false;
    const uint8_t flags = d[3];
    out.lowBattery = flags & 0x40;
    size_t i = 4;
    if (flags & 0x01) {
        if (i + 2 > len) return true;
        out.temp = int16_t((d[i] << 8) | d[i + 1]) / 100.0f;
        out.hasTemp = true;
        i += 2;
    }
    if (flags & 0x02) {
        if (i + 1 > len) return true;
        out.humidity = d[i];
        out.hasHumidity = true;
        i += 1;
    }
    if (flags & 0x10) i += 2;
    if (flags & 0x20) i += 3;
    if ((flags & 0x80) && i + 1 <= len) {
        out.mv = 2000 + d[i] * 10;
        out.hasVoltage = true;
    }
    return true;
}
