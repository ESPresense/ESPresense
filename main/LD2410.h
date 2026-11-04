#pragma once
#include <cstddef>
#include <cstdint>

// HLK-LD2410 / LD2410B / LD2410C 24 GHz presence radar: read-only decoding of the report frames
// it streams over UART (256000 8N1 by default, about 10 frames a second). Layout follows the
// Hi-Link serial protocol manual and ESPHome's ld2410 component:
//   F4 F3 F2 F1 | len (2, LE) | payload (len bytes) | F8 F7 F6 F5
// payload: [0] type (0x02 basic, 0x01 engineering) [1] 0xAA [2] target state (bit 0 moving,
// bit 1 still) [3..4] moving distance cm [5] moving energy [6..7] still distance cm
// [8] still energy [9..10] detection distance cm ... [len-2] 0x55 [len-1] 0x00.
// Engineering frames add per-gate energies after byte 10; only the common fields are read.
// Command ACKs (FD FC FB FA ...) only follow commands, and this never sends any.
// No hardware here, so it runs in the host tests.

struct Ld2410Reading {
    bool moving = false, still = false;
    uint16_t movingCm = 0, stillCm = 0, detectionCm = 0;
    uint8_t movingEnergy = 0, stillEnergy = 0;  // 0-100
};

// Feed it the UART byte stream one byte at a time; feed() returns true when a byte completes a
// valid frame and fills out. Resynchronises on the next header after garbage or a bad frame.
class Ld2410Parser {
public:
    bool feed(uint8_t b, Ld2410Reading& out) {
        static const uint8_t head[4] = {0xF4, 0xF3, 0xF2, 0xF1};
        static const uint8_t tail[4] = {0xF8, 0xF7, 0xF6, 0xF5};
        if (len_ < 4) {
            if (b == head[len_]) buf_[len_++] = b;
            else len_ = (b == head[0]) ? (buf_[0] = b, 1) : 0;
            return false;
        }
        buf_[len_++] = b;
        if (len_ == 6) {
            payload_ = buf_[4] | (size_t)buf_[5] << 8;
            if (payload_ < MIN_PAYLOAD || 6 + payload_ + 4 > sizeof(buf_)) return reset(b);
        }
        if (len_ < 6 || len_ < 6 + payload_ + 4) return false;
        bool ok = true;
        for (int i = 0; i < 4; i++) ok = ok && buf_[6 + payload_ + i] == tail[i];
        const uint8_t* p = buf_ + 6;
        ok = ok && (p[0] == 0x01 || p[0] == 0x02) && p[1] == 0xAA && p[payload_ - 2] == 0x55 && p[payload_ - 1] == 0x00;
        len_ = 0;
        if (!ok) return false;
        out.moving = p[2] & 0x01;
        out.still = p[2] & 0x02;
        out.movingCm = p[3] | p[4] << 8;
        out.movingEnergy = p[5];
        out.stillCm = p[6] | p[7] << 8;
        out.stillEnergy = p[8];
        out.detectionCm = p[9] | p[10] << 8;
        return true;
    }

private:
    static constexpr size_t MIN_PAYLOAD = 13;  // a basic frame
    // Engineering frames are 0x23 (35) bytes of payload; leave room for firmware that adds more.
    uint8_t buf_[80] = {};
    size_t len_ = 0;
    size_t payload_ = 0;

    // A bad length: drop the frame, but this byte may start the next one.
    bool reset(uint8_t b) {
        len_ = 0;
        if (b == 0xF4) buf_[len_++] = b;
        return false;
    }
};
