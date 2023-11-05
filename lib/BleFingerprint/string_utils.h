#pragma once
#include <Arduino.h>
#include <WString.h>
#include <string>
#include <cstring>

#define CHIPID (uint32_t)(ESP.getEfuseMac() >> 24)
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
