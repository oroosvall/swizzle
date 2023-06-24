
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
            result = common::CreateRef<TextureAsset>(U32(x), U32(y), 4u, textureData);
        }
        stbi_image_free(textureData);

        return result;
    }

    common::Resource<ITextureAsset> SWIZZLE_API LoadCubeTexture(const SwChar* rightFile, const SwChar* leftFile,
                                                                const SwChar* topFile, const SwChar* bottomFile,
                                                                const SwChar* frontFile, const SwChar* backFile)
    {
        S32 x = 0;
        S32 y = 0;
        S32 c = 0;

        U8* textureDataRight = stbi_load(rightFile, &x, &y, &c, 4);
        U8* textureDataLeft = stbi_load(leftFile, &x, &y, &c, 4);
        U8* textureDataTop = stbi_load(topFile, &x, &y, &c, 4);
        U8* textureDataBottom = stbi_load(bottomFile, &x, &y, &c, 4);
        U8* textureDataFront = stbi_load(frontFile, &x, &y, &c, 4);
        U8* textureDataBack = stbi_load(backFile, &x, &y, &c, 4);

        U64 singleImageSize = (U64)(x * y * c);
        auto mDataBuffer = common::CreateRef<BufferImpl>(singleImageSize * 6u);
        memset(mDataBuffer->data(), 0, singleImageSize * 6u);

        memcpy(&mDataBuffer->data()[0 * singleImageSize], textureDataRight, singleImageSize);
        memcpy(&mDataBuffer->data()[1 * singleImageSize], textureDataLeft, singleImageSize);
        memcpy(&mDataBuffer->data()[2 * singleImageSize], textureDataTop, singleImageSize);
        memcpy(&mDataBuffer->data()[3 * singleImageSize], textureDataBottom, singleImageSize);
        memcpy(&mDataBuffer->data()[4 * singleImageSize], textureDataFront, singleImageSize);
        memcpy(&mDataBuffer->data()[5 * singleImageSize], textureDataBack, singleImageSize);

        common::Resource<TextureAsset> result = common::CreateRef<TextureAsset>(U32(x), U32(y), U32(c), mDataBuffer);

        stbi_image_free(textureDataRight);
        stbi_image_free(textureDataLeft);
        stbi_image_free(textureDataTop);
        stbi_image_free(textureDataBottom);
        stbi_image_free(textureDataFront);
        stbi_image_free(textureDataBack);

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

    TextureAsset::TextureAsset(U32 width, U32 height, U32 channels, common::Resource<IBuffer> buffer)
        : mWidth(width)
        , mHeight(height)
        , mChannels(channels)
        , mDataBuffer(buffer)
    {
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
