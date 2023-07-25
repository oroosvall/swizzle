
/* Include files */

#include <swizzle/profiler/Profiler.hpp>

#include "../backend/VkDebug.hpp"
#include "Device.hpp"
#include "Instance.hpp"
#include "ShaderPipeline.hpp"
#include "QueryPool.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace vk
{
    Device::Device(common::Resource<Instance> inst, VkPhysicalDevice physDev, VkDevice logDev)
        : std::enable_shared_from_this<Device>()
        , mInstance(inst)
        , mPhysDevice(physDev)
        , mLogicalDevice(logDev)
        , mMemoryProperties()
        , mPhysicalDeviceProperties()
        , mPhysicalDeviceFeatures()
        , mMemoryPools()
        , mCleanup(nullptr)
        , mDescriptorPool_TEMP(VK_NULL_HANDLE)
        , mBufferCount(0u)
        , mImageCount(0u)
        , mPipelineCount(0u)
        , mDeviceStats()
    {
        // VkPhysicalDeviceMemoryBudgetPropertiesEXT budget{};
        // budget.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;
        // budget.pNext = VK_NULL_HANDLE;
        // VkPhysicalDeviceMemoryProperties2 props {};
        // props.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
        // props.pNext = &budget;
        // props.pNext = VK_NULL_HANDLE;
        //vkGetPhysicalDeviceMemoryProperties2(mPhysDevice, &props);
        vkGetPhysicalDeviceMemoryProperties(mPhysDevice, &mMemoryProperties);
        //mMemoryProperties = props.memoryProperties;

        vkGetPhysicalDeviceProperties(mPhysDevice, &mPhysicalDeviceProperties);
        vkGetPhysicalDeviceFeatures(mPhysDevice, &mPhysicalDeviceFeatures);

        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = 1000u;

        VkDescriptorPoolCreateInfo descCreateInfo{};
        descCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descCreateInfo.pNext = VK_NULL_HANDLE;
        descCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        descCreateInfo.maxSets = 1000u;
        descCreateInfo.poolSizeCount = 1u;
        descCreateInfo.pPoolSizes = &poolSize;

        vkCreateDescriptorPool(mLogicalDevice, &descCreateInfo, mInstance->getAllocCallbacks(), &mDescriptorPool_TEMP);
    }

    Device::~Device()
    {
        vkDestroyDescriptorPool(mLogicalDevice, mDescriptorPool_TEMP, mInstance->getAllocCallbacks());
        vkDestroyDevice(mLogicalDevice, mInstance->getAllocCallbacks());
    }

    void Device::shutdown()
    {
        mCleanup->stop();
        delete mCleanup;
        mCleanup = nullptr;

        mTimingQuery.reset();
        mStatisticsQuery.reset();
        deinitMemoryPools();
    }

    void Device::initAfterConstruction()
    {
        initMemoryPools();
        mCleanup = new CleanupRunnable(shared_from_this());
        mStatisticsQuery = std::make_shared<StatisticsQuery>(shared_from_this());
        U32 queryCount = 500u;
        mTimingQuery = common::CreateRef<TimingQuery>(shared_from_this(), queryCount);
    }

    void Device::waitDeviceIdle()
    {
        VkResult res = vkDeviceWaitIdle(mLogicalDevice);
        LogVulkanError(res, "vkDeviceWaitIdle");
    }

    VkDevice Device::getDeviceHandle() const
    {
        return mLogicalDevice;
    }

    VkPhysicalDevice Device::getPhysicalHandle() const
    {
        return mPhysDevice;
    }

    const SwChar* Device::getDeviceName() const
    {
        return mPhysicalDeviceProperties.deviceName;
    }

    VkPhysicalDeviceFeatures Device::getDeviceFeatures() const
    {
        return mPhysicalDeviceFeatures;
    }

    VkPhysicalDeviceProperties Device::getDeviceProperties() const
    {
        return mPhysicalDeviceProperties;
    }

    VkQueue Device::getQueue()
    {
        VkQueue que = VK_NULL_HANDLE;
        vkGetDeviceQueue(mLogicalDevice, 0, 0, &que);
        return que;
    }

    VkAllocationCallbacks* Device::getAllocCallbacks() const
    {
        return mInstance->getAllocCallbacks();
    }

    VkFence Device::createFence(FenceCreateFlags flags)
    {
        VkFence fence = VK_NULL_HANDLE;

        VkFenceCreateInfo createInfo{};
        createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = (VkFenceCreateFlagBits)flags;

        vkCreateFence(mLogicalDevice, &createInfo, mInstance->getAllocCallbacks(), &fence);

        return fence;
    }

    VkFence* Device::createFences(U32 fenceCount, FenceCreateFlags flags)
    {
        VkFence* fences = new VkFence[fenceCount];

        VkFenceCreateInfo createInfo{};
        createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = (VkFenceCreateFlagBits)flags;

        for (U32 i = 0u; i < fenceCount; ++i)
        {
            vkCreateFence(mLogicalDevice, &createInfo, mInstance->getAllocCallbacks(), &fences[i]);
        }

        return fences;
    }

    void Device::destroyFence(VkFence fence)
    {
        vkDestroyFence(mLogicalDevice, fence, mInstance->getAllocCallbacks());
    }

    void Device::destroyFences(VkFence* fences, U32 fenceCount)
    {
        for (U32 i = 0u; i < fenceCount; ++i)
        {
            vkDestroyFence(mLogicalDevice, fences[i], mInstance->getAllocCallbacks());
        }
        delete[] fences;
    }

    VkQueryPool Device::createQueryPool(VkQueryPoolCreateInfo& info)
    {
        VkQueryPool pool = VK_NULL_HANDLE;

        VkResult res = vkCreateQueryPool(mLogicalDevice, &info, mInstance->getAllocCallbacks(), &pool);
        vk::LogVulkanError(res, "vkCreateQueryPool");

        return pool;
    }

    void Device::destroyQueryPool(VkQueryPool pool)
    {
        vkDestroyQueryPool(mLogicalDevice, pool, mInstance->getAllocCallbacks());
    }

    common::Resource<VkResource<VkBuffer>> Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags flags)
    {
        VkBufferCreateInfo createInfo{};
        createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0u;
        createInfo.size = size;
        createInfo.usage = flags;
        createInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0u;
        createInfo.pQueueFamilyIndices = VK_NULL_HANDLE;

        VkBuffer buffer = VK_NULL_HANDLE;
        VkResult res = vkCreateBuffer(mLogicalDevice, &createInfo, mInstance->getAllocCallbacks(), &buffer);
        vk::LogVulkanError(res, "vkCreateBuffer");
        mBufferCount++;

        return common::CreateRef<VkResource<VkBuffer>>(buffer, ResourceType::BufferResource);
    }

    common::Resource<VkResource<VkImage>> Device::createImage(VkImageType type, VkFormat format, VkImageUsageFlags usage, VkExtent3D size, U32 layerCount, U32 mipLevel)
    {
        U32 flags = 0;
        if (layerCount == 6)
        {
            flags = VkImageCreateFlagBits::VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        }
        VkImageCreateInfo createInfo{};
        createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = flags;
        createInfo.imageType = type;
        createInfo.format = format;
        createInfo.extent = size;
        createInfo.mipLevels = mipLevel;
        createInfo.arrayLayers = layerCount;
        createInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
        createInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
        createInfo.usage = usage;
        createInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0u;
        createInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
        createInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_PREINITIALIZED;

        VkImage img = VK_NULL_HANDLE;
        VkResult res = vkCreateImage(mLogicalDevice, &createInfo, mInstance->getAllocCallbacks(), &img);
        vk::LogVulkanError(res, "vkCreateImage");
        mImageCount++;

        return common::CreateRef<VkResource<VkImage>>(img, ResourceType::ImageResource);
    }

    common::Resource<VkResource<VkDescriptorSet>> Device::allocateDescriptorSet(common::Resource<ShaderPipeline> shader)
    {
        std::unique_lock l{ mDescriptorMutex };
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.pNext = VK_NULL_HANDLE;
        allocInfo.descriptorPool = getDescriptorPool_TEMP();
        allocInfo.descriptorSetCount = 1;
        auto layout = shader->getDescriptorLayout();
        allocInfo.pSetLayouts = &layout;

        VkDescriptorSet desc = VK_NULL_HANDLE;

        VkResult res = vkAllocateDescriptorSets(mLogicalDevice, &allocInfo, &desc);
        vk::LogVulkanError(res, "vkAllocateDescriptorSets");
        mDescriptorCount++;

        return common::CreateRef<VkResource<VkDescriptorSet>>(desc, ResourceType::DescriptorResource);
    }

    void Device::destroyResource(common::Resource<IVkResource>& resource)
    {
        if (resource->getType() == ResourceType::BufferResource)
        {
            destroyBuffer((common::Resource<VkResource<VkBuffer>>&)resource);
        }
        else if (resource->getType() == ResourceType::ImageResource)
        {
            destroyTexture((common::Resource<VkResource<VkImage>>&)resource);
        }
        else if (resource->getType() == ResourceType::DescriptorResource)
        {
            freeDescriptorSet((common::Resource<VkResource<VkDescriptorSet>>&)resource);
        }
    }

    common::Resource<DeviceMemory> Device::allocateMemory(VkMemoryPropertyFlags propertyFlags,
                                                          VkMemoryRequirements req)
    {
        SWIZZLE_PROFILE_EVENT("Device::allocateMemory");
        U32 memoryTypeIndex = ~0u; // Unable to find memoryType
        U32 heapIndex = ~0u;

        for (U32 i = 0u; i < mMemoryProperties.memoryTypeCount; i++)
        {
            U32 memProp = (mMemoryProperties.memoryTypes[i].propertyFlags & propertyFlags);
            if ((memProp == (U32)propertyFlags) && (req.memoryTypeBits & (1U << i)))
            {
                memoryTypeIndex = i;
                heapIndex = mMemoryProperties.memoryTypes[i].heapIndex;
                break;
            }
        }

        common::Resource<DeviceMemory> mem = nullptr;

        if ((heapIndex != ~0u) && (memoryTypeIndex != ~0u))
        {
            mem = mMemoryPools[heapIndex]->allocateMemory(req, memoryTypeIndex);
        }

        return mem;
    }

    void Device::freeMemory(common::Resource<DeviceMemory> memory)
    {
        SWIZZLE_PROFILE_EVENT("Device::freeMemory");
        common::Resource<DeviceMemoryPool> pool = memory->mOwnerPool;
        pool->freeMemory(memory);
    }

    void Device::scheduleResourceDestruction(common::Resource<IVkResource> resource)
    {
        SWIZZLE_PROFILE_EVENT("Device::scheduleResourceDestruction");
        // send to cleanup thread
        mCleanup->scheduleResourceDestruction(resource);
    }

    void Device::scheduleFreeingMemory(common::Resource<DeviceMemory> memory)
    {
        SWIZZLE_PROFILE_EVENT("Device::scheduleFreeingMemory");
        // send to cleanup thread
        mCleanup->scheduleFreeingMemory(memory);
    }

    void Device::updateHeapBudget()
    {
        VkPhysicalDeviceMemoryBudgetPropertiesEXT budget{};
        budget.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;
        budget.pNext = VK_NULL_HANDLE;
        VkPhysicalDeviceMemoryProperties2 props{};
        props.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
        props.pNext = &budget;
        vkGetPhysicalDeviceMemoryProperties2(mPhysDevice, &props);

        for (U32 i = 0u; i < props.memoryProperties.memoryHeapCount; ++i)
        {
            mMemoryPools[i]->updateBudget(budget.heapBudget[i], budget.heapUsage[i]);
        }
    }

    VkDescriptorPool Device::getDescriptorPool_TEMP() const
    {
        return mDescriptorPool_TEMP;
    }

    U32 Device::getBufferCount() const
    {
        return mBufferCount;
    }

    U32 Device::getImageCount() const
    {
        return mImageCount;
    }

    U64 Device::getGPUMemUsage() const
    {
        U64 usedMemory = 0ull;
        for (U32 i = 0u; i < mMemoryProperties.memoryHeapCount; ++i)
        {
            if (mMemoryProperties.memoryHeaps[i].flags == VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            {
                usedMemory += mMemoryPools[i]->getUsedSize();
            }
        }

        return usedMemory;
    }

    U64 Device::getCPUMemUsage() const
    {
        U64 usedMemory = 0ull;
        for (U32 i = 0u; i < mMemoryProperties.memoryHeapCount; ++i)
        {
            if (mMemoryProperties.memoryHeaps[i].flags != VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            {
                usedMemory += mMemoryPools[i]->getUsedSize();
            }
        }

        return usedMemory;
    }

    void Device::performCleanup()
    {
        mCleanup->notify();
    }

    U32 Device::getMemoryHeapCount() const
    {
        return (U32)mMemoryPools.size();
    }

    swizzle::gfx::MemoryStatistics* Device::getHeapStatsistics(U32 index) const
    {
        return mMemoryPools[index]->getMemoryStats();
    }

    swizzle::gfx::DeviceStatistics* Device::getDeviceStats()
    {
        mDeviceStats.mNumBuffers = mBufferCount;
        mDeviceStats.mNumTextures = mImageCount;
        mDeviceStats.mNumPipelines = mPipelineCount;
        mDeviceStats.mNumDescriptors = mDescriptorCount;
        return &mDeviceStats;
    }

    common::Resource<Query> Device::getStatisticsQuery() const
    {
        return mStatisticsQuery;
    }

    common::Resource<Query> Device::getTimingQuery() const
    {
        return mTimingQuery;
    }

}

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{
    void Device::initMemoryPools()
    {
        for (U32 i = 0u; i < mMemoryProperties.memoryHeapCount; ++i)
        {
            VkDeviceSize chunkSize = 1024llu * 1024llu * 4llu;
            std::string poolName = "Device Memory (GPU)";
            if(mMemoryProperties.memoryHeaps[i].flags != VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            {
                chunkSize = 1024llu * 1024llu * 16llu;
                poolName = "System Memory (CPU)";
            }

            for (U32 j = 0u; j < mMemoryProperties.memoryTypeCount; ++j)
            {
                if ((mMemoryProperties.memoryTypes[j].heapIndex == i) &&
                    (mMemoryProperties.memoryTypes[j].propertyFlags & VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
                {
                    poolName += " [Host Visible]";
                    break;
                }
            }

            mMemoryPools.push_back(common::CreateRef<DeviceMemoryPool>(shared_from_this(), chunkSize, poolName));
        }
    }

    void Device::deinitMemoryPools()
    {
        mMemoryPools.clear();
    }

    void Device::destroyBuffer(common::Resource<VkResource<VkBuffer>> resource)
    {
        VkBuffer buf = resource->reset();
        vkDestroyBuffer(mLogicalDevice, buf, mInstance->getAllocCallbacks());
        mBufferCount--;
    }

    void Device::destroyTexture(common::Resource<VkResource<VkImage>> resource)
    {
        VkImage img = resource->reset();
        vkDestroyImage(mLogicalDevice, img, mInstance->getAllocCallbacks());
        mImageCount--;
    }

    void Device::freeDescriptorSet(common::Resource<VkResource<VkDescriptorSet>> resource)
    {
        std::unique_lock l{ mDescriptorMutex };
        VkDescriptorSet set = resource->reset();
        vkFreeDescriptorSets(mLogicalDevice, getDescriptorPool_TEMP(), 1, &set);
        mDescriptorCount--;
    }

} // namespace vk