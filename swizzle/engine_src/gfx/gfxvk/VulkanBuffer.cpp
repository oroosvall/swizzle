#include "VulkanBuffer.hpp"
#include "backend/VulkanMemory.hpp"

namespace swizzle::gfx
{

    VulkanBuffer::VulkanBuffer(const VulkanObjectContainer& vkObjects, BufferType type)
        : mVkObjects(vkObjects)
        , mBuffer(VK_NULL_HANDLE)
        , mBufferMemory(vkObjects)
        //, mMemory(VK_NULL_HANDLE)
        , mType(type)
        , mStride(0U)
        //, mBufferSize(0)
        , mVertCount(0)
    {
    }

    VulkanBuffer::~VulkanBuffer()
    {
        if (mBuffer)
        {
            vkDestroyBuffer(mVkObjects.mLogicalDevice, mBuffer, mVkObjects.mDebugAllocCallbacks);
            mBuffer = VK_NULL_HANDLE;
        }
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

    void* VulkanBuffer::mapMemory(U64 size)
    {
        createOrResize(size);
        void* mappedPtr = mBufferMemory.mapMemory();
        return mappedPtr;
    }

    void VulkanBuffer::unmapMemory()
    {
        mBufferMemory.unmapMemory();
    }

    void VulkanBuffer::createOrResize(U64 newSize)
    {
        if (newSize < mBufferMemory.getMemorySize())
            return;
        if (mBuffer != VK_NULL_HANDLE)
            vkDestroyBuffer(mVkObjects.mLogicalDevice, mBuffer, mVkObjects.mDebugAllocCallbacks);

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

        VkBufferCreateInfo bufferInfo = {};

        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext = VK_NULL_HANDLE;
        bufferInfo.flags = 0;
        bufferInfo.size = newSize;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.queueFamilyIndexCount = 0;
        bufferInfo.pQueueFamilyIndices = &mVkObjects.mQueueFamilyIndex;

        vkCreateBuffer(mVkObjects.mLogicalDevice, &bufferInfo, mVkObjects.mDebugAllocCallbacks, &mBuffer);

        VkMemoryRequirements req = {};
        vkGetBufferMemoryRequirements(mVkObjects.mLogicalDevice, mBuffer, &req);

        // @ TODO move to device local memory and use staging buffer instead
        mBufferMemory.allocateMemory(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req);

        vkBindBufferMemory(mVkObjects.mLogicalDevice, mBuffer, mBufferMemory.getMemoryHandle(), 0U);

        //mBufferSize = newSize;
    }

}