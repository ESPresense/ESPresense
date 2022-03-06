#ifndef STRINGS_h
#define STRINGS_h

#include <Arduino.h>

#define ESPMAC (Sprintf("%06" PRIx64, ESP.getEfuseMac() >> 24))
#define Sprintf(f, ...) ({ char* s; asprintf(&s, f, __VA_ARGS__); String r = s; free(s); r; })
#define Stdprintf(f, ...) ({ char* s; asprintf(&s, f, __VA_ARGS__); std::string r = s; free(s); r; })
#define SMacf(f) (                                                                                                                                       \
    {                                                                                                                                                    \
        auto nativeAddress = (f).getNative();                                                                                                            \
        Sprintf("%02x%02x%02x%02x%02x%02x", nativeAddress[5], nativeAddress[4], nativeAddress[3], nativeAddress[2], nativeAddress[1], nativeAddress[0]); \
    })

std::string slugify(const std::string& text);
String slugify(const String& text);
std::string kebabify(const std::string& text);
String kebabify(const String& text);
std::string hexStr(const char *data, int len);
std::string hexStr(const std::string& s);
bool prefixExists(const String& prefixes, const String& s);
#endif
