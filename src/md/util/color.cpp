#include <vector>
#include <iostream>
#include <sstream>

#include <md/util/color.hpp>

namespace clientmap {
     
int nk_parse_hex(const char *p, int length)
{
    int i = 0;
    int len = 0;
    while (len < length) 
    {
        i <<= 4;

        if (p[len] >= 'a' && p[len] <= 'f')
            i += ((p[len] - 'a') + 10);
        else if (p[len] >= 'A' && p[len] <= 'F')
            i += ((p[len] - 'A') + 10);
        else 
            i += (p[len] - '0');

        len++;
    }
    return i;
}

int ColorI::parseHexRGB(const char *rgb, ColorI& col)
{
    col = {};
    const char *c = rgb;
    if (*c == '#'){
        c++;
    }
    else {
        return -1;
    }
    col.r = (uint8_t)nk_parse_hex(c, 2);
    col.g = (uint8_t)nk_parse_hex(c + 2, 2);
    col.b = (uint8_t)nk_parse_hex(c + 4, 2);
    col.a = 255;
    return 0;
}

std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

template <typename T>
uint8_t clamp_css_byte(T i) 
{   
    // Clamp to integer 0 .. 255.
    i = ::round(i);  // Seems to be what Chrome does (vs truncation).
    return i < 0 ? 0 : i > 255 ? 255 : uint8_t(i);
}

int64_t parseInt(const std::string& str, uint8_t base = 10) 
{
    return strtoll(str.c_str(), nullptr, base);
}

uint8_t parse_css_int(const std::string& str) {  // int or percentage.
    return clamp_css_byte(parseInt(str));
}

int ColorI::parse255RGB(const std::string& str, ColorI& aResult)
{
    const std::vector<std::string> params = split(str, ',');

    float alpha = 1.0f;
    if (params.size() != 3) {
        return {};
    }

    aResult = { parse_css_int(params[0]),
        parse_css_int(params[1]),
        parse_css_int(params[2]) ,
        255
    };
    return 0;
}

} //  namespace clientmap
