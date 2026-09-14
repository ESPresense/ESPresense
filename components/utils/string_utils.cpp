#include "string_utils.h"

#include <algorithm>
#include <cctype>

static constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

static std::string ltrim(const std::string& s, char toTrim) {
    size_t start = s.find_first_not_of(toTrim);
    return (start == std::string::npos) ? "" : s.substr(start);
}

static std::string rtrim(const std::string& s, char toTrim) {
    size_t end = s.find_last_not_of(toTrim);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

static std::string normalizeWordSeparators(const std::string& text, char replacement) {
    std::string out;
    out.reserve(text.size());
    bool lastWasReplacement = false;
    for (unsigned char c : text) {
        const bool isWordChar = std::isalnum(c) || c == '_';
        if (isWordChar) {
            out.push_back(static_cast<char>(std::tolower(c)));
            lastWasReplacement = false;
        } else if (!lastWasReplacement) {
            out.push_back(replacement);
            lastWasReplacement = true;
        }
    }
    return rtrim(ltrim(out, replacement), replacement);
}

std::string slugify(const std::string& text) { return normalizeWordSeparators(text, '_'); }
std::string kebabify(const std::string& text) { return normalizeWordSeparators(text, '-'); }

std::string hexStr(const uint8_t* data, int len) {
    std::string s(len * 2, ' ');
    for (int i = 0; i < len; ++i) {
        s[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[data[i] & 0x0F];
    }
    return s;
}
std::string hexStr(const char* data, int len) { return hexStr(reinterpret_cast<const uint8_t*>(data), len); }
std::string hexStr(const std::string& s) { return hexStr(s.c_str(), s.length()); }

std::string hexStrRev(const uint8_t* data, int len) {
    std::string s(len * 2, ' ');
    for (int i = 0; i < len; ++i) {
        s[len - (2 * i + 1)] = hexmap[(data[i] & 0xF0) >> 4];
        s[len - (2 * i + 2)] = hexmap[data[i] & 0x0F];
    }
    return s;
}
std::string hexStrRev(const char* data, int len) { return hexStrRev(reinterpret_cast<const uint8_t*>(data), len); }
std::string hexStrRev(const std::string& s) { return hexStrRev(s.c_str(), s.length()); }

static uint8_t hextob(char ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    return 0;
}

bool hextostr(const std::string& hexStr, uint8_t* output, size_t len) {
    if (hexStr.length() & 1) return false;
    if (hexStr.length() < len * 2) return false;
    int k = 0;
    for (size_t i = 0; i < len * 2; i += 2)
        output[k++] = (hextob(hexStr[i]) << 4) | hextob(hexStr[i + 1]);
    return true;
}

static std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    return s;
}

bool prefixExists(const std::string& prefixes, const std::string& s) {
    const std::string sLower = lower(s);
    size_t start = 0;
    while (start <= prefixes.size()) {
        size_t space = prefixes.find(' ', start);
        std::string sub = prefixes.substr(start, space == std::string::npos ? std::string::npos : space - start);
        if (!sub.empty() && sLower.compare(0, sub.size(), lower(sub)) == 0) return true;
        if (space == std::string::npos) break;
        start = space + 1;
    }
    return false;
}
