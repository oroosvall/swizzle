#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <common/Common.hpp>

namespace swizzle::gfx
{
    class Texture
    {
    public:
        virtual ~Texture() {}

        virtual void setData(U32 width, U32 height, U32 channels, const U8* pixelData) = 0;
        virtual void getTextureSize(U32& w, U32& h) = 0;

        virtual void upload() = 0;
    };
}

#endif