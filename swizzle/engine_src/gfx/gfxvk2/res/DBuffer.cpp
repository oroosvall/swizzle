
/* Include files */

#include <swizzle/profiler/Profiler.hpp>

#include "CmdBuffer.hpp"
#include "DBuffer.hpp"
#include "Device.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace vk
{
    common::Resource<DBuffer> GetBufferAsDBuffer(common::Resource<swizzle::gfx::GfxBuffer> buffer)
    {
        return std::dynamic_pointer_cast<DBuffer>(buffer);
    }
} // namespace vk

/* Class Public Function Definition */

namespace vk
{

    DBuffer::DBuffer(common::Resource<Device> device, swizzle::gfx::GfxBufferType type,
                     swizzle::gfx::GfxMemoryArea memArea)
        : mDevice(device)
        , mType(type)
        , mMemArea(memArea)
        , mBuffer()
        , mMemory()
        , mStride(0u)
        , mVertCount(0u)
        , mUsedSize(0u)
        , mBufferSize(0u)
    {
    }

    DBuffer::~DBuffer()
    {
        mDevice->scheduleResourceDestruction(mBuffer);
        mDevice->scheduleFreeingMemory(mMemory);
    }

    void DBuffer::setBufferData(void* data, U64 size, U32 stride)
    {
        SWIZZLE_PROFILE_EVENT("DBuffer::setBufferData");

        mVertCount = (U32)(size / (U64)stride);
        mStride = stride;

        void* mappedPtr = mapMemory(size);
        memcpy(mappedPtr, data, size);
        unmapMemory();
    }

    U64 DBuffer::getRemainingSize() const
    {
        return mMemory->mSize - mUsedSize;
    }

    U64 DBuffer::getSize() const
    {
        return mMemory->mSize;
    }

    U64 DBuffer::getUsedSize() const
    {
        return mUsedSize;
    }

    void DBuffer::setStride(U32 stride)
    {
        mStride = stride;
    }

    void* DBuffer::mapMemory(U64 size)
    {
        SWIZZLE_PROFILE_EVENT("DBuffer::mapMemory");
        UNUSED_ARG(size);
        createOrResize(size);
        void* ptr = nullptr;
        if (!mMemory->mFreed)
        {
            VkResult res = vkMapMemory(mDevice->getDeviceHandle(), mMemory->mMemory, mMemory->mAlignOffset,
                                       mMemory->mSize, 0, &ptr);
            vk::LogVulkanError(res, "vkMapMemory, mapping error");
        }
        else
        {
            LOG_ERROR("Unable to map memory that has been freed!");
        }

        // memset(ptr, 0xff, mMemory->mSize);
        // memset(ptr, 0xAA, size);

        return ptr;
    }

    void DBuffer::unmapMemory()
    {
        SWIZZLE_PROFILE_EVENT("DBuffer::unmapMemory");
        // VkMappedMemoryRange rng{};

        // rng.memory = mMemory->mMemory;
        // rng.offset = mMemory->mOffset;
        // rng.size = mMemory->mSize;
        // rng.pNext = VK_NULL_HANDLE;
        // rng.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;

        // vkFlushMappedMemoryRanges(mDevice->getDeviceHandle(), 1, &rng);

        vkUnmapMemory(mDevice->getDeviceHandle(), mMemory->mMemory);
        mVertCount = (U32)(mUsedSize / (U64)mStride);
    }

    void DBuffer::invalidateMappedMemory()
    {
        VkMappedMemoryRange mmr{};
        mmr.memory = mMemory->mMemory;
        mmr.offset = mMemory->mAlignOffset;
        mmr.size = mMemory->mSize;
        mmr.pNext = nullptr;
        mmr.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;

        vkInvalidateMappedMemoryRanges(mDevice->getDeviceHandle(), 1u, &mmr);
    }

    common::Resource<VkResource<VkBuffer>>& DBuffer::getBuffer()
    {
        return mBuffer;
    }

    U32 DBuffer::getCount()
    {
        return mVertCount;
    }

    swizzle::gfx::GfxBufferType DBuffer::getType()
    {
        return mType;
    }

} // namespace vk

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{
    void DBuffer::createOrResize(U64 newSize)
    {
        // this causes artifacting due to memory being reused
        // if (mMemory && newSize <= mBufferSize)
        //{
        //    mUsedSize = newSize;
        //    return;
        //}

        if (mBuffer)
        {
            mDevice->scheduleResourceDestruction(mBuffer);
            mBuffer.reset();
        }

        VkBufferUsageFlags usage = 0;
        switch (mType)
        {
        case swizzle::gfx::GfxBufferType::Vertex:
            usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            break;
        case swizzle::gfx::GfxBufferType::Index:
            usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            break;
        case swizzle::gfx::GfxBufferType::UniformBuffer:
            usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
                    VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                    VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            break;
        case swizzle::gfx::GfxBufferType::StorageBuffer:
            usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            break;
        default:
            break;
        }

        // newSize = (newSize * 110) / 100;

        mBuffer = mDevice->createBuffer(newSize, usage);

        VkMemoryRequirements req = {};
        vkGetBufferMemoryRequirements(mDevice->getDeviceHandle(), mBuffer->getVkHandle(), &req);

        if (mMemory)
        {
            // if (req.size > mMemory->mSize)
            {
                // this free/reallocate can be improved by checking if we can expand the currenly allocated size
                // for now free it and allocate new memory
                mDevice->scheduleFreeingMemory(mMemory);
                mMemory = tryAllocatePreferredMemory(req);
            }
        }
        else
        {
            // @ TODO move to device local memory and use staging buffer instead
            mMemory = tryAllocatePreferredMemory(req);
        }

        if (!mMemory)
        {
            LOG_ERROR("Failed to allocate memory");
        }

        mMemory->bind(mDevice, mBuffer);

        mUsedSize = newSize;
        mBufferSize = newSize;
    }

    common::Resource<DeviceMemory> DBuffer::tryAllocatePreferredMemory(VkMemoryRequirements req)
    {
        SWIZZLE_PROFILE_EVENT("DBuffer::tryAllocatePreferredMemory");

        VkMemoryPropertyFlags flags = 0u;
        if (mMemArea == swizzle::gfx::GfxMemoryArea::DeviceLocal)
        {
            flags = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        }
        else if (mMemArea == swizzle::gfx::GfxMemoryArea::DeviceLocalHostVisible)
        {
            flags = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        }
        else
        {
            flags = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        }

        auto mem = mDevice->allocateMemory(flags, req);

        // We reached OOM for the pool we wanted to use
        if (!mem)
        {
            // For device local and BAR memory pool, Try again with CPU memory pool (very slow, last resort)
            if (flags != VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
            {
                flags = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
                mem = mDevice->allocateMemory(flags, req);
            }
        }

        return mem;
    }

} // namespace vk
