#pragma once
#include <Arduino.h>
#include <WString.h>
#include <string>
#include <cstring>

#define CHIPID ((unsigned int)(ESP.getEfuseMac() >> 24))
#define ESPMAC (Sprintf("%06x", CHIPID))
// asprintf mallocs *s and returns -1 on OOM leaving *s indeterminate. The old form read an
// uninitialized s (String/std::string from a garbage pointer -> strlen/copy fault) and free()'d
// it -> the #2309 S3 Guru (LoadProhibited / std::length_error) once the heap was exhausted.
// Guard: only touch/free s when asprintf succeeded; otherwise yield an empty string.
#define Sprintf(f, ...) ({ char* s = nullptr; String r; if (asprintf(&s, f, __VA_ARGS__) >= 0 && s) { r = s; free(s); } r; })
#define Stdprintf(f, ...) ({ char* s = nullptr; std::string r; if (asprintf(&s, f, __VA_ARGS__) >= 0 && s) { r = s; free(s); } r; })

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
