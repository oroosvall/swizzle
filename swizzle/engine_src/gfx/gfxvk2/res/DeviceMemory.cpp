
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

    DeviceMemoryPool::DeviceMemoryPool(common::Resource<Device> device, VkDeviceSize chunkSize, std::string memoryPoolName)
        : std::enable_shared_from_this<DeviceMemoryPool>()
        , mDevice(device)
        , mMemoryChunks()
        , mChunkSize(chunkSize)
        , mMemoryPoolName(memoryPoolName)
        , mTotalAllocated(0ull)
        , mUsedSize(0ull)
        , mMaxHeapSize(0ull)
        , mOtherUseSize(0ull)
        , mLock()
        , mVAllocCount(0u)
        , mStatistics()
    {
    }

    DeviceMemoryPool::~DeviceMemoryPool()
    {
        for (auto& ch : mMemoryChunks)
        {
            vkFreeMemory(mDevice->getDeviceHandle(), ch.mMemory, mDevice->getAllocCallbacks());
        }
        mMemoryChunks.clear();
        mTotalAllocated = 0ull;
        mUsedSize = 0ull;
    }

    void DeviceMemoryPool::updateBudget(VkDeviceSize budget, VkDeviceSize usage)
    {
        // TODO: Use heap budget
        UNUSED_ARG(budget);
        UNUSED_ARG(usage);
    }

    VkDeviceSize DeviceMemoryPool::getFreeSize()
    {
        return (mTotalAllocated - mUsedSize);
    }

    VkDeviceSize DeviceMemoryPool::getUsedSize()
    {
        return mUsedSize;
    }

    VkDeviceSize DeviceMemoryPool::getTotalSize()
    {
        return mTotalAllocated;
    }

    common::Resource<DeviceMemory> DeviceMemoryPool::allocateMemory(VkMemoryRequirements reqs, U32 memoryTypeIndex)
    {
        std::lock_guard lck(mLock);

        OPTICK_EVENT("DeviceMemoryPool::allocateMemory");
        // TODO: Needs thread locking for multiple threads
        // TODO: handle alignment from reqs
        common::Resource<DeviceMemory> mem = nullptr;

        AlignmentInfo chunkAlignedSize = calcAlignedSize(reqs.size, reqs);
        Chunk* ch = getChunk(chunkAlignedSize.mSize, memoryTypeIndex);

        if (!ch)
        {
            VkDeviceSize chSize = mChunkSize;
            if (reqs.size > chSize)
            {
                chSize = reqs.size;
                chSize = calcAlignedSize(chSize, reqs).mSize;
            }
            allocateNewChunk(chSize, memoryTypeIndex);
            ch = getChunk(chSize, memoryTypeIndex);
        }

        if (ch)
        {
            for (auto& frag : ch->mFreeFragments)
            {
                AlignmentInfo alignedSize = calcAlignedSize(frag.mSize, reqs);
                if (frag.mSize >= alignedSize.mSize)
                {
                    mem = common::CreateRef<DeviceMemory>(shared_from_this(), ch->mMemory, frag.mOffset + alignedSize.mAlignOffset, alignedSize.mSize);

                    frag.mSize -= mem->mSize;
                    frag.mOffset += mem->mSize;
                    mUsedSize += mem->mSize;
                    ch->mUsedSize += mem->mSize;
                    assert(ch->mUsedSize <= ch->mTotalSize);
                    mVAllocCount++;
                    break;
                }
            }

            ch->mFreeFragments.erase(
                std::remove_if(
                    ch->mFreeFragments.begin(),
                    ch->mFreeFragments.end(),
                    [](const Fragment& f) { return f.mSize == 0; }),
                ch->mFreeFragments.end());
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
            // find the owner chunk and return the memory there
            if (ch.mMemory == memory->mMemory)
            {
                VkDeviceSize checkOffset = memory->mAlignOffset + memory->mSize;

                for (auto& f : ch.mFreeFragments)
                {
                    if (checkOffset == f.mOffset)
                    {
                        f.mOffset = memory->mAlignOffset;
                        f.mSize += memory->mSize;
                        mUsedSize -= memory->mSize;
                        ch.mUsedSize -= memory->mSize;
                        mVAllocCount--;
                        checkOffset = ~0ull;
                        memory->mFreed = true;
                        break;
                    }
                    else if (f.mOffset + f.mSize == memory->mAlignOffset)
                    {
                        f.mSize += memory->mSize;
                        mUsedSize -= memory->mSize;
                        ch.mUsedSize -= memory->mSize;
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

                if (checkOffset != ~0ull)
                {
                    ch.mFreeFragments.emplace_back(Fragment{memory->mAlignOffset, memory->mSize});
                    mUsedSize -= memory->mSize;
                    ch.mUsedSize -= memory->mSize;
                    mVAllocCount--;
                    memory->mFreed = true;
                }

                break;
            }
        }
    }

    swizzle::gfx::MemoryStatistics* DeviceMemoryPool::getMemoryStats()
    {
        mStatistics.mName = mMemoryPoolName.c_str();
        mStatistics.mSize = mTotalAllocated;
        mStatistics.mUsed = mUsedSize;
        mStatistics.mFree = mTotalAllocated - mUsedSize;
        mStatistics.mNumAllocations = (U32)mMemoryChunks.size();
        mStatistics.mNumVirtualAllocations = mVAllocCount;

        return &mStatistics;
    }
}

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{
    void DeviceMemoryPool::allocateNewChunk(VkDeviceSize size, U32 memoryTypeIndex)
    {
        OPTICK_EVENT("DeviceMemoryPool::allocateNewChunk");
        Chunk ch{};
        ch.mMemory = VK_NULL_HANDLE;
        ch.mTotalSize = size;
        ch.mUsedSize = 0ull;
        ch.memoryTypeIndex = memoryTypeIndex;
        ch.mFreeFragments.emplace_back(Fragment{0ull, size});

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = VK_NULL_HANDLE;
        allocInfo.allocationSize = size;
        allocInfo.memoryTypeIndex = memoryTypeIndex;

        VkResult res = vkAllocateMemory(mDevice->getDeviceHandle(), &allocInfo, mDevice->getAllocCallbacks(), &ch.mMemory);
        if (res == VK_SUCCESS)
        {
            mTotalAllocated += size;
            mMemoryChunks.emplace_back(ch);
            LOG_INFO("Allocated new memory chunk, %llu", size);
        }
        else
        {
            vk::LogVulkanError(res, "vkAllocateMemory");
        }

    }

    Chunk* DeviceMemoryPool::getChunk(VkDeviceSize size, U32 memoryTypeIndex)
    {
        OPTICK_EVENT("DeviceMemoryPool::getChunk");
        Chunk* chunk = nullptr;

        for (auto& ch : mMemoryChunks)
        {
            if (ch.memoryTypeIndex == memoryTypeIndex)
            {
                for (auto& frag : ch.mFreeFragments)
                {
                    if (frag.mSize >= size)
                    {
                        chunk = &ch;
                        break;
                    }
                }
                if (chunk)
                {
                    break;
                }
            }
        }

        return chunk;
    }

    AlignmentInfo DeviceMemoryPool::calcAlignedSize(VkDeviceSize offset, VkMemoryRequirements memreq)
    {
        VkDeviceSize alval = memreq.alignment - 1u;
        VkDeviceSize alignedValue = (offset + alval) & ~(alval);

        VkDeviceSize newOffset = alignedValue - offset;
        VkDeviceSize newSize = memreq.size + newOffset;

        return { newSize, newOffset };

    }

}