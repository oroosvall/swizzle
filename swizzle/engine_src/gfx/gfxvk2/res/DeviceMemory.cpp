
/* Include files */

#include "DeviceMemory.hpp"

#include "VkResource.hpp"
#include "Device.hpp"

#include <algorithm>

#include <optick/optick.h>

#include <assert.h>

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
    /*
    Device Memory
    */
    void DeviceMemory::bind(common::Resource<Device> device, common::Resource<IVkResource> resource)
    {
        bool added = false;
        for (U32 i = 0u; i < mResources.size(); ++i)
        {
            if (mResources[i].expired())
            {
                mResources[i] = resource;
                added = true;
                break;
            }
        }

        if (!added)
        {
            mResources.push_back(resource);
        }

        if (resource->getType() == ResourceType::BufferResource)
        {
            VkResource<VkBuffer>* res = (VkResource<VkBuffer>*)resource.get();
            vkBindBufferMemory(device->getDeviceHandle(), res->getVkHandle(), mMemory, mAlignOffset);
        }
        else if (resource->getType() == ResourceType::ImageResource)
        {
            VkResource<VkImage>* res = (VkResource<VkImage>*)resource.get();
            vkBindImageMemory(device->getDeviceHandle(), res->getVkHandle(), mMemory, mAlignOffset);
        }
        else
        {
            LOG_ERROR("DeviceMemory::bind, undefined resource type, cannot bind memory");
        }
    }

    U32 DeviceMemory::activeUserCound()
    {
        U32 userCount = 0u;
        for (auto& r : mResources)
        {
            if (!r.expired())
            {
                userCount++;
            }
        }
        return userCount;
    }

    /*
    MemoryChunk
    */

    MemoryChunk::MemoryChunk(common::Resource<Device> device, VkDeviceMemory memory, U32 memoryTypeIndex, VkDeviceSize size)
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
        frag.mSize = size;
        mFreeFragments.push_back(frag);
    }

    MemoryChunk::~MemoryChunk()
    {
        vkFreeMemory(mDevice->getDeviceHandle(), mMemory, mDevice->getAllocCallbacks());
    }

    U32 MemoryChunk::getMemoryTypeIndex() const
    {
        return mMemoryTypeIndex;
    }

    U32 MemoryChunk::getAllocCount() const
    {
        return mVAllocCount;
    }

    SwBool MemoryChunk::canAllocate(VkMemoryRequirements reqs, U32 memoryTypeIndex) const
    {
        SwBool canAlloc = false;
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

    SwBool MemoryChunk::isOwner(common::Resource<DeviceMemory> memory) const
    {
        return mMemory == memory->mMemory;
    }

    common::Resource<DeviceMemory> MemoryChunk::allocate(VkMemoryRequirements reqs, common::Resource<DeviceMemoryPool> pool)
    {
        common::Resource<DeviceMemory> mem = nullptr;

        Fragment frag1 {};

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
                mem = common::CreateRef<DeviceMemory>(pool, mMemory, frag.mOffset + alignmentOffset, reqs.size);

                // adjust free fragments
                if (alignmentOffset != 0ull)
                {
                    frag1.mOffset = frag.mOffset;
                    frag1.mSize = alignmentOffset;
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

        mFreeFragments.erase(
            std::remove_if(
                mFreeFragments.begin(),
                mFreeFragments.end(),
                [](const Fragment& f) { return f.mSize == 0; }),
            mFreeFragments.end());

        if (frag1.mSize != 0ull)
        {
            mFreeFragments.push_back(frag1);
        }

        return mem;
    }

    void MemoryChunk::free(common::Resource<DeviceMemory> memory)
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
                checkOffset = ~0ull;
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
                checkOffset = ~0ull;
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
            mFreeFragments.emplace_back(Fragment{ memory->mAlignOffset, memory->mSize });
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

    /*
    DeviceMemoryPool
    */

    DeviceMemoryPool::DeviceMemoryPool(common::Resource<Device> device, VkDeviceSize chunkSize, std::string memoryPoolName)
        : std::enable_shared_from_this<DeviceMemoryPool>()
        , mDevice(device)
        , mMemoryChunks()
        , mChunkSize(chunkSize)
        , mMemoryPoolName(memoryPoolName)
        , mMaxHeapSize(0ull)
        , mOtherUseSize(0ull)
        , mLock()
        , mStatistics()
    {
    }

    DeviceMemoryPool::~DeviceMemoryPool()
    {
        //for (auto& ch : mMemoryChunks)
        //{
        //    vkFreeMemory(mDevice->getDeviceHandle(), ch.mMemory, mDevice->getAllocCallbacks());
        //}
        mMemoryChunks.clear();
    }

    void DeviceMemoryPool::updateBudget(VkDeviceSize budget, VkDeviceSize usage)
    {
        // TODO: Use heap budget
        UNUSED_ARG(budget);
        UNUSED_ARG(usage);
    }

    VkDeviceSize DeviceMemoryPool::getFreeSize()
    {
        return (getTotalSize() - getUsedSize());
    }

    VkDeviceSize DeviceMemoryPool::getUsedSize()
    {
        VkDeviceSize used = 0ull;
        for (const auto& ch : mMemoryChunks)
        {
            used +=ch->getUsed();
        }
        return used;
    }

    VkDeviceSize DeviceMemoryPool::getTotalSize()
    {
        VkDeviceSize allocated = 0ull;
        for (const auto& ch : mMemoryChunks)
        {
            allocated += ch->getTotal();
        }
        return allocated;
    }

    common::Resource<DeviceMemory> DeviceMemoryPool::allocateMemory(VkMemoryRequirements reqs, U32 memoryTypeIndex)
    {
        std::lock_guard lck(mLock);

        OPTICK_EVENT("DeviceMemoryPool::allocateMemory");
        // TODO: Needs thread locking for multiple threads
        // TODO: handle alignment from reqs
        common::Resource<DeviceMemory> mem = nullptr;

        common::Resource<MemoryChunk> ch = getChunk(reqs, memoryTypeIndex);

        if (!ch)
        {
            VkDeviceSize chSize = mChunkSize;
            if (reqs.size > chSize)
            {
                chSize = reqs.size;
            }
            ch = allocateNewChunk(chSize, memoryTypeIndex);
        }

        if (ch)
        {
            mem = ch->allocate(reqs, shared_from_this());
        }

        return mem;
    }

    void DeviceMemoryPool::freeMemory(common::Resource<DeviceMemory> memory)
    {
        // TODO: fix me, this is a temporary solution to fix double free
        if (memory->mFreed)
        {
            return;
        }
        std::lock_guard lck(mLock);
        OPTICK_EVENT("DeviceMemoryPool::freeMemory");
        for (auto& ch : mMemoryChunks)
        {
            if (ch->isOwner(memory))
            {
                ch->free(memory);
            }
        }
    }

    swizzle::gfx::MemoryStatistics* DeviceMemoryPool::getMemoryStats()
    {
        mStatistics.mName = mMemoryPoolName.c_str();
        mStatistics.mSize = getTotalSize();
        mStatistics.mUsed = getUsedSize();
        mStatistics.mFree = getFreeSize();
        mStatistics.mNumAllocations = (U32)mMemoryChunks.size();
        mStatistics.mNumVirtualAllocations = getVirtAllocs();

        return &mStatistics;
    }
}

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{

    VkDeviceSize MemoryChunk::calculateAlignmentOffset(VkDeviceSize currentOffset, VkDeviceSize requireAlignment) const
    {
        VkDeviceSize alval = requireAlignment - 1u;
        VkDeviceSize alignedValue = (currentOffset + alval) & ~(alval);
        VkDeviceSize newOffset = alignedValue - currentOffset;

        return newOffset;
    }

    common::Resource<MemoryChunk> DeviceMemoryPool::allocateNewChunk(VkDeviceSize size, U32 memoryTypeIndex)
    {
        OPTICK_EVENT("DeviceMemoryPool::allocateNewChunk");

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = VK_NULL_HANDLE;
        allocInfo.allocationSize = size;
        allocInfo.memoryTypeIndex = memoryTypeIndex;

        common::Resource<MemoryChunk> chunk = nullptr;
        VkDeviceMemory mem = VK_NULL_HANDLE;

        VkResult res = vkAllocateMemory(mDevice->getDeviceHandle(), &allocInfo, mDevice->getAllocCallbacks(), &mem);
        if (res == VK_SUCCESS)
        {
            chunk = common::CreateRef<MemoryChunk>(mDevice, mem, memoryTypeIndex, size);
            mMemoryChunks.emplace_back(chunk);
            LOG_INFO("Allocated new memory chunk, %llu", size);
        }
        else
        {
            vk::LogVulkanError(res, "vkAllocateMemory");
        }
        return chunk;
    }

    common::Resource<MemoryChunk> DeviceMemoryPool::getChunk(VkMemoryRequirements memreq, U32 memoryTypeIndex)
    {
        OPTICK_EVENT("DeviceMemoryPool::getChunk");
        common::Resource<MemoryChunk> chunk = nullptr;

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

    U32 DeviceMemoryPool::getVirtAllocs() const
    {
        U32 valloc = 0ul;
        for (const auto& ch : mMemoryChunks)
        {
            valloc += ch->getAllocCount();
        }
        return valloc;
    }

}