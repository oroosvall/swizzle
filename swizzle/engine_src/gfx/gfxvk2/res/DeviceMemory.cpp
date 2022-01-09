
/* Include files */

#include "DeviceMemory.hpp"

#include "VkResource.hpp"
#include "Device.hpp"

#include <algorithm>

#include <optick/optick.h>

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

        if(resource->getType() == ResourceType::BufferResource)
        {
            VkResource<VkBuffer>* res = (VkResource<VkBuffer>*)resource.get();
            vkBindBufferMemory(device->getDeviceHandle(), res->getVkHandle(), mMemory, mOffset + mAlignOffset);
        }
        else if (resource->getType() == ResourceType::ImageResource)
        {
            VkResource<VkImage>* res = (VkResource<VkImage>*)resource.get();
            vkBindImageMemory(device->getDeviceHandle(), res->getVkHandle(), mMemory, mOffset + mAlignOffset);
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
            if(!r.expired())
            {
                userCount++;
            }
        }
        return userCount;
    }

    DeviceMemoryPool::DeviceMemoryPool(common::Resource<Device> device, VkDeviceSize chunkSize)
        : std::enable_shared_from_this<DeviceMemoryPool>()
        , mDevice(device)
        , mMemoryChunks()
        , mChunkSize(chunkSize)
        , mTotalAllocated(0ull)
        , mUsedSize(0ull)
        , mMaxHeapSize(0ull)
        , mOtherUseSize(0ull)
        , mLock()
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

        Chunk* ch = getChunk(reqs.size, memoryTypeIndex);

        if (!ch)
        {
            VkDeviceSize chSize = mChunkSize;
            if (reqs.size > chSize)
            {
                chSize = reqs.size;
            }
            allocateNewChunk(chSize, memoryTypeIndex);
            ch = getChunk(reqs.size, memoryTypeIndex);
        }

        if (ch)
        {
            for (auto& frag : ch->mFreeFragments)
            {
                if (frag.mSize >= reqs.size)
                {
                    mem = common::CreateRef<DeviceMemory>(shared_from_this(), ch->mMemory, frag.mOffset, reqs.size);

                    frag.mSize -= reqs.size;
                    frag.mOffset += reqs.size;
                    mUsedSize += reqs.size;
                    ch->mUsedSize += reqs.size;
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

        /*for (auto& ch : mMemoryChunks)
        {
            if (ch.memoryTypeIndex == memoryTypeIndex)
            {
                for (auto& frag : ch.mFreeFragments)
                {
                    if (frag.mSize > reqs.size)
                    {
                        mem = common::CreateRef<DeviceMemory>(shared_from_this(), ch.mMemory, frag.mOffset, reqs.size);
                        frag.mSize -= reqs.size;
                        frag.mOffset += reqs.size;
                        break;
                    }
                }

                ch.mFreeFragments.erase(
                    std::remove_if(
                        ch.mFreeFragments.begin(),
                        ch.mFreeFragments.end(),
                        [](const Fragment & f) { return f.mSize == 0; }
                    ),
                    ch.mFreeFragments.end());

                if (mem)
                {
                    break;
                }
            }
        }*/

        return mem;
    }

    void DeviceMemoryPool::freeMemory(common::Resource<DeviceMemory> memory)
    {
        std::lock_guard lck(mLock);
        OPTICK_EVENT("DeviceMemoryPool::freeMemory");
        for (auto& ch : mMemoryChunks)
        {
            // find the owner chunk and return the memory there
            if (ch.mMemory == memory->mMemory)
            {
                VkDeviceSize checkOffset = memory->mOffset + memory->mSize;

                for (auto& f : ch.mFreeFragments)
                {
                    if (checkOffset == f.mOffset)
                    {
                        f.mOffset = memory->mOffset;
                        f.mSize += memory->mSize;
                        mUsedSize -= memory->mSize;
                        ch.mUsedSize -= memory->mSize;
                        checkOffset = ~0ull;
                        break;
                    }
                    else if (f.mOffset + f.mSize == memory->mOffset)
                    {
                        f.mSize += memory->mSize;
                        mUsedSize -= memory->mSize;
                        ch.mUsedSize -= memory->mSize;
                        checkOffset = ~0ull;
                        break;
                    }
                    else
                    {
                        // do nothing
                    }
                }

                if (checkOffset != ~0ull)
                {
                    ch.mFreeFragments.emplace_back(Fragment{memory->mOffset, memory->mSize});
                    mUsedSize -= memory->mSize;
                    ch.mUsedSize -= memory->mSize;
                }

                break;
            }
        }
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

        VkResult res =
            vkAllocateMemory(mDevice->getDeviceHandle(), &allocInfo, mDevice->getAllocCallbacks(), &ch.mMemory);
        if (res == VK_SUCCESS)
        {
            mTotalAllocated += size;
            mMemoryChunks.emplace_back(ch);
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
}