
#pragma once

/* Include files */

#include "Vk.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace rvk
{
    struct DebugCallbacks
    {
        VkDebugReportCallbackEXT mDebugCallback;

        PFN_vkCreateDebugReportCallbackEXT mCreate;
        PFN_vkDestroyDebugReportCallbackEXT mDestroy;
    };
}

/* Class Declaration */

/* Function Declaration */

namespace rvk
{
    VkDebugReportCallbackCreateInfoEXT getDebugCallbacks();
} // namespace rvk
