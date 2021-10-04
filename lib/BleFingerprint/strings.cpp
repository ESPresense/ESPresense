#include <regex>
#include <strings.h>

std::string lowertrim(std::string str, char c)
{
    int atstart = 0, atend = str.size() - 1;
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] != c)
        {
            atstart = i;
            break;
        }
    }
    for (int i = str.size() - 1; i >= 0; i--)
    {
        if (str[i] != c)
        {
            atend = i;
            break;
        }
    }

    char diff = 'a' - 'A';
    std::string fistr = "";
    for (int i = atstart; i <= atend; i++)
    {
        if ((str[i] >= 'A') && (str[i] <= 'Z'))
            fistr += (str[i] + diff);
        else
            fistr += str[i];
    }

    return fistr;
}

std::regex whitespace_re("[\\s\\W-]+");
std::string slugify(std::string text)
{
    return lowertrim(std::regex_replace(text, whitespace_re, "_"), '_');
}

String slugify(String text)
{
    std::string s = std::string(text.c_str());
    return slugify(s).c_str();
}

std::string kebabify(std::string text)
{
    return lowertrim(std::regex_replace(text, whitespace_re, "-"), '-');
}

String kebabify(String text)
{
    std::string s = std::string(text.c_str());
    return kebabify(s).c_str();
}

std::string hexStr(const char *data, int len)
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

std::string hexStr(std::string s)
{
    return hexStr(s.c_str(), s.length());
}
