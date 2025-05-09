#ifndef DEVICE_HPP
#define DEVICE_HPP

/* Include files */

#include <swizzle/gfx/GfxStats.hpp>

#include "../backend/Vk.hpp"
#include "_fwDecl.hpp"

#include "DeviceMemory.hpp"
#include "VkResource.hpp"

#include <memory>

#include <atomic>

#include "Cleanup.hpp"

/* Defines */

/* Typedefs/enums */

namespace vk
{
    enum class FenceCreateFlags
    {
        notSignaled = 0x0,
        signaled = 0x1
    };
}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace vk
{

    class Device : public std::enable_shared_from_this<Device>
    {
    public:
        Device(common::Resource<Instance> inst, VkPhysicalDevice physDev, VkDevice logDev);
        virtual ~Device();

        void shutdown();

        void initAfterConstruction();

        void waitDeviceIdle();

        VkDevice getDeviceHandle() const;
        VkPhysicalDevice getPhysicalHandle() const;
        const SwChar* getDeviceName() const;
        VkPhysicalDeviceFeatures getDeviceFeatures() const;
        VkPhysicalDeviceProperties getDeviceProperties() const;

        VkQueue getQueue();

        VkAllocationCallbacks* getAllocCallbacks() const;

        VkFence createFence(FenceCreateFlags flags);
        VkFence* createFences(U32 fenceCount, FenceCreateFlags flags);
        void destroyFence(VkFence fence);
        void destroyFences(VkFence* fences, U32 fenceCount);

        VkQueryPool createQueryPool(VkQueryPoolCreateInfo& info);
        void destroyQueryPool(VkQueryPool pool);

        common::Resource<VkResource<VkBuffer>> createBuffer(VkDeviceSize size, VkBufferUsageFlags flags);
        common::Resource<VkResource<VkImage>> createImage(VkImageType type, VkFormat format, VkImageUsageFlags usage,
                                                          VkExtent3D size, U32 layerCount, U32 mipLevel);
        common::Resource<VkResource<VkDescriptorSet>> allocateDescriptorSet(common::Resource<ShaderPipeline> shader);

        void destroyResource(common::Resource<IVkResource>& resource);

        common::Resource<DeviceMemory> allocateMemory(VkMemoryPropertyFlags properties, VkMemoryRequirements req);
        void freeMemory(common::Resource<DeviceMemory> memory);

        void scheduleResourceDestruction(common::Resource<IVkResource> resource);
        void scheduleFreeingMemory(common::Resource<DeviceMemory> memory);

        void updateHeapBudget();

        VkDescriptorPool getDescriptorPool_TEMP() const;

        U32 getBufferCount() const;
        U32 getImageCount() const;

        U64 getGPUMemUsage() const;
        U64 getCPUMemUsage() const;

        void performCleanup();

        U32 getMemoryHeapCount() const;
        swizzle::gfx::MemoryStatistics* getHeapStatsistics(U32 index) const;
        swizzle::gfx::DeviceStatistics* getDeviceStats();

        common::Resource<Query> getStatisticsQuery() const;
        common::Resource<Query> getTimingQuery() const;

    private:
        void initMemoryPools();
        void deinitMemoryPools();

        void destroyBuffer(common::Resource<VkResource<VkBuffer>> resource);
        void destroyTexture(common::Resource<VkResource<VkImage>> resource);
        void freeDescriptorSet(common::Resource<VkResource<VkDescriptorSet>> resource);

        common::Resource<Instance> mInstance;

        VkPhysicalDevice mPhysDevice;
        VkDevice mLogicalDevice;

        VkPhysicalDeviceMemoryProperties mMemoryProperties;
        VkPhysicalDeviceProperties mPhysicalDeviceProperties;
        VkPhysicalDeviceFeatures mPhysicalDeviceFeatures;

        std::vector<common::Resource<DeviceMemoryPool>> mMemoryPools;

        CleanupRunnable* mCleanup;

        VkDescriptorPool mDescriptorPool_TEMP;

        std::atomic_uint32_t mBufferCount;
        std::atomic_uint32_t mImageCount;
        std::atomic_uint32_t mPipelineCount;
        std::atomic_uint32_t mDescriptorCount;

        swizzle::gfx::DeviceStatistics mDeviceStats;

        common::Resource<Query> mStatisticsQuery;
        common::Resource<Query> mTimingQuery;


        std::mutex mDescriptorMutex;
    };
} // namespace vk
/* Function Declaration */

#endif
