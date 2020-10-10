#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <swizzle/core/common/Types.hpp>

namespace swizzle::gfx
{
    class Texture
    {
    public:
        virtual ~Texture() {}

        virtual void setData(U32 width, U32 height, U32 channels, U8* pixelData) = 0;

    };
}

#endif