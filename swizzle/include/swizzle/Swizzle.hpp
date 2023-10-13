#ifndef SWIZZLE_HPP
#define SWIZZLE_HPP

/* Includes */

#include <common/Common.hpp>
#include "swizzle/Api.hpp"

/* Defines */

/* Typedefs */

/* Function Declaration */

namespace swizzle
{
    const SwChar SWIZZLE_API *VersionString();
    SwBool SWIZZLE_API SwInitialize(const SwChar* appName = nullptr);
    SwBool SWIZZLE_API SwCleanup();
}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace sw = swizzle;
#endif
