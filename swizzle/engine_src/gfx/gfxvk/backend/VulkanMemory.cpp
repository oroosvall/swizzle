
#include "VulkanMemory.hpp"

#include "../VulkanInstance.hpp"
#include "../VulkanPhysicalDevice.hpp"

namespace vk
{

    VulkanMemory2::VulkanMemory2(const swizzle::gfx::VkContainer vkObjects)
        : mVkObjects(vkObjects)
        , mMemory(VK_NULL_HANDLE)
        , mMemorySize(0U)
    {
    }

    VulkanMemory2::~VulkanMemory2()
    {
        vkFreeMemory(mVkObjects.mLogicalDevice->getLogical(), mMemory, mVkObjects.mDebugAllocCallbacks);
        mMemory = VK_NULL_HANDLE;
        mMemorySize = 0U;
    }

    void VulkanMemory2::allocateMemory(VkMemoryPropertyFlags properties, VkMemoryRequirements memReqs)
    {
        auto log = mVkObjects.mLogicalDevice.get();
        auto phys = mVkObjects.mPhysicalDevice.get();

        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = VK_NULL_HANDLE;
        allocInfo.allocationSize = memReqs.size;
        allocInfo.memoryTypeIndex = FindMemoryType(phys->getMemoryProperties(), properties, memReqs.memoryTypeBits);

        vkAllocateMemory(log->getLogical(), &allocInfo, mVkObjects.mDebugAllocCallbacks, &memory);
        mMemorySize = memReqs.size;

        if (mMemory)
        {
            vkFreeMemory(log->getLogical(), mMemory, mVkObjects.mDebugAllocCallbacks);
            mMemory = VK_NULL_HANDLE;
        }

        if (memory == VK_NULL_HANDLE)
        {
            mMemorySize = 0U;
        }

        mMemory = memory;
    }

    VkDeviceSize VulkanMemory2::getMemorySize()
    {
        return mMemorySize;
    }

    VkDeviceMemory VulkanMemory2::getMemoryHandle()
    {
        return mMemory;
    }

    void* VulkanMemory2::mapMemory()
    {
        void* ptr = nullptr;
        vkMapMemory(mVkObjects.mLogicalDevice->getLogical(), mMemory, 0U, mMemorySize, 0, &ptr);
        return ptr;
    }

    void VulkanMemory2::unmapMemory()
    {
        vkUnmapMemory(mVkObjects.mLogicalDevice->getLogical(), mMemory);
    }

    U32 VulkanMemory2::FindMemoryType(VkPhysicalDeviceMemoryProperties props, VkMemoryPropertyFlags properties, uint32_t typeBits)
    {
        U32 result = 0xFFFFFFFF; // Unable to find memoryType

        for (U32 i = 0U; i < props.memoryTypeCount; i++)
        {
            if (((props.memoryTypes[i].propertyFlags & properties) == properties) && (typeBits & (1U << i)))
            {
                result = i;
                break;
            }
        }
        return result;
    }
}