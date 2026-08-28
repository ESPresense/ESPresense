#pragma once
#include <cstdint>
#include <cstring>
#include <string>

std::string slugify(const std::string& text);
std::string kebabify(const std::string& text);
std::string hexStr(const uint8_t* data, int len);
std::string hexStr(const char* data, int len);
std::string hexStr(const std::string& s);
std::string hexStrRev(const uint8_t* data, int len);
std::string hexStrRev(const char* data, int len);
std::string hexStrRev(const std::string& s);
bool hextostr(const std::string& hexStr, uint8_t* output, size_t len);
bool prefixExists(const std::string& prefixes, const std::string& s);
