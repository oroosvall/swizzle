#pragma once

/* Include files */

#include <swizzle/asset2/Assets.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::asset2
{
    class TextureAsset : public ITextureAsset
    {
    public:
        TextureAsset(U32 width, U32 height, U32 channels, U8* data);
        TextureAsset(U32 width, U32 height, U32 channels, common::Resource<IBuffer> buffer);
        virtual ~TextureAsset() = default;

        virtual U32 getWidth() const override;
        virtual U32 getHeight() const override;
        virtual U32 getChannels() const override;

        virtual common::Resource<IBuffer> getData() const override;

    private:
        U32 mWidth;
        U32 mHeight;
        U32 mChannels;
        common::Resource<IBuffer> mDataBuffer;
    };
} // namespace swizzle::asset2

/* Function Declaration */
