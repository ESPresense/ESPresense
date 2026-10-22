#ifndef _RSSI_
#define _RSSI_

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
#endif
