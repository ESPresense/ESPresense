#ifndef STRINGS_h
#define STRINGS_h

#include <Arduino.h>

std::string slugify(const std::string& text);
String slugify(const String& text);
std::string kebabify(const std::string& text);
String kebabify(const String& text);
std::string hexStr(const char *data, int len);
std::string hexStr(const std::string& s);
#endif
