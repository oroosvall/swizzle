#ifndef VULKAN_PLATFORM_HPP
#define VULKAN_PLATFORM_HPP

#include <swizzle/Window.hpp>
#include "vk.hpp"

namespace swizzle
{

    const char* getOsSurfaceExtension();

    VkSurfaceKHR createOsSurface(Window* window, VkInstance instance);

} // namespace renderer

#endif