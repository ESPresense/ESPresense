#pragma once
// Pure parser for the Apple iBeacon manufacturer-data frame, shared by the advert accessor and
// host tests. Layout: [0..1] 0x004c LE, [2] 0x02, [3] 0x15, [4..19] uuid, [20..21] major,
// [22..23] minor, [24] rssi@1m.
#include <cstddef>
#include <cstdint>
#include <cstring>

// The node's own iBeacon advertisement: flags, then Apple manufacturer data with the uuid
// big-endian (uuidLE is its little-endian bytes, as NimBLE stores a 128-bit uuid), major, minor
// and tx (rssi@1m) as the last byte. Returns the length, 30; out needs 31 bytes.
inline size_t buildIBeaconAdv(uint8_t out[31], const uint8_t uuidLE[16], uint16_t major, uint16_t minor, int8_t tx) {
    uint8_t* p = out;
    *p++ = 2; *p++ = 0x01; *p++ = 0x04;  // flags: BR/EDR not supported
    *p++ = 26; *p++ = 0xFF; *p++ = 0x4C; *p++ = 0x00; *p++ = 0x02; *p++ = 0x15;
    for (int i = 15; i >= 0; i--) *p++ = uuidLE[i];
    *p++ = major >> 8; *p++ = major & 0xFF;
    *p++ = minor >> 8; *p++ = minor & 0xFF;
    *p++ = (uint8_t)tx;
    return p - out;
}

inline bool iBeaconUuid(const uint8_t* mfg, size_t len, uint8_t out[16]) {
    if (len != 25 || mfg[0] != 0x4c || mfg[1] != 0x00 || mfg[2] != 0x02 || mfg[3] != 0x15) return false;
    memcpy(out, mfg + 4, 16); /* Flawfinder: ignore */
    return true;
}

// Decides which same-MAC fingerprint an advert feeds. Feed candidates newest first; a fingerprint
// locked to the advert's UUID wins immediately, otherwise the newest fingerprint with no UUID yet,
// otherwise none (caller creates one). A non-iBeacon advert takes the newest candidate.
struct IBeaconPick {
    static constexpr size_t none = static_cast<size_t>(-1);
    bool isBeacon;
    uint16_t crc;
    size_t locked = none, unlocked = none;

    // Returns true when the search can stop.
    bool consider(size_t i, bool hasUuid, uint16_t uuidCrc) {
        if (!isBeacon || (hasUuid && uuidCrc == crc)) {
            locked = i;
            return true;
        }
        if (!hasUuid && unlocked == none) unlocked = i;
        return false;
    }
    size_t result() const { return locked != none ? locked : unlocked; }
};
