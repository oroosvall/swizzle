#ifndef COLOR_HPP
#define COLOR_HPP

#include <swizzle/core/common/Types.hpp>
#include <swizzle/Api.hpp>

namespace swizzle::gfx
{

    struct RGBA
    {
        SwFloat r, g, b, a;
    };

    struct HSVA
    {
        SwFloat h, s, v, a;
    };

    RGBA SWIZZLE_API HsvaToRgba(HSVA hsva);
    HSVA SWIZZLE_API RgbaToHsva(RGBA rgba);
}

#endif