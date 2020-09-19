#ifndef VK_HPP
#define VK_HPP

#include <swizzle/core/common/Types.hpp>

#if defined(SW_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#pragma warning(push)
#pragma warning(disable : 26495)
#pragma warning(disable : 26812)
#include <vulkan/vulkan.h>
#pragma warning(pop)

const char* VkResultToString(VkResult res);

SwU32 FindMemoryType(VkPhysicalDeviceMemoryProperties props, VkMemoryPropertyFlags properties, uint32_t type_bits);

#endif