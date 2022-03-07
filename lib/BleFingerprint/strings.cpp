#include <regex>
#include <strings.h>

std::string lowertrim(std::string str, char c)
{
    unsigned int atstart = 0, atend = str.size() - 1;
    for (unsigned int i = 0; i < str.size(); i++)
    {
        if (str[i] != c)
        {
            atstart = i;
            break;
        }
    }
    for (unsigned int i = str.size() - 1; i >= 0; i--)
    {
        if (str[i] != c)
        {
            atend = i;
            break;
        }
    }

    char diff = 'a' - 'A';
    std::string fistr;
    for (unsigned int i = atstart; i <= atend; i++)
    {
        if ((str[i] >= 'A') && (str[i] <= 'Z'))
            fistr += char(str[i] + diff);
        else
            fistr += str[i];
    }

    return fistr;
}

std::regex whitespace_re("[\\s\\W-]+");
std::string slugify(const std::string& text)
{
    return lowertrim(std::regex_replace(text, whitespace_re, "_"), '_');
}

String slugify(const String& text)
{
    std::string s = std::string(text.c_str());
    return slugify(s).c_str();
}

std::string kebabify(const std::string& text)
{
    return lowertrim(std::regex_replace(text, whitespace_re, "-"), '-');
}

String kebabify(const String& text)
{
    std::string s = std::string(text.c_str());
    return kebabify(s).c_str();
}

std::string hexStr(const char *data, unsigned int len)
{
    constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    std::string s(len * 2, ' ');
    for (int i = 0; i < len; ++i)
    {
        s[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[data[i] & 0x0F];
    }
    return s;
}

std::string hexStr(const std::string& s)
{
    return hexStr(s.c_str(), s.length());
}

std::string hexStr(const uint8_t *&s, unsigned int len)
{
    return hexStr(reinterpret_cast<const char *>(s), len);
}

std::string hexStrRev(const char *data, unsigned int len)
{
    constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

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

bool prefixExists(const String& prefixes, const String& s)
{
    unsigned int start = 0;
    unsigned int space = 0;

    while ((space = prefixes.indexOf(" ", start)) != -1)
    {
        if (space > start)
        {
            auto sub = prefixes.substring(start, space);
            if (s.indexOf(sub) != -1) return true;
        }
        start = space + 1;
    }
    auto sub = prefixes.substring(start);
    return !sub.isEmpty() && s.indexOf(sub) != -1;
}
