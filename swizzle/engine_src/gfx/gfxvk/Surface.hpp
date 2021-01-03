#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <swizzle/core/Resource.hpp>
#include <swizzle/core/Window.hpp>

#include "vk.hpp"

namespace swizzle::gfx
{
    const char* GetOsSurfaceExtension();

    VkSurfaceKHR CreateOsSurface(core::Resource<core::Window> window, VkInstance instance);

}

#endif