#ifndef _RSSI_
#define _RSSI_

#include <cstdint>

// "No 1 m reference". Outside any real value: references and their sums (ref_rssi + tx power,
// Eddystone power - 41) can go below -128, which int8_t couldn't hold (-141 wrapped to +115).
#define NO_RSSI int16_t(INT16_MIN)

#define CLOSE_RSSI int8_t(-40)
#define LEFT_RSSI int8_t(-50)

#define DEFAULT_TX int8_t(-6)

#define APPLE_TX int8_t(0)
#define RM_ASST_TX int8_t(0)
#define TILE_TX int8_t(-4)
#define EXPOSURE_TX int8_t(-12)
#define ITAG_TX int8_t(-10)

#define NUT_TX int8_t(-12)
#define FLORA_TX int8_t(-10)

#define EDDYSTONE_ADD_1M int8_t(-41)

// Eddystone advertises its power at 0 m; the 1 m reference is 41 dB lower.
inline int16_t eddystone1mRssi(int8_t power0m) { return int16_t(EDDYSTONE_ADD_1M + power0m); }
#endif
