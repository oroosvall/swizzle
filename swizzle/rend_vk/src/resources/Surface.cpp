
/* Include files */

#if defined(SW_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(SW_LINUX_XLIB)
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined(SW_LINUX_XCB)
#define VK_USE_PLATFORM_XCB_KHR
#endif

#include "Surface.hpp"
#include "Vk.hpp"

#include "../Log.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace rvk
{
    const char* GetOsSurfaceExtension()
    {
#if defined(SW_WINDOWS)
        return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#elif defined(SW_LINUX_XLIB)
        return VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
#elif defined(SW_LINUX_XCB)
        return VK_KHR_XCB_SURFACE_EXTENSION_NAME;
#else
#error "undfined platform"
#endif
    }

    const char* GetKHRSurfaceExtension()
    {
        return VK_KHR_SURFACE_EXTENSION_NAME;
    }

} // namespace rvk

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
