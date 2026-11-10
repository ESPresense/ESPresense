#pragma once
#include <cstdint>

// ESP-IDF's nano printf does not support the ll length modifier. Keep exact
// integer timestamps/counters without routing them through printf or a float.
inline const char* csiReferenceDecimal(uint64_t value, char (&buffer)[21]) {
    char* cursor = buffer + 20;
    *cursor = '\0';
    do {
        *--cursor = static_cast<char>('0' + value % 10);
        value /= 10;
    } while (value);
    return cursor;
}
