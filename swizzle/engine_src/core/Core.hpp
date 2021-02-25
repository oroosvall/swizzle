#ifndef CORE_HPP
#define CORE_HPP

#include <common/Types.hpp>

namespace swizzle::core
{
    SwBool CoreInitialize(const SwChar* appName);
    SwBool CoreCleanup();

}

#endif