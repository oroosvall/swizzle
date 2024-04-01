
#pragma once

/* Include files */

#if defined(SW_WINDOWS)
#pragma warning(push)
#pragma warning(disable : 26495)
#pragma warning(disable : 26812)
#endif
#include <vulkan/vulkan.h>
#if defined(SW_WINDOWS)
#pragma warning(pop)
#endif

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace rvk
{
    const char* VkResultToString(VkResult res);

    void LogVulkanError(VkResult res, const char* functionName);
}
