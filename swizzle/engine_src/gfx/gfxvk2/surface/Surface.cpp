#if defined(SW_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined (SW_LINUX_XLIB)
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined (SW_LINUX_XCB)
#define VK_USE_PLATFORM_XCB_KHR
#endif

#include "Surface.hpp"
#include <swizzle/core/Logging.hpp>

#include "../backend/Vk.hpp"

namespace vk
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

    VkSurfaceKHR CreateOsSurface(common::Resource<swizzle::core::SwWindow> window, VkInstance instance)
    {
#if defined(SW_WINDOWS)
        VkSurfaceKHR surface = VK_NULL_HANDLE;

        VkWin32SurfaceCreateInfoKHR createInfo;
        createInfo.hinstance = GetModuleHandle(NULL);
        createInfo.hwnd = (HWND)window->getNativeWindowHandle();
        createInfo.pNext = nullptr;
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.flags = 0;

        VkResult res = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface);

        if (res != VK_SUCCESS)
        {
            LOG_ERROR("vkCreateWin32SurfaceKHR failed!, returned %s\n", vk::VkResultToString(res));
        }
        return surface;
#elif defined(SW_LINUX_XLIB)

        VkXlibSurfaceCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.window = (Window)window->getNativeWindowHandle();
        createInfo.dpy = (Display*)window->getNativeDisplayHandle();

        VkSurfaceKHR surface = VK_NULL_HANDLE;
        vkCreateXlibSurfaceKHR(instance, &createInfo, nullptr, &surface);

        return surface;
#elif defined(SW_LINUX_XCB)

        VkXcbSurfaceCreateInfoKHR  createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        U64 wnd = (U64)window->getNativeWindowHandle();
        createInfo.window = (xcb_window_t)wnd;
        createInfo.connection = (xcb_connection_t*)window->getNativeDisplayHandle();

        VkSurfaceKHR surface = VK_NULL_HANDLE;
        vkCreateXcbSurfaceKHR(instance, &createInfo, nullptr, &surface);

        return surface;
#else
#error "undfined platform"
#endif
    }

}
