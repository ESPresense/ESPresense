#pragma once
#include <cmath>
#include <cstddef>
#include <cstdint>

// HLK-LD2450 24 GHz tracking radar: read-only decoding of its report frames (256000 8N1 by
// default, about 10 a second). Layout follows the Hi-Link serial protocol manual and ESPHome's
// ld2450 component; one 30-byte frame carries three target slots:
//   AA FF 03 00 | target 1 (8) | target 2 (8) | target 3 (8) | 55 CC
// target: x (2) y (2) speed (2) distance resolution (2), little-endian. x, y (mm) and speed
// (cm/s) are sign-magnitude with the top bit set for positive; resolution is plain mm.
// An empty slot is all zeros. y is the distance straight out from the sensor, x is left/right.
// Command ACKs (FD FC FB FA ...) only follow commands, and this never sends any.
// No hardware here, so it runs in the host tests.

struct Ld2450Target {
    bool present = false;
    int16_t x = 0, y = 0;      // mm
    int16_t speed = 0;         // cm/s; negative while approaching
    uint16_t resolution = 0;   // mm
    uint16_t distance() const { return (uint16_t)std::lround(std::sqrt((double)x * x + (double)y * y)); }
};

struct Ld2450Reading {
    static constexpr int TARGETS = 3;
    Ld2450Target targets[TARGETS];
    int count() const {
        int n = 0;
        for (auto& t : targets) n += t.present;
        return n;
    }
};

inline int16_t ld2450Signed(uint16_t raw) { return (raw & 0x8000) ? (int16_t)(raw & 0x7FFF) : (int16_t)-(int16_t)(raw & 0x7FFF); }

// Feed it the UART byte stream one byte at a time; feed() returns true when a byte completes a
// valid frame and fills out. Resynchronises on the next header after garbage or a bad frame.
class Ld2450Parser {
public:
    static constexpr size_t FRAME = 30;

    bool feed(uint8_t b, Ld2450Reading& out) {
        static const uint8_t head[4] = {0xAA, 0xFF, 0x03, 0x00};
        if (len_ < 4) {
            if (b == head[len_]) buf_[len_++] = b;
            else len_ = (b == head[0]) ? (buf_[0] = b, 1) : 0;
            return false;
        }
        buf_[len_++] = b;
        if (len_ < FRAME) return false;
        len_ = 0;
        if (buf_[28] != 0x55 || buf_[29] != 0xCC) return false;
        for (int i = 0; i < Ld2450Reading::TARGETS; i++) {
            const uint8_t* t = buf_ + 4 + i * 8;
            uint16_t rx = t[0] | t[1] << 8, ry = t[2] | t[3] << 8, rs = t[4] | t[5] << 8, rr = t[6] | t[7] << 8;
            auto& o = out.targets[i];
            o.present = rx || ry || rs || rr;
            o.x = o.present ? ld2450Signed(rx) : 0;
            o.y = o.present ? ld2450Signed(ry) : 0;
            o.speed = o.present ? ld2450Signed(rs) : 0;
            o.resolution = o.present ? rr : 0;
        }
        return true;
    }

private:
    uint8_t buf_[FRAME] = {};
    size_t len_ = 0;
};
