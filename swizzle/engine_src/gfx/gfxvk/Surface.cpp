#include "Surface.hpp"

namespace swizzle::gfx
{
    const char* GetOsSurfaceExtension()
    {
#if defined(SW_WINDOWS)
        return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#else
#error "undfined platform"
#endif
    }

    VkSurfaceKHR CreateOsSurface(core::Resource<core::Window> window, VkInstance instance)
    {
#if defined(SW_WINDOWS)
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
#else
#error "undfined platform"
#endif
    }

}