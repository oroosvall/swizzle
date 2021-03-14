#ifndef VULKAN_MEMORY_HPP
#define VULKAN_MEMORY_HPP

/* Include files */

#include "Vk.hpp"

#include "../VulkanInstance.hpp"
#include "../VulkanPhysicalDevice.hpp"

#include "VkContainer.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace vk
{
    class VulkanMemory2
    {
    public:

        VulkanMemory2(const swizzle::gfx::VkContainer vkObjects);
        virtual ~VulkanMemory2();

        void allocateMemory(VkMemoryPropertyFlags properties, VkMemoryRequirements memReqs);

        VkDeviceSize getMemorySize();
        VkDeviceMemory getMemoryHandle();

        void* mapMemory();
        void unmapMemory();

        static U32 FindMemoryType(VkPhysicalDeviceMemoryProperties props, VkMemoryPropertyFlags properties, uint32_t typeBits);

    private:

        const swizzle::gfx::VkContainer mVkObjects;

        VkDeviceMemory mMemory;
        VkDeviceSize mMemorySize;
    };
}

/* Function Declaration */

#endif
