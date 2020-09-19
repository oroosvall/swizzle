#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <swizzle/core/common/Resource.hpp>
#include <swizzle/core/platform/Window.hpp>

#include "vk.hpp"

namespace swizzle::gfx
{
    const char* GetOsSurfaceExtension();

    VkSurfaceKHR CreateOsSurface(core::Resource<core::Window> window, VkInstance instance);

}

#endif