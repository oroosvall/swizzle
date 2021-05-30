#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <common/Common.hpp>
#include <swizzle/core/Window.hpp>

#include "../backend/Vk.hpp"

namespace swizzle::gfx
{
    const char* GetOsSurfaceExtension();

    VkSurfaceKHR CreateOsSurface(common::Resource<core::SwWindow> window, VkInstance instance);

}

#endif