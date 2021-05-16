#ifndef VK_CONTAINER_HPP
#define VK_CONTAINER_HPP

/* Include files */

#include <common/Types.hpp>
#include <common/Resource.hpp>

#include "Vk.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

class VulkanInstance;
class PhysicalDevice;
class LogicalDevice;

/* Struct Declaration */

namespace swizzle::gfx // move to vk namespace
{
    // this basically contains all vulkan objects created for the pipeline
    // that get passed around between objects
    // so it is handy to access
    struct VkContainer
    {
        core::Resource<VulkanInstance> mVkInstance;
        core::Resource<PhysicalDevice> mPhysicalDevice;
        core::Resource<LogicalDevice> mLogicalDevice;

        VkAllocationCallbacks* mDebugAllocCallbacks;

        VkDescriptorPool mDescriptorPool; // TODO: Fix me
    };
}

/* Class Declaration */

/* Function Declaration */


#endif