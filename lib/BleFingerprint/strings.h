#ifndef STRINGS_h
#define STRINGS_h

#include <Arduino.h>

std::string slugify(std::string text);
String slugify(String text);
std::string kebabify(std::string text);
String kebabify(String text);
std::string hexStr(const char *data, int len);
std::string hexStr(std::string s);
#endif
