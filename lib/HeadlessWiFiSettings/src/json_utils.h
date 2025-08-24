#pragma once

#include <Arduino.h>
#include <cstdint>
#include <cstdio>

namespace {
inline String json_encode(const String &raw) {
    String r;
    char buf[7];
    for (unsigned int i = 0; i < raw.length(); i++) {
        uint8_t c = raw[i];
        switch (c) {
            case '"': r += "\\\""; break;
            case '\\': r += "\\\\"; break;
            case '\b': r += "\\b"; break;
            case '\f': r += "\\f"; break;
            case '\n': r += "\\n"; break;
            case '\r': r += "\\r"; break;
            case '\t': r += "\\t"; break;
            default:
                if (c < 0x20) {
                    std::snprintf(buf, sizeof(buf), "\\u%04x", c);
                    r += buf;
                } else {
                    r += static_cast<char>(c);
                }
        }
    }
    return r;
}
} // namespace

