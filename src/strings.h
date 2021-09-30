#ifndef STRINGS_h
#define STRINGS_h
#include <regex>

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
#endif
