
#include "Core.hpp"
#include "platform/PlatformLayer.hpp"

namespace swizzle::core
{
    SwBool CoreInitialize(const SwChar* appName)
    {
        platform::platformInit(appName);

        return true;
    }

    SwBool CoreCleanup()
    {
        platform::platformDeInit();

        return true;
    }

}