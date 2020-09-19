#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <swizzle/core/common/Types.hpp>

namespace swizzle::gfx
{
    class Texture
    {
    public:
        virtual ~Texture() {}

        virtual void setData(SwU32 width, SwU32 height, SwU32 channels, SwU8* pixelData) = 0;

    };
}

#endif