
#pragma once

/* Include files */

#include <common/Common.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle
{
    class IBuffer
    {
    public:
        virtual ~IBuffer() {}

        virtual U64 size() const = 0;
        virtual U8* data() const = 0;
    };
}

/* Function Declaration */
