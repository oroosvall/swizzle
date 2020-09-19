#include <swizzle/Swizzle.hpp>

#include "core/Core.hpp"
#include "gfx/Gfx.hpp"

namespace swizzle
{
    SwBool SwInitialize(const SwChar* appName)
    {
        appName;
        LOG_INFO("Initializing Swizzle Engine");
        SwBool status = core::CoreInitialize(appName);
        status &= gfx::GfxInitialize();
        
        return status;
    }

    SwBool SwCleanup()
    {
        LOG_INFO("Shutting down");
        SwBool status = gfx::GfxCleanup(); 
        status &= core::CoreCleanup();

        return status;
    }
}