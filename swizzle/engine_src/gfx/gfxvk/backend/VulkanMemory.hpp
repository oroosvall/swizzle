#pragma once

#include "Vk.hpp"
#include "VulkanObjectContainer.hpp"

namespace vk
{
    class VulkanMemory
    {
    public:

        VulkanMemory(const swizzle::gfx::VulkanObjectContainer& vkObjects);
        virtual ~VulkanMemory();

        void allocateMemory(VkMemoryPropertyFlags properties, VkMemoryRequirements memReqs);

        VkDeviceSize getMemorySize();
        VkDeviceMemory getMemoryHandle();

        void* mapMemory();
        void unmapMemory();

        static U32 FindMemoryType(VkPhysicalDeviceMemoryProperties props, VkMemoryPropertyFlags properties, uint32_t typeBits);

    private:

        const swizzle::gfx::VulkanObjectContainer& mVkObjects;

        VkDeviceMemory mMemory;
        VkDeviceSize mMemorySize;
    };
}