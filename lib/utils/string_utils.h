#pragma once
#include <Arduino.h>
#include <WString.h>
#include <string>
#include <cstring>

// CHIPID extracts a unique 24-bit value from the eFuse MAC for use in MQTT
// topic suffixes and Home Assistant entity IDs. We deliberately mask the low
// 24 bits (& 0xFFFFFFu) rather than shifting the high bits (>> 24), because
// the eFuse byte order varies between classic ESP32 and newer targets such
// as ESP32-C6 and ESP32-S3: on classic ESP32 the high byte holds a NIC
// octet, but on C6/S3 those high bytes are the OUI/constant bits, so >> 24
// would collapse many devices to the same identifier (see issue #2402).
// Masking the low 24 bits yields a stable unique value on every supported
// target while preserving the 6-hex-digit format used by ESPMAC / mqtt.cpp.
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
