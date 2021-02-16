#pragma once

#include "Vk.hpp"

namespace vk
{
    VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
        uint64_t sourceObj, size_t location, int32_t msgCode,
        const char* layerPrefix, const char* msg, void* userData);

    VkAllocationCallbacks* getDebugAllocCallback();

    struct VkStatisics
    {
        U64 mFrameCounter;
        U32 mDrawCalls;
    };

}