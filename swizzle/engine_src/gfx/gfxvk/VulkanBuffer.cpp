#include "VulkanDevice.hpp"
#include "VulkanBuffer.hpp"

#include "backend/VulkanMemory.hpp"

#include <cstring>

namespace swizzle::gfx
{

    VulkanBuffer::VulkanBuffer(const VkContainer vkObjects, BufferType type)
        : mVkObjects(vkObjects)
        , mType(type)
        , mBuffer(VK_NULL_HANDLE)
        , mBufferMemory(vkObjects)
        , mStride(0u)
        , mVertCount(0u)
        , mUsedSize(0u)
    {
    }

    VulkanBuffer::~VulkanBuffer()
    {
        mBuffer.reset();
    }

    void VulkanBuffer::setBufferData(void* data, U64 size, U32 stride)
    {
        createOrResize(size);

        mVertCount = (U32)(size / (U64)stride);

        void* mappedPtr = mBufferMemory.mapMemory();
        memcpy(mappedPtr, data, size);
        mBufferMemory.unmapMemory();
        mStride = stride;
    }

    U64 VulkanBuffer::getRemainingSize() const
    {
        return mBufferMemory.getMemorySize() - mUsedSize;
    }

    void VulkanBuffer::setStride(U32 stride)
    {
        mStride = stride;
    }

    void* VulkanBuffer::mapMemory(U64 size)
    {
        createOrResize(size);
        void* mappedPtr = mBufferMemory.mapMemory();
        return mappedPtr;
    }

    void VulkanBuffer::unmapMemory()
    {
        mBufferMemory.unmapMemory();
        mVertCount = (U32)(mUsedSize / (U64)mStride);
    }

    void VulkanBuffer::createOrResize(U64 newSize)
    {
        if (newSize <= mBufferMemory.getMemorySize())
        {
            mUsedSize = newSize;
            return;
        }

        if (mBuffer)
        {
            mBuffer.reset();
        }

        VkBufferUsageFlags usage = 0;
        switch (mType)
        {
        case BufferType::Vertex:
            usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            break;
        case BufferType::Index:
            usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            break;
        case BufferType::UniformBuffer:
            usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            break;
        default:
            break;
        }

        newSize = (newSize * 110) / 100;

        mBuffer = mVkObjects.mLogicalDevice->createBuffer(newSize, usage);

        VkMemoryRequirements req = {};
        vkGetBufferMemoryRequirements(mVkObjects.mLogicalDevice->getLogical(), mBuffer.get()->mBuffer, &req);

        // @ TODO move to device local memory and use staging buffer instead
        mBufferMemory.allocateMemory(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req);
        //mBufferMemory.allocateMemory(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req);
        //mBufferMemory.allocateMemory(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req);

        vkBindBufferMemory(mVkObjects.mLogicalDevice->getLogical(), mBuffer.get()->mBuffer, mBufferMemory.getMemoryHandle(), 0U);

        mUsedSize = newSize;
        //mBufferSize = newSize;
    }

}