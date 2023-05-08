
/* Include files */

#include <swizzle/Swizzle.hpp>

#include "core/Core.hpp"
#include "gfx/Gfx.hpp"
#include "sound/Sound.hpp"

#include "utils/HighResolutionClock.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swizzle
{
    SwBool SwInitialize(const SwChar* appName)
    {
        LOG_INFO("Initializing Swizzle Engine");

        SwBool status = core::CoreInitialize(appName);
        status &= gfx::GfxInitialize();
        status &= sound::SoundInitialize();

        return status;
    }

    SwBool SwCleanup()
    {
        LOG_INFO("Shutting down");
        SwBool status = gfx::GfxCleanup();
        status &= core::CoreCleanup();
        status &= sound::SoundCleanup();

        return status;
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
