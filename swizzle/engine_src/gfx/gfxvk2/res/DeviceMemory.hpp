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
            , mOffset(offset)
            , mAlignOffset(0ull)
            , mSize(size)
            , mResources()
            , mFreed(false)
            {}

        common::Resource<DeviceMemoryPool> mOwnerPool;
        VkDeviceMemory mMemory;
        VkDeviceSize mOffset;
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

    struct Chunk
    {
        VkDeviceMemory mMemory;
        VkDeviceSize mTotalSize;
        VkDeviceSize mUsedSize;
        U32 memoryTypeIndex;
        std::vector<Fragment> mFreeFragments;
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

        void allocateNewChunk(VkDeviceSize size, U32 memoryTypeIndex);

        Chunk* getChunk(VkDeviceSize size, U32 memoryTypeIndex);

        AlignmentInfo calcAlignedSize(VkDeviceSize offset, VkMemoryRequirements memReq);

        common::Resource<Device> mDevice;
        std::vector<Chunk> mMemoryChunks;
        VkDeviceSize mChunkSize;
        std::string mMemoryPoolName;

        VkDeviceSize mTotalAllocated;
        VkDeviceSize mUsedSize;

        VkDeviceSize mMaxHeapSize;
        VkDeviceSize mOtherUseSize;

        std::mutex mLock;

        U32 mVAllocCount;
        swizzle::gfx::MemoryStatistics mStatistics;

    };
}

/* Function Declaration */

#endif
