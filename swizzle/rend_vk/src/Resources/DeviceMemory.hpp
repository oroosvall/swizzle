
#pragma once

/* Include files */

#include "Vk.hpp"
#include "_fw.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <vector>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace rvk
{
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
} // namespace rvk

/* Class Declaration */

namespace rvk
{

    class DeviceMemory
    {
    public:
        DeviceMemory(std::shared_ptr<DeviceMemoryPool> ownerPool,
                     VkDeviceMemory memory,
                     VkDeviceSize offset,
                     VkDeviceSize size)
            : mOwnerPool(ownerPool)
            , mMemory(memory)
            , mAlignOffset(offset)
            , mSize(size)
            , mUserCount(0ull)
            , mFreed(false)
        {
        }

        virtual ~DeviceMemory()
        {
        }

        std::shared_ptr<DeviceMemoryPool> mOwnerPool;
        VkDeviceMemory mMemory;
        VkDeviceSize mAlignOffset;
        VkDeviceSize mSize;

        uint32_t mUserCount;
        bool mFreed;

        void addUser();
        void removeUser();
        uint32_t activeUserCount();
    };

    class MemoryChunk
    {
    public:
        MemoryChunk(std::shared_ptr<Device> device, VkDeviceMemory memory, uint32_t memoryTypeIndex, VkDeviceSize size);
        ~MemoryChunk();

        uint32_t getMemoryTypeIndex() const;
        uint32_t getAllocCount() const;
        bool canAllocate(VkMemoryRequirements reqs, uint32_t memoryTypeIndex) const;
        bool isOwner(std::shared_ptr<DeviceMemory> memory) const;
        std::shared_ptr<DeviceMemory> allocate(VkMemoryRequirements reqs, std::shared_ptr<DeviceMemoryPool> pool);
        void free(std::shared_ptr<DeviceMemory> memory);

        VkDeviceSize getTotal() const;
        VkDeviceSize getUsed() const;
        bool empty() const;

    private:
        VkDeviceSize calculateAlignmentOffset(VkDeviceSize currentOffset, VkDeviceSize requiredAlignment) const;

        std::shared_ptr<Device> mDevice;
        VkDeviceMemory mMemory;
        VkDeviceSize mTotalSize;
        VkDeviceSize mUsedSize;
        uint32_t mMemoryTypeIndex;
        std::vector<Fragment> mFreeFragments;
        uint32_t mVAllocCount;
    };

    class DeviceMemoryPool : public std::enable_shared_from_this<DeviceMemoryPool>
    {
    public:
        DeviceMemoryPool(std::shared_ptr<Device> device, VkDeviceSize chunkSize, std::string poolName);
        virtual ~DeviceMemoryPool();

        void updateBudget(VkDeviceSize budget, VkDeviceSize usage);

        std::shared_ptr<DeviceMemory> allocateMemory(VkMemoryRequirements reqs, uint32_t memoryTypeIndex);
        void freeMemory(std::shared_ptr<DeviceMemory> memory);

    private:
        std::shared_ptr<MemoryChunk> allocateNewChunk(VkDeviceSize size, uint32_t memoryTypeIndex);
        std::shared_ptr<MemoryChunk> getChunk(VkMemoryRequirements memreq, uint32_t memoryTypeIndex);

        void increaseChunkSize();

        std::shared_ptr<Device> mDevice;
        VkDeviceSize mChunkSize;
        std::string mPoolName;

        std::vector<std::shared_ptr<MemoryChunk>> mMemoryChunks;

        std::mutex mLock;
    };
} // namespace rvk

/* Function Declaration */