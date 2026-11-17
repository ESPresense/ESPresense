#pragma once
#include <cstdint>
#include <cstdlib>

// Reference-only radio-sharing requests for the bench. Pure parsing and
// arithmetic, so host tests cover every bound; nothing here touches a radio.
//
// The ESP32-C3 coexistence library picks a fixed schedule from the BLE and
// Wi-Fi state. Layout: phase count, period in beacon intervals, then four
// bytes per phase (percent, Wi-Fi flags, two BT callback bytes). With the
// station connected, BLE status 0x02 alone selects
// coex_schm_ble_mesh_traffic_wifi_conn (60% Wi-Fi then 40% BLE per beacon
// interval); bench pilot 2047 read that table back while scanning. Other
// non-mesh BLE status selects coex_schm_ble_default_bt_idle_wifi_conn (50/50).
constexpr uint8_t CSI_REFERENCE_COEX_SCAN_TABLE[10] = {2, 1, 60, 3, 0, 0, 40, 0, 0, 0};
constexpr uint8_t CSI_REFERENCE_COEX_DEFAULT_TABLE[10] = {2, 1, 50, 3, 0, 0, 50, 0, 0, 0};
// The station's default beacon timeout is 6 s; keep a BLE phase well below it.
constexpr unsigned CSI_REFERENCE_COEX_MAX_BT_PHASE_INTERVALS = 30;

struct CsiReferenceCoexRequest {
    bool valid = false;  // false keeps the library schedule
    uint8_t period = 1;  // beacon intervals per cycle
    uint8_t wifi = 50;   // percent of the cycle, Wi-Fi phase first
    uint8_t bt = 50;
};

struct CsiReferenceScanCycle {
    bool valid = false;  // false scans continuously
    uint16_t pauseSeconds = 0;
    uint16_t periodSeconds = 0;
};

namespace csi_reference_detail {
// Strict decimal fields: no sign, space, empty field or trailing text.
inline bool fields(const char* text, long* out, unsigned count) {
    if (!text) return false;
    for (unsigned i = 0; i < count; ++i) {
        if (*text < '0' || *text > '9') return false;
        char* end = nullptr;
        out[i] = std::strtol(text, &end, 10);
        if (end - text > 5) return false;
        text = end;
        if (*text != (i + 1 < count ? ',' : '\0')) return false;
        ++text;
    }
    return true;
}
}  // namespace csi_reference_detail

// "P,W,B": P beacon intervals per cycle, W percent Wi-Fi then B percent BLE.
// Anything else, including the default "0", keeps the library schedule.
inline CsiReferenceCoexRequest csiReferenceParseCoex(const char* text) {
    long v[3];
    CsiReferenceCoexRequest request;
    if (!csi_reference_detail::fields(text, v, 3)) return request;
    if (v[0] < 1 || v[0] > 60 || v[1] < 5 || v[1] > 95 || v[2] < 5 || v[1] + v[2] != 100) return request;
    if (v[0] * v[2] > long(CSI_REFERENCE_COEX_MAX_BT_PHASE_INTERVALS) * 100) return request;
    request.valid = true;
    request.period = static_cast<uint8_t>(v[0]);
    request.wifi = static_cast<uint8_t>(v[1]);
    request.bt = static_cast<uint8_t>(v[2]);
    return request;
}

// "C,T": pause BLE scanning for C seconds of every T seconds. Scanning keeps
// at least half of every cycle and a pause never exceeds ten seconds.
inline CsiReferenceScanCycle csiReferenceParseScanCycle(const char* text) {
    long v[2];
    CsiReferenceScanCycle cycle;
    if (!csi_reference_detail::fields(text, v, 2)) return cycle;
    if (v[0] < 1 || v[0] > 10 || v[1] > 120 || v[1] < 2 * v[0]) return cycle;
    cycle.valid = true;
    cycle.pauseSeconds = static_cast<uint16_t>(v[0]);
    cycle.periodSeconds = static_cast<uint16_t>(v[1]);
    return cycle;
}

// The pause comes first in each cycle, counted from when the plan started.
inline bool csiReferenceScanPausedAt(uint64_t elapsedMs, const CsiReferenceScanCycle& cycle) {
    return cycle.valid && elapsedMs % (uint64_t(cycle.periodSeconds) * 1000) < uint64_t(cycle.pauseSeconds) * 1000;
}

// Pilot 2049: cancelling a running scan does not return the library to its
// BLE-idle (all Wi-Fi) schedule. It moves to the 50/50 table, so a scan pause
// alone left coverage near 40%. While scanning is paused BLE needs no radio
// time, so that table runs this share for the pause.
constexpr CsiReferenceCoexRequest CSI_REFERENCE_COEX_PAUSED_SHARE{true, 1, 95, 5};
constexpr CsiReferenceCoexRequest CSI_REFERENCE_COEX_DEFAULT_SHARE{true, 1, 50, 50};

// The pinned SDK calibrates over ten consecutive ready one-second windows and
// restarts after any one-second silence. A plan with whole-second BLE periods
// cannot supply that, so the SDK calibrates once with scanning paused.
inline bool csiReferenceNeedsCalibrationAssist(const CsiReferenceCoexRequest& coex,
                                               const CsiReferenceScanCycle& cycle) {
    return cycle.valid || (coex.valid && coex.period > 4);
}

// Only the period and the two shares change; phase count and flags stay as
// the library defined them, so each single-byte write leaves a valid table.
inline void csiReferenceApplyCoex(volatile uint8_t* table, const CsiReferenceCoexRequest& request) {
    if (!request.valid) return;
    table[1] = request.period;
    table[2] = request.wifi;
    table[6] = request.bt;
}
