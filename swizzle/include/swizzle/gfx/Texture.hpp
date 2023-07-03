
#pragma once

/* Include files */

#include <common/Common.hpp>
#include <swizzle/gfx/GfxTypes.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct/Class Declaration */

namespace swizzle::gfx
{
    class Texture
    {
    public:
        virtual ~Texture() {}

        virtual TextureDimensions getSize() const = 0;

        virtual bool isDepth() = 0;
    };
} // namespace swizzle::gfx

/* Function Declaration */
