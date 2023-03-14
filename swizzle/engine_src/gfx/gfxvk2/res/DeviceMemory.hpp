#ifndef DEVICE_MEMORY_HPP
#define DEVICE_MEMORY_HPP

/* Include files */

#include <swizzle/gfx/GfxStats.hpp>
#include <common/Common.hpp>
#include "_fwDecl.hpp"

#include "../backend/Vk.hpp"

#include <vector>
#include <memory>
#include <mutex>
#include <string>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace vk
{
    class DeviceMemory
    {
    public:
        DeviceMemory(common::Resource<DeviceMemoryPool> ownerPool, VkDeviceMemory memory, VkDeviceSize offset,
                     VkDeviceSize size)
            : mOwnerPool(ownerPool)
            , mMemory(memory)
            , mAlignOffset(offset)
            , mSize(size)
            , mResources()
            , mFreed(false)
        {
        }

        common::Resource<DeviceMemoryPool> mOwnerPool;
        VkDeviceMemory mMemory;
        VkDeviceSize mAlignOffset;
        VkDeviceSize mSize;

        std::vector<std::weak_ptr<IVkResource>> mResources;
        SwBool mFreed;

        void bind(common::Resource<Device> device, common::Resource<IVkResource> resource);
        U32 activeUserCound();
    };

    struct Fragment
    {
        VkDeviceSize mOffset;
        VkDeviceSize mSize;
    };

    struct AlignmentInfo
    {
        VkDeviceSize mSize;
        VkDeviceSize mAlignOffset;
    };

}

/* Class Declaration */

namespace vk
{
    class MemoryChunk
    {
    public:
        MemoryChunk(common::Resource<Device> device, VkDeviceMemory memory, U32 memoryTypeIndex, VkDeviceSize size);
        ~MemoryChunk();

        U32 getMemoryTypeIndex() const;
        U32 getAllocCount() const;
        SwBool canAllocate(VkMemoryRequirements reqs, U32 memoryTypeIndex) const;
        SwBool isOwner(common::Resource<DeviceMemory> memory) const;
        common::Resource<DeviceMemory> allocate(VkMemoryRequirements reqs, common::Resource<DeviceMemoryPool> pool);
        void free(common::Resource<DeviceMemory> memory);

        VkDeviceSize getTotal() const;
        VkDeviceSize getUsed() const;
    private:

        VkDeviceSize calculateAlignmentOffset(VkDeviceSize currentOffset, VkDeviceSize requiredAlignment) const;

        common::Resource<Device> mDevice;
        VkDeviceMemory mMemory;
        VkDeviceSize mTotalSize;
        VkDeviceSize mUsedSize;
        U32 mMemoryTypeIndex;
        std::vector<Fragment> mFreeFragments;
        U32 mVAllocCount;
    };

    class DeviceMemoryPool : public std::enable_shared_from_this<DeviceMemoryPool>
    {
    public:

        DeviceMemoryPool(common::Resource<Device> device, VkDeviceSize chunkSize, std::string memoryPoolName);
        ~DeviceMemoryPool();

        void updateBudget(VkDeviceSize budget, VkDeviceSize usage);

        VkDeviceSize getFreeSize();
        VkDeviceSize getUsedSize();
        VkDeviceSize getTotalSize();

        common::Resource<DeviceMemory> allocateMemory(VkMemoryRequirements reqs, U32 memoryTypeIndex);
        void freeMemory(common::Resource<DeviceMemory> memory);

        swizzle::gfx::MemoryStatistics* getMemoryStats();

    private:

        common::Resource<MemoryChunk> allocateNewChunk(VkDeviceSize size, U32 memoryTypeIndex);
        common::Resource<MemoryChunk> getChunk(VkMemoryRequirements memreq, U32 memoryTypeIndex);

        U32 getVirtAllocs() const;
        void increaseChunkSiez();

        common::Resource<Device> mDevice;
        std::vector<common::Resource<MemoryChunk>> mMemoryChunks;
        VkDeviceSize mChunkSize;
        std::string mMemoryPoolName;

        VkDeviceSize mMaxHeapSize;
        VkDeviceSize mOtherUseSize;

        std::mutex mLock;

        swizzle::gfx::MemoryStatistics mStatistics;

    };
}

/* Function Declaration */

#endif
