
/* Include files */

#include <swizzle/asset/TextureLoader.hpp>

#include <stb/stb_image.h>

#include <swizzle/core/Logging.hpp>

#include <cstring>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::asset
{
    common::Resource<gfx::Texture>  LoadTexture2D(common::Resource<gfx::GfxContext> ctx, const SwChar* file)
    {
        common::Resource<gfx::Texture> texture = nullptr;
        S32 x = 0;
        S32 y = 0;
        S32 c = 0;

        U8* textureData = stbi_load(file, &x, &y, &c, 4);
        if (textureData)
        {
            texture = ctx->createTexture(x, y, 4, false, textureData);
        }
        else
        {
            texture = ctx->createTexture(1,1,1, false, nullptr);
            LOG_ERROR("Texture %s failed to load", file);
        }

        stbi_image_free(textureData);

        return texture;
    }

    common::Resource<gfx::Texture> SWIZZLE_API LoadTextureCubeMap(common::Resource<gfx::GfxContext> ctx,
        const SwChar* rightFile, const SwChar* leftFile, const SwChar* topFile, const SwChar* bottomFile, const SwChar* frontFile, const SwChar* backFile)
    {
        common::Resource<gfx::Texture> texture = nullptr;
        S32 x = 0;
        S32 y = 0;
        S32 c = 0;

        // TODO verify all images are same size
        U8* textureDataRight = stbi_load(rightFile, &x, &y, &c, 4);
        U8* textureDataLeft = stbi_load(leftFile, &x, &y, &c, 4);
        U8* textureDataFront = stbi_load(frontFile, &x, &y, &c, 4);
        U8* textureDataBack = stbi_load(backFile, &x, &y, &c, 4);
        U8* textureDataTop = stbi_load(topFile, &x, &y, &c, 4);
        U8* textureDataBottom = stbi_load(bottomFile, &x, &y, &c, 4);

        U64 singleImageSize = (U64)(x * y * c);
        U8* combinedData = new U8[singleImageSize * 6u];

        memset(combinedData, 0, singleImageSize * 6u);

        memcpy(&combinedData[0 * singleImageSize], textureDataRight, singleImageSize);
        memcpy(&combinedData[1 * singleImageSize], textureDataLeft, singleImageSize);
        memcpy(&combinedData[2 * singleImageSize], textureDataTop, singleImageSize);
        memcpy(&combinedData[3 * singleImageSize], textureDataBottom, singleImageSize);
        memcpy(&combinedData[4 * singleImageSize], textureDataFront, singleImageSize);
        memcpy(&combinedData[5 * singleImageSize], textureDataBack, singleImageSize);

        texture = ctx->createCubeMapTexture(x, y, c, combinedData);

        delete[] combinedData;

        stbi_image_free(textureDataRight);
        stbi_image_free(textureDataLeft);
        stbi_image_free(textureDataTop);
        stbi_image_free(textureDataBottom);
        stbi_image_free(textureDataFront);
        stbi_image_free(textureDataBack);

        return texture;
    }

}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
