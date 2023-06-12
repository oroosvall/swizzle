
/* Include files */

#include "TextureAsset.hpp"
#include "core/BufferImpl.hpp"

#include <stb/stb_image.h>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::asset2
{
    common::Resource<ITextureAsset> LoadTexture(const SwChar* fileName)
    {
        S32 x = 0;
        S32 y = 0;
        S32 c = 0;

        common::Resource<TextureAsset> result = nullptr;

        U8* textureData = stbi_load(fileName, &x, &y, &c, 4);
        if (textureData)
        {
            result = common::CreateRef<TextureAsset>(U32(x), U32(y), U32(c), textureData);
        }
        stbi_image_free(textureData);

        return result;
    }
} // namespace swizzle::asset2

/* Class Public Function Definition */

namespace swizzle::asset2
{
    TextureAsset::TextureAsset(U32 width, U32 height, U32 channels, U8* data)
        : mWidth(width)
        , mHeight(height)
        , mChannels(channels)
        , mDataBuffer(nullptr)
    {
        U64 size = U64(mWidth) * U64(mHeight) * U64(mChannels);
        mDataBuffer = common::CreateRef<BufferImpl>(size);
        memcpy(mDataBuffer->data(), data, size);
    }

    U32 TextureAsset::getWidth() const
    {
        return mWidth;
    }

    U32 TextureAsset::getHeight() const
    {
        return mHeight;
    }

    U32 TextureAsset::getChannels() const
    {
        return mChannels;
    }

    common::Resource<IBuffer> TextureAsset::getData() const
    {
        return mDataBuffer;
    }

} // namespace swizzle::asset2

/* Class Protected Function Definition */

/* Class Private Function Definition */
