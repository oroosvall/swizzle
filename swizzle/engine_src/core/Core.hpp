#ifndef CORE_HPP
#define CORE_HPP

#include <common/Common.hpp>

namespace swizzle::core
{
    SwBool CoreInitialize(const SwChar* appName);
    SwBool CoreCleanup();

}

#endif