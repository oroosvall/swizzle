#ifndef TEXTURE_LOADER
#define TEXTURE_LOADER

/* Include files */

#include <swizzle/Api.hpp>
#include <common/Common.hpp>
#include <swizzle/gfx/Context.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace swizzle::asset
{
    common::Resource<gfx::Texture> SWIZZLE_API LoadTexture2D(common::Resource<gfx::GfxContext> ctx, const SwChar* file);

    common::Resource<gfx::Texture> SWIZZLE_API LoadTextureCubeMap(common::Resource<gfx::GfxContext> ctx,
        const SwChar* rightFile, const SwChar* leftFile,const SwChar* topFile, const SwChar* bottomFile, const SwChar* frontFile, const SwChar* backFile);

}

#endif