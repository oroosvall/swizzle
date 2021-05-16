#ifndef VULKAN_MEMORY_HPP
#define VULKAN_MEMORY_HPP

/* Include files */

#include "Vk.hpp"

#include "../VulkanInstance.hpp"
#include "../VulkanDevice.hpp"

#include "VkContainer.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace vk
{
    class VulkanMemory
    {
    public:

        VulkanMemory(const swizzle::gfx::VkContainer vkObjects);
        virtual ~VulkanMemory();

        void allocateMemory(VkMemoryPropertyFlags properties, VkMemoryRequirements memReqs);

        VkDeviceSize getMemorySize() const;
        VkDeviceMemory getMemoryHandle() const;

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
