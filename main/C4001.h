#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

// DFRobot Gravity C4001 (SEN0609 / SEN0610) 24 GHz presence radar: read-only decoding of the
// text lines it streams over UART (9600 8N1 in DFRobot's examples). Layout follows DFRobot's
// DFRobot_C4001 library (anaysisData). Which line it sends depends on the run mode saved in the
// module (setRunApp, from DFRobot's tools):
//   presence mode: $DFHPD,<1 present | 0 clear>, , , *
//   speed mode:    $DFDMD,<targets>,<target>,<distance m>,<speed m/s>,<energy>, , *
// Command responses ($JYRPO..., "Done", "leapMMW:/>") are ignored; this never sends commands.
// No hardware here, so it runs in the host tests.

struct C4001Reading {
    bool speedMode = false;  // false: a $DFHPD line, which only carries presence
    bool present = false;
    uint16_t distanceCm = 0;
    int16_t speedCms = 0;
    uint32_t energy = 0;
};

// Feed it the UART byte stream one byte at a time; feed() returns true when a byte ends a
// recognised line and fills out.
class C4001Parser {
public:
    bool feed(uint8_t b, C4001Reading& out) {
        if (b == '$') {
            len_ = 0;
            buf_[len_++] = '$';
            return false;
        }
        if (len_ == 0) return false;
        if (b != '*' && b != '\r' && b != '\n') {
            if (len_ < sizeof(buf_) - 1) buf_[len_++] = b;
            else len_ = 0;  // runaway line
            return false;
        }
        buf_[len_] = 0;
        len_ = 0;
        return parse(out);
    }

private:
    char buf_[96] = {};
    size_t len_ = 0;

    // Splits buf_ on commas in place; fields keep their spaces (" " for an empty one).
    int split(char** fields, int max) {
        int n = 0;
        char* p = buf_;
        while (n < max) {
            fields[n++] = p;
            p = strchr(p, ',');
            if (!p) break;
            *p++ = 0;
        }
        return n;
    }

    static bool number(const char* s, double& v) {
        char* end;
        v = strtod(s, &end);
        return end != s;
    }

    bool parse(C4001Reading& out) {
        char* f[8];
        int n = split(f, 8);
        if (strcmp(f[0], "$DFHPD") == 0) {
            if (n < 2 || (f[1][0] != '0' && f[1][0] != '1')) return false;
            out = {};
            out.present = f[1][0] == '1';
            return true;
        }
        if (strcmp(f[0], "$DFDMD") == 0) {
            double num, dist, speed, energy;
            if (n < 6 || !number(f[1], num) || !number(f[3], dist) || !number(f[4], speed) || !number(f[5], energy)) return false;
            out = {};
            out.speedMode = true;
            out.present = num > 0;
            if (out.present) {
                out.distanceCm = (uint16_t)(dist * 100 + 0.5);
                out.speedCms = (int16_t)(speed * 100 + (speed < 0 ? -0.5 : 0.5));
                out.energy = (uint32_t)energy;
            }
            return true;
        }
        return false;
    }
};
