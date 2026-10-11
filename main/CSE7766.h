#pragma once
#include <cstddef>
#include <cstdint>

// CSE7766 (Athom, Sonoff POW R2 / S31 plugs): a 24-byte frame every ~50 ms at 4800 baud 8E1.
// The chip is factory calibrated: each frame carries the coefficients and the measured cycle
// lengths, so value = coefficient / cycle. Decoding follows ESPHome's cse7766 component.
//   [0] state  [1] 0x5A  [2..4] V coef  [5..7] V cycle  [8..10] I coef  [11..13] I cycle
//   [14..16] P coef  [17..19] P cycle  [20] adj  [21..22] CF pulse count  [23] checksum of 2..22
// No hardware here, so it runs in the host tests.

struct CseReading {
    float voltage = 0, current = 0, power = 0;
    uint16_t cfPulses = 0;
    float joulesPerPulse = 0;  // power coefficient / 1e6, for energy from the pulse count
};

inline uint32_t cse24(const uint8_t* f, size_t i) { return (uint32_t)f[i] << 16 | (uint32_t)f[i + 1] << 8 | f[i + 2]; }

// True when f[0..23] is a valid frame; fills out. A state byte of 0xF? flags cycles that ran past
// the chip's range (no load / no current): those values are reported as 0.
inline bool cseDecode(const uint8_t* f, CseReading& out) {
    uint8_t state = f[0];
    if (f[1] != 0x5A || (state != 0x55 && (state & 0xF0) != 0xF0)) return false;
    uint8_t sum = 0;
    for (size_t i = 2; i < 23; i++) sum += f[i];
    if (sum != f[23]) return false;
    if ((state & 0xF1) == 0xF1) return false;  // coefficient storage abnormal

    uint32_t vCoef = cse24(f, 2), vCycle = cse24(f, 5);
    uint32_t iCoef = cse24(f, 8), iCycle = cse24(f, 11);
    uint32_t pCoef = cse24(f, 14), pCycle = cse24(f, 17);
    uint8_t adj = f[20];
    bool overflow = (state & 0xF0) == 0xF0;

    out = {};
    if ((adj & 0x40) && vCycle && !(overflow && (state & 0x08))) out.voltage = (float)vCoef / vCycle;
    if ((adj & 0x10) && pCycle && !(overflow && (state & 0x02))) out.power = (float)pCoef / pCycle;
    if ((adj & 0x20) && iCycle && out.power > 0 && !(overflow && (state & 0x04))) out.current = (float)iCoef / iCycle;
    out.cfPulses = (uint16_t)(f[21] << 8 | f[22]);
    out.joulesPerPulse = pCoef / 1e6f;
    return true;
}
