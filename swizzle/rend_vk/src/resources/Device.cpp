/* Include files */

#include "Device.hpp"
#include "Extension.hpp"
#include "DeviceMemory.hpp"
#include "Instance.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace rvk
{
    Device::Device(std::shared_ptr<Instance> instance,
                   VkPhysicalDevice physDev,
                   VkDevice logDev,
                   std::shared_ptr<Extensions> exts)
        : mInstance(instance)
        , mPhysicalDevice(physDev)
        , mLogicalDevice(logDev)
        , mExtensions(exts)
        , mMemoryProperties()
        , mMemoryPools()
    {
        vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mMemoryProperties);
    }

    Device::~Device()
    {
    }

    void Device::initAfterConstruction()
    {
        initMemoryPools();
    }

    void Device::waitIdle() const
    {
        VkResult res = vkDeviceWaitIdle(mLogicalDevice);
        LogVulkanError(res, "vkDeviceWaitIdle");
    }

    std::string Device::getDeviceName() const
    {
        VkPhysicalDeviceProperties props{};
        vkGetPhysicalDeviceProperties(mPhysicalDevice, &props);
        return props.deviceName;
    }

    std::shared_ptr<DeviceMemory> Device::allocateMemory(VkMemoryPropertyFlags propertyFlags, VkMemoryRequirements req)
    {
        uint32_t memoryTypeIndex = ~0u; // Unable to find memoryType
        uint32_t heapIndex       = ~0u;

        for (uint32_t i = 0u; i < mMemoryProperties.memoryTypeCount; i++)
        {
            uint32_t memProp = (mMemoryProperties.memoryTypes[i].propertyFlags & propertyFlags);
            if ((memProp == (uint32_t)propertyFlags) && (req.memoryTypeBits & (1U << i)))
            {
                memoryTypeIndex = i;
                heapIndex       = mMemoryProperties.memoryTypes[i].heapIndex;
                break;
            }
        }

        std::shared_ptr<DeviceMemory> mem = nullptr;

        if ((heapIndex != ~0u) && (memoryTypeIndex != ~0u))
        {
            mem = mMemoryPools[heapIndex]->allocateMemory(req, memoryTypeIndex);
        }

        return mem;
    }

    void Device::freeMemory(std::shared_ptr<DeviceMemory> memory)
    {
        std::shared_ptr<DeviceMemoryPool> pool = memory->mOwnerPool;
        pool->freeMemory(memory);
    }

    VkDevice Device::getHandle() const
    {
        return mLogicalDevice;
    }

    VkAllocationCallbacks* Device::getAllocCallbacks() const
    {
        return mInstance->getAllocCallbacks();
    }

} // namespace rvk

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace rvk
{
    void Device::initMemoryPools()
    {
        for (uint32_t i = 0u; i < mMemoryProperties.memoryHeapCount; ++i)
        {
            VkDeviceSize chunkSize = 1024llu * 1024llu * 4llu;
            std::string poolName = "Device Memory (GPU)";
            if (mMemoryProperties.memoryHeaps[i].flags != VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            {
                chunkSize = 1024llu * 1024llu * 16llu;
                poolName = "System Memory (CPU)";
            }

            for (uint32_t j = 0u; j < mMemoryProperties.memoryTypeCount; ++j)
            {
                if ((mMemoryProperties.memoryTypes[j].heapIndex == i) &&
                    (mMemoryProperties.memoryTypes[j].propertyFlags & VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
                {
                    poolName += " [Host Visible]";
                    break;
                }
            }

            mMemoryPools.push_back(std::make_shared<DeviceMemoryPool>(shared_from_this(), chunkSize,  poolName));
        }
    }
}