#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <common/Resource.hpp>
#include <swizzle/core/Window.hpp>

#include "backend/Vk.hpp"

namespace swizzle::gfx
{
    const char* GetOsSurfaceExtension();

    VkSurfaceKHR CreateOsSurface(core::Resource<core::Window> window, VkInstance instance);

}

#endif