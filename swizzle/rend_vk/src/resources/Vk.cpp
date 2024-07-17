
/* Include files */

#include "Vk.hpp"
#include <map>

#include "../Log.hpp"

/* Defines */

#define RESULT_STR(a)                                                                                                  \
    {                                                                                                                  \
        a, #a                                                                                                          \
    }

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

namespace rvk
{
    static std::map<VkResult, const char*> vkResultStrings = {
        RESULT_STR(VK_SUCCESS),
        RESULT_STR(VK_NOT_READY),
        RESULT_STR(VK_TIMEOUT),
        RESULT_STR(VK_EVENT_SET),
        RESULT_STR(VK_EVENT_RESET),
        RESULT_STR(VK_INCOMPLETE),
        RESULT_STR(VK_ERROR_OUT_OF_HOST_MEMORY),
        RESULT_STR(VK_ERROR_OUT_OF_DEVICE_MEMORY),
        RESULT_STR(VK_ERROR_INITIALIZATION_FAILED),
        RESULT_STR(VK_ERROR_DEVICE_LOST),
        RESULT_STR(VK_ERROR_MEMORY_MAP_FAILED),
        RESULT_STR(VK_ERROR_LAYER_NOT_PRESENT),
        RESULT_STR(VK_ERROR_EXTENSION_NOT_PRESENT),
        RESULT_STR(VK_ERROR_FEATURE_NOT_PRESENT),
        RESULT_STR(VK_ERROR_INCOMPATIBLE_DRIVER),
        RESULT_STR(VK_ERROR_TOO_MANY_OBJECTS),
        RESULT_STR(VK_ERROR_FORMAT_NOT_SUPPORTED),
        RESULT_STR(VK_ERROR_FRAGMENTED_POOL),
        RESULT_STR(VK_ERROR_UNKNOWN),
        RESULT_STR(VK_ERROR_OUT_OF_POOL_MEMORY),
        RESULT_STR(VK_ERROR_INVALID_EXTERNAL_HANDLE),
        RESULT_STR(VK_ERROR_FRAGMENTATION),
        RESULT_STR(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS),
        RESULT_STR(VK_ERROR_SURFACE_LOST_KHR),
        RESULT_STR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR),
        RESULT_STR(VK_SUBOPTIMAL_KHR),
        RESULT_STR(VK_ERROR_OUT_OF_DATE_KHR),
        RESULT_STR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR),
        RESULT_STR(VK_ERROR_VALIDATION_FAILED_EXT),
        RESULT_STR(VK_ERROR_INVALID_SHADER_NV),
        RESULT_STR(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT),
        RESULT_STR(VK_ERROR_NOT_PERMITTED_EXT),
        RESULT_STR(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT),
        RESULT_STR(VK_THREAD_IDLE_KHR),
        RESULT_STR(VK_THREAD_DONE_KHR),
        RESULT_STR(VK_OPERATION_DEFERRED_KHR),
        RESULT_STR(VK_OPERATION_NOT_DEFERRED_KHR),
        RESULT_STR(VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT)};
}

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace rvk
{
    const char* VkResultToString(VkResult res)
    {
        return vkResultStrings[res];
    }

    void LogVulkanError(VkResult res, const char* functionName)
    {
        if (res != VK_SUCCESS)
        {
            LogMessage(LogLevel::Error, "%s failed!, returned %s\n", functionName, VkResultToString(res));
        }
    }
} // namespace rvk

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
