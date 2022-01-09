#ifndef VK2_HPP
#define VK2_HPP

#include <common/Common.hpp>

#if defined(SW_WINDOWS)
#pragma warning(push)
#pragma warning(disable : 26495)
#pragma warning(disable : 26812)
#endif
#include <vulkan/vulkan.h>
#if defined(SW_WINDOWS)
#pragma warning(pop)
#endif

#include <swizzle/core/Logging.hpp>

namespace vk
{
    const char* VkResultToString(VkResult res);

    void LogVulkanError(VkResult res, const char* functionName);

}

#endif