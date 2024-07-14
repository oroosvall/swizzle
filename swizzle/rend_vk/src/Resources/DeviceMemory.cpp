
/* Include files */

#include "DeviceMemory.hpp"
#include "Device.hpp"

#include "../Log.hpp"

#include <assert.h>

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

    ///
    /// Device Memory
    ///

    void DeviceMemory::addUser()
    {
        mUserCount++;
    }

    void DeviceMemory::removeUser()
    {
        mUserCount--;
    }

    uint32_t DeviceMemory::activeUserCount()
    {
        return mUserCount;
    }

    ///
    /// MemoryChunk
    ///
    MemoryChunk::MemoryChunk(std::shared_ptr<Device> device,
                             VkDeviceMemory memory,
                             uint32_t memoryTypeIndex,
                             VkDeviceSize size)
        : mDevice(device)
        , mMemory(memory)
        , mTotalSize(size)
        , mUsedSize(0ull)
        , mMemoryTypeIndex(memoryTypeIndex)
        , mFreeFragments()
        , mVAllocCount(0ul)
    {
        Fragment frag{};
        frag.mOffset = 0ull;
        frag.mSize   = size;
        mFreeFragments.push_back(frag);
    }

    MemoryChunk::~MemoryChunk()
    {
        vkFreeMemory(mDevice->getHandle(), mMemory, mDevice->getAllocCallbacks());
    }

    uint32_t MemoryChunk::getMemoryTypeIndex() const
    {
        return mMemoryTypeIndex;
    }

    uint32_t MemoryChunk::getAllocCount() const
    {
        return mVAllocCount;
    }

    bool MemoryChunk::canAllocate(VkMemoryRequirements reqs, uint32_t memoryTypeIndex) const
    {
        bool canAlloc = false;
        // Check that this memory chunk is of correct memroyType
        if (mMemoryTypeIndex == memoryTypeIndex)
        {
            for (const auto& frag : mFreeFragments)
            {
                // calculate the offset for making the memory aligned
                VkDeviceSize alignmentOffset = calculateAlignmentOffset(frag.mOffset, reqs.alignment);

                // check that the frag.mSize is not underflowing
                // that would cause allocation to happen when not supposed to
                if (alignmentOffset > frag.mSize)
                {
                    continue;
                }

                // After allocation this is the actual size we have in the fragment
                VkDeviceSize adjustedSize = frag.mSize - alignmentOffset;

                // check if the fragment can fit the required allocation
                if (adjustedSize >= reqs.size)
                {
                    canAlloc = true;
                    break;
                }
            }
        }
        return canAlloc;
    }

    bool MemoryChunk::isOwner(std::shared_ptr<DeviceMemory> memory) const
    {
        return mMemory == memory->mMemory;
    }

    std::shared_ptr<DeviceMemory> MemoryChunk::allocate(VkMemoryRequirements reqs,
                                                        std::shared_ptr<DeviceMemoryPool> pool)
    {
        std::shared_ptr<DeviceMemory> mem = nullptr;

        Fragment frag1{};

        for (auto& frag : mFreeFragments)
        {
            // calculate the offset for making the memory aligned
            VkDeviceSize alignmentOffset = calculateAlignmentOffset(frag.mOffset, reqs.alignment);

            // check that the frag.mSize is not underflowing
            // that would cause allocation to happen when not supposed to
            if (alignmentOffset > frag.mSize)
            {
                continue;
            }
            // After allocation this is the actual size we have in the fragment
            VkDeviceSize adjustedSize = frag.mSize - alignmentOffset;

            // check if the fragment can fit the required allocation
            if (adjustedSize >= reqs.size)
            {
                // up to 3 new fragments can be created
                // |----------------------------|-------------------|----------------|
                // | fragmet by aligning offset | actual allocation | remaining size |
                // | alignmentOffset != 0       | claimed memory    |  remaining     |
                // |----------------------------|-------------------|----------------|

                // create memory
                mem = std::make_shared<DeviceMemory>(pool, mMemory, frag.mOffset + alignmentOffset, reqs.size);

                // adjust free fragments
                if (alignmentOffset != 0ull)
                {
                    frag1.mOffset = frag.mOffset;
                    frag1.mSize   = alignmentOffset;
                }

                // adjust the current fragment
                // this will become the remaining fragment after claimed memory
                frag.mSize -= (alignmentOffset + mem->mSize);
                frag.mOffset += alignmentOffset + mem->mSize;
                mUsedSize += mem->mSize;
                assert(mUsedSize <= mTotalSize);
                mVAllocCount++;

                break;
            }
        }

        mFreeFragments.erase(std::remove_if(mFreeFragments.begin(),
                                            mFreeFragments.end(),
                                            [](const Fragment& f) { return f.mSize == 0; }),
                             mFreeFragments.end());

        if (frag1.mSize != 0ull)
        {
            mFreeFragments.push_back(frag1);
        }

        return mem;
    }

    void MemoryChunk::free(std::shared_ptr<DeviceMemory> memory)
    {
        VkDeviceSize checkOffset = memory->mAlignOffset + memory->mSize;

        for (auto& frag : mFreeFragments)
        {
            // This matches
            // |--------------|----------|
            // | memoryToFree | fragment |
            // |--------------|----------|
            if (checkOffset == frag.mOffset)
            {
                frag.mOffset = memory->mAlignOffset;
                frag.mSize += memory->mSize;
                mUsedSize -= memory->mSize;
                mVAllocCount--;
                checkOffset    = ~0ull;
                memory->mFreed = true;
                break;
            }
            // This matches
            // |----------|--------------|
            // | fragment | memoryToFree |
            // |----------|--------------|
            else if (frag.mOffset + frag.mSize == memory->mAlignOffset)
            {
                frag.mSize += memory->mSize;
                mUsedSize -= memory->mSize;
                mVAllocCount--;
                checkOffset    = ~0ull;
                memory->mFreed = true;
                break;
            }
            else
            {
                // do nothing
            }
        }

        // This is if we did not have any fragments adjacent to the chunk so create a new fragment
        if (checkOffset != ~0ull)
        {
            mFreeFragments.emplace_back(Fragment{memory->mAlignOffset, memory->mSize});
            mUsedSize -= memory->mSize;
            mVAllocCount--;
            memory->mFreed = true;
        }
    }

    VkDeviceSize MemoryChunk::getTotal() const
    {
        return mTotalSize;
    }

    VkDeviceSize MemoryChunk::getUsed() const
    {
        return mUsedSize;
    }

    bool MemoryChunk::empty() const
    {
        return getUsed() == 0ull;
    }

    ///
    /// DeviceMemoryPool
    ///

    DeviceMemoryPool::DeviceMemoryPool(std::shared_ptr<Device> device, VkDeviceSize chunkSize, std::string poolName)
        : mDevice(device)
        , mChunkSize(chunkSize)
        , mPoolName(poolName)
    {
    }

    DeviceMemoryPool::~DeviceMemoryPool()
    {
        mMemoryChunks.clear();
    }

    void DeviceMemoryPool::updateBudget(VkDeviceSize budget, VkDeviceSize usage)
    {
        RVK_UNUSED_ARG(budget);
        RVK_UNUSED_ARG(usage);
    }

    std::shared_ptr<DeviceMemory> DeviceMemoryPool::allocateMemory(VkMemoryRequirements reqs, uint32_t memoryTypeIndex)
    {
        std::lock_guard l(mLock);

        std::shared_ptr<DeviceMemory> mem = nullptr;

        std::shared_ptr<MemoryChunk> ch = getChunk(reqs, memoryTypeIndex);

        if (!ch)
        {
            VkDeviceSize chSize = mChunkSize;
            if (reqs.size > chSize)
            {
                chSize = reqs.size;
            }
            ch = allocateNewChunk(chSize, memoryTypeIndex);
            increaseChunkSize();
        }

        if (ch)
        {
            mem = ch->allocate(reqs, shared_from_this());
        }

        return mem;
    }

    void DeviceMemoryPool::freeMemory(std::shared_ptr<DeviceMemory> memory)
    {
        std::lock_guard l(mLock);
        for (auto& ch : mMemoryChunks)
        {
            if (ch->isOwner(memory))
            {
                ch->free(memory);
            }
        }

        mMemoryChunks.erase(std::remove_if(mMemoryChunks.begin(),
                                           mMemoryChunks.end(),
                                           [](const auto& chunk) { return chunk->empty(); }),
                            mMemoryChunks.end());
    }

} // namespace rvk

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace rvk
{

    VkDeviceSize MemoryChunk::calculateAlignmentOffset(VkDeviceSize currentOffset, VkDeviceSize requiredAlignment) const
    {
        VkDeviceSize alval        = requiredAlignment - 1u;
        VkDeviceSize alignedValue = (currentOffset + alval) & ~(alval);
        VkDeviceSize newOffset    = alignedValue - currentOffset;

        return newOffset;
    }

    std::shared_ptr<MemoryChunk> DeviceMemoryPool::allocateNewChunk(VkDeviceSize size, uint32_t memoryTypeIndex)
    {
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType           = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext           = VK_NULL_HANDLE;
        allocInfo.allocationSize  = size;
        allocInfo.memoryTypeIndex = memoryTypeIndex;

        std::shared_ptr<MemoryChunk> chunk = nullptr;
        VkDeviceMemory mem                 = VK_NULL_HANDLE;

        VkResult res = vkAllocateMemory(mDevice->getHandle(), &allocInfo, mDevice->getAllocCallbacks(), &mem);
        if (res == VK_SUCCESS)
        {
            chunk = std::make_shared<MemoryChunk>(mDevice, mem, memoryTypeIndex, size);
            mMemoryChunks.emplace_back(chunk);
            LogMessage(LogLevel::Info, "Allocated new memory chunk, %llu", size);
        }
        else
        {
            rvk::LogVulkanError(res, "vkAllocateMemory");
        }
        return chunk;
    }

    std::shared_ptr<MemoryChunk> DeviceMemoryPool::getChunk(VkMemoryRequirements memreq, uint32_t memoryTypeIndex)
    {
        std::shared_ptr<MemoryChunk> chunk = nullptr;

        for (auto& ch : mMemoryChunks)
        {
            if (ch->canAllocate(memreq, memoryTypeIndex))
            {
                chunk = ch;
                break;
            }
        }

        return chunk;
    }

    void DeviceMemoryPool::increaseChunkSize()
    {
        const VkDeviceSize maxChunkSize = 1024ull * 1024ull * 64ull;
        mChunkSize *= 2ull;
        if (mChunkSize > maxChunkSize)
        {
            mChunkSize = maxChunkSize;
        }
    }

} // namespace rvk
