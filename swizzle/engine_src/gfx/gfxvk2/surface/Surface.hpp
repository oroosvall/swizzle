#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <common/Common.hpp>
#include <swizzle/core/Window.hpp>

#include "../backend/Vk.hpp"

namespace vk
{
    const char* GetOsSurfaceExtension();
    const char* GetKHRSurfaceExtension();

    VkSurfaceKHR CreateOsSurface(common::Resource<swizzle::core::SwWindow> window, VkInstance instance);

}

#endif