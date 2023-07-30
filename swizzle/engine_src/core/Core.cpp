
#include "Core.hpp"
#include "PlatformLayer.hpp"
#include "WindowLayer.hpp"

namespace swizzle::core
{
    SwBool CoreInitialize(const SwChar* appName)
    {
        platform::platformInit(appName);
        plf::window::init();

        return true;
    }

    SwBool CoreCleanup()
    {
        platform::platformDeInit();
        plf::window::cleanup();

        return true;
    }

}