#include <regex>
#include <string_utils.h>
#include <SPIFFS.h>

static constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

std::string ltrim(const std::string &s, char toTrim)
{
    size_t start = s.find_first_not_of(toTrim);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s, char toTrim)
{
    size_t end = s.find_last_not_of(toTrim);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string lowertrim(std::string str, char toTrim)
{
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c)
                   { return std::tolower(c); });
    return rtrim(ltrim(str, toTrim), toTrim);
}

std::regex whitespace_re("[\\s\\W-]+");
std::string slugify(const std::string &text)
{
    return lowertrim(std::regex_replace(text, whitespace_re, "_"), '_');
}

String slugify(const String &text)
{
    std::string s = std::string(text.c_str());
    return slugify(s).c_str();
}

std::string kebabify(const std::string &text)
{
    return lowertrim(std::regex_replace(text, whitespace_re, "-"), '-');
}

String kebabify(const String &text)
{
    std::string s = std::string(text.c_str());
    return kebabify(s).c_str();
}

std::string hexStr(const uint8_t *data, int len) {
    std::string s(len * 2, ' ');
    for (int i = 0; i < len; ++i) {
        s[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[data[i] & 0x0F];
    }
    return s;
}

std::string hexStr(const char *data, unsigned int len)
{
    std::string s(len * 2, ' ');
    for (int i = 0; i < len; ++i)
    {
        s[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[data[i] & 0x0F];
    }
    return s;
}

std::string hexStr(const std::string &s)
{
    return hexStr(s.c_str(), s.length());
}

std::string hexStr(const uint8_t *&s, unsigned int len)
{
    return hexStr(reinterpret_cast<const char *>(s), len);
}

std::string hexStrRev(const char *data, unsigned int len)
{
    std::string s(len * 2, ' ');
    for (int i = 0; i < len; ++i)
    {
        s[len - (2 * i + 1)] = hexmap[(data[i] & 0xF0) >> 4];
        s[len - (2 * i + 2)] = hexmap[data[i] & 0x0F];
    }
    return s;
}

std::string hexStrRev(const uint8_t *&s, unsigned int len)
{
    return hexStrRev(reinterpret_cast<const char *>(s), len);
}

std::string hexStrRev(const std::string &s)
{
    return hexStrRev(s.c_str(), s.length());
}

uint8_t hextob(char ch)
{
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    return 0;
}

bool hextostr(const String &hexStr, uint8_t* output, size_t len)
{
    if (len & 1) return false;
    if (hexStr.length() < len*2) return false;
    int k = 0;
    for (size_t i = 0; i < len*2; i+=2)
        output[k++] = (hextob(hexStr[i]) << 4) | hextob(hexStr[i+1]);
    return true;
}

bool prefixExists(const String &prefixes, const String &s)
{
    unsigned int start = 0;
    unsigned int space;

    while ((space = prefixes.indexOf(" ", start)) != -1)
    {
        if (space > start)
        {
            auto sub = prefixes.substring(start, space);
            if (s.indexOf(sub) == 0) return true;
        }
        start = space + 1;
    }
    auto sub = prefixes.substring(start);
    return !sub.isEmpty() && s.indexOf(sub) == 0;
}

bool spurt(const String &fn, const String &content)
{
    File f = SPIFFS.open(fn, "w");
    if (!f) return false;
    auto w = f.print(content);
    f.close();
    return w == content.length();
}
