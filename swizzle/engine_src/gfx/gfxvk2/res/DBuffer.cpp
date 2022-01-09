
/* Include files */

#include "DBuffer.hpp"
#include "Device.hpp"
#include "CmdBuffer.hpp"

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

    DBuffer::DBuffer(common::Resource<Device> device, swizzle::gfx::BufferType type)
        : mDevice(device)
        , mType(type)
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
        mDevice->sheduleResourceDestruction(mBuffer);
        mDevice->sheduleFreeingMemory(mMemory);
    }

    void DBuffer::setBufferData(void* data, U64 size, U32 stride)
    {
        createOrResize(size);

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

    void DBuffer::setStride(U32 stride)
    {
        mStride = stride;
    }

    void* DBuffer::mapMemory(U64 size)
    {
        UNUSED_ARG(size);
        createOrResize(size);
        void* ptr = nullptr;
        vkMapMemory(mDevice->getDeviceHandle(), mMemory->mMemory, mMemory->mOffset, mMemory->mSize, 0, &ptr);

        return ptr;
    }

    void DBuffer::unmapMemory()
    {
        vkUnmapMemory(mDevice->getDeviceHandle(), mMemory->mMemory);
        mVertCount = (U32)(mUsedSize / (U64)mStride);
    }

    common::Resource<VkResource<VkBuffer>>& DBuffer::getBuffer()
    {
        return mBuffer;
    }

    U32 DBuffer::getVertexCount()
    {
        return mVertCount;
    }

}

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{
    void DBuffer::createOrResize(U64 newSize)
    {
        if (mMemory && newSize <= mBufferSize)
        {
            mUsedSize = newSize;
            return;
        }

        if (mBuffer)
        {
            mDevice->sheduleResourceDestruction(mBuffer);
            mBuffer.reset();
        }

        VkBufferUsageFlags usage = 0;
        switch (mType)
        {
        case swizzle::gfx::BufferType::Vertex :
            usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            break;
        case swizzle::gfx::BufferType::Index:
            usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            break;
        case swizzle::gfx::BufferType::UniformBuffer:
            usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            break;
        default:
            break;
        }

        //newSize = (newSize * 110) / 100;

        mBuffer = mDevice->createBuffer(newSize, usage);

        VkMemoryRequirements req = {};
        vkGetBufferMemoryRequirements(mDevice->getDeviceHandle(), mBuffer->getVkHandle(), &req);

        if (mMemory)
        {
            // this free/reallocate can be improved by checking if we can expand the currenly allocated size
            // for now free it and allocate new memory
            mDevice->sheduleFreeingMemory(mMemory);
        }
        // @ TODO move to device local memory and use staging buffer instead
        mMemory = mDevice->allocateMemory(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                              VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                          req);

        mMemory->bind(mDevice, mBuffer);

        mUsedSize = newSize;
        mBufferSize = newSize;
    }
}
