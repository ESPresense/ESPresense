#pragma once
#include <Arduino.h>
#include <WString.h>
#include <string>
#include <cstring>

// CHIPID is the lower 24 bits of the eFuse MAC, i.e. the NIC-specific OUI
// suffix + per-device unique bytes. Historically this used (>> 24), which
// happened to yield the unique bytes on classic ESP32 (Espressif OUI
// 24:0A:C4 lives in byte[0..2]) but extracts the wrong bytes on ESP32-C6
// and ESP32-S3, causing two devices to publish to the same MQTT topic.
// Mask the lower 3 bytes so it works on all current variants. Note that
// %06x does NOT cap to 6 digits, so masking to 24 bits is required to
// keep ESPMAC at exactly 6 hex chars.
#define CHIPID ((unsigned int)(ESP.getEfuseMac() & 0xFFFFFFu))
#define ESPMAC (Sprintf("%06x", CHIPID))
#define Sprintf(f, ...) ({ char* s; asprintf(&s, f, __VA_ARGS__); const String r = s; free(s); r; })
#define Stdprintf(f, ...) ({ char* s; asprintf(&s, f, __VA_ARGS__); const std::string r = s; free(s); r; })

std::string slugify(const std::string& text);
String slugify(const String& text);
std::string kebabify(const std::string& text);
String kebabify(const String& text);
std::string hexStr(const uint8_t *data, int len);
std::string hexStr(const char *data, int len);
std::string hexStr(const std::string& s);
std::string hexStrRev(const uint8_t *data, int len);
std::string hexStrRev(const char *data, int len);
std::string hexStrRev(const std::string &s);
bool hextostr(const String &hexStr, uint8_t* output, size_t len);
bool prefixExists(const String &prefixes, const String &s);
bool spurt(const String &fn, const String &content);
