#pragma once
#include <cstdint>
#include <string>

namespace clientmap {

// We have two structs to represent color:
struct ColorI;
struct ColorF;

// ColorI represents a 32-bit color, with 8 bits per color channel for 'r', 'g',
// 'b', and 'a'. The color can also be accessed as a single 32-bit integer with
// all channels packed together as 'abgr'. A ColorI can be converted to a ColorF
// without losing precision.
struct ColorI {

    // This union allows the color components to be accessed either as separate
    // bytes or as an integer combining the four bytes. On a big-endian system
    // the integer will combine the components in 'rgba' order instead.
    union {
        struct {
            uint8_t r, g, b, a;
        };
        uint32_t abgr = 0;
    };

    ColorI() = default;
    ColorI(uint32_t _abgr) : abgr(_abgr)
    {}
    ColorI(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a=255)
        : r(_r), g(_g), b(_b), a(_a) 
    {}

    friend bool operator==(const ColorI& L,const ColorI& other) {
        return L.abgr == other.abgr;
    }

    inline ColorF toColorF() const;

    ColorI withAlpha(float alpha) {
        ColorI result(abgr);
        result.a = static_cast<uint8_t>(alpha * 255.f);
        return result;
    }

    static ColorI mix(const ColorI& _x, const ColorI& _y, float _a) {
        return ColorI(
            _x.r * (1 - _a) + _y.r * _a,
            _x.g * (1 - _a) + _y.g * _a,
            _x.b * (1 - _a) + _y.b * _a,
            _x.a * (1 - _a) + _y.a * _a
        );
    }

    static int parse255RGB(const std::string& str, ColorI& aResult);
    static int parseHexRGB(const char *rgb, ColorI& col);
};

// ColorF represents a color with floating point channel values for 'r', 'g',
// 'b', and 'a'. A ColorF can be converted to a ColorI but may lose precision.
struct ColorF {

    float r = 0, g = 0, b = 0, a = 0;

    ColorF() = default;
    ColorF(float _r, float _g, float _b, float _a=1.0)
        : r(_r), g(_g), b(_b), a(_a) 
    {}

    friend bool operator==(const ColorF& L,const ColorF& other)
    {
        return L.r == other.r && 
            (L.g == other.g &&
               (L.b == other.b && L.a == other.a)
             );
    }

    inline ColorI toColor()const;

    static ColorF mix(const ColorF& _x, 
                     const ColorF& _y, 
                     float _a) 
    {
        return ColorF(
            _x.r * (1 - _a) + _y.r * _a,
            _x.g * (1 - _a) + _y.g * _a,
            _x.b * (1 - _a) + _y.b * _a,
            _x.a * (1 - _a) + _y.a * _a
        );
    }

};

ColorF ColorI::toColorF()const
{
    return ColorF(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

ColorI ColorF::toColor() const
{
    return ColorI(255 * r, 255 * g, 255 * b, 255 * a);
}

} // namespace Tangram
