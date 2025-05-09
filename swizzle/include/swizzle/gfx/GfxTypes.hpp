
#pragma once

/* Include files */

#include <common/Common.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct/Class Declaration */

namespace swizzle::gfx
{
    /// <summary>
    /// Represents texture dimensions
    /// </summary>
    struct TextureDimensions
    {
        U32 mWidth;
        U32 mHeight;
        U32 mLayers;
        U32 mMipLevels;
    };

    /// <summary>
    /// Preferred location to store a reasource
    /// </summary>
    enum class GfxMemoryArea
    {
        DeviceLocalHostVisible,
        DeviceLocal,
        Host
    };
} // namespace swizzle::gfx

/* Function Declaration */
