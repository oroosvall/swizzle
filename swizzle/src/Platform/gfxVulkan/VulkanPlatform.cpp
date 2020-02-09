#include "VulkanPlatform.hpp"

#if defined(SW_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <cstdio>
#include <vulkan/vulkan.h>

namespace swizzle
{

#if defined(SW_WINDOWS)
    const char* getOsSurfaceExtension()
    {
        return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
    }

    VkSurfaceKHR createOsSurface(Window* window, VkInstance instance)
    {
        VkSurfaceKHR surface = VK_NULL_HANDLE;

        VkWin32SurfaceCreateInfoKHR createInfo;
        createInfo.hinstance = GetModuleHandle(NULL);
        createInfo.hwnd = (HWND)window->getNativeHandle();
        createInfo.pNext = nullptr;
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.flags = 0;

        VkResult res = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface);

        if (res != VK_SUCCESS)
        {
            printf("vkCreateWin32SurfaceKHR failed!, returned %d\n", static_cast<int32_t>(res));
        }
        return surface;
    }
#endif

} // namespace renderer