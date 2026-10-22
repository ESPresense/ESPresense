#ifndef _RSSI_
#define _RSSI_

#include <cstdint>

// A 1 m reference. Every field and config value that holds one uses this type, so the host test
// that checks its range also covers them.
typedef int16_t rssi1m_t;

// "No 1 m reference". Outside any real value: references and their sums (ref_rssi + tx power,
// Eddystone power - 41) can go below -128, which int8_t couldn't hold (-141 wrapped to +115).
#define NO_RSSI rssi1m_t(INT16_MIN)

#define CLOSE_RSSI int8_t(-40)
#define LEFT_RSSI int8_t(-50)

// Close/left hysteresis on normalized RSSI, which already has the receiver adjustment applied
// (once): enter above CLOSE_RSSI, leave below LEFT_RSSI, otherwise keep the state.
inline bool nextClose(bool close, float rssi) {
    if (!close && rssi > CLOSE_RSSI) return true;
    if (close && rssi < LEFT_RSSI) return false;
    return close;
}

#define DEFAULT_TX int8_t(-6)

#define APPLE_TX int8_t(0)
#define RM_ASST_TX int8_t(0)
#define TILE_TX int8_t(-4)
#define EXPOSURE_TX int8_t(-12)
#define ITAG_TX int8_t(-10)

#define NUT_TX int8_t(-12)
#define FLORA_TX int8_t(-10)

#define EDDYSTONE_ADD_1M int8_t(-41)

// An int8_t rssi setting from a long (MQTT payload or NVS), clamped to [lo, hi] before the cast so
// out-of-range input can't wrap (-200 would become +56).
inline int8_t clampRssi(long v, int8_t lo, int8_t hi) { return int8_t(v < lo ? lo : v > hi ? hi : v); }

// Eddystone advertises its power at 0 m; the 1 m reference is 41 dB lower.
inline rssi1m_t eddystone1mRssi(int8_t power0m) { return rssi1m_t(EDDYSTONE_ADD_1M + power0m); }
#endif
