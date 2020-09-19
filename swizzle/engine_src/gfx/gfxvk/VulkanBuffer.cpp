#include "VulkanBuffer.hpp"

namespace swizzle::gfx
{

    VulkanBuffer::VulkanBuffer(const VulkanObjectContainer& vkObjects, BufferType type)
        : mVkObjects(vkObjects)
        , mBuffer(VK_NULL_HANDLE)
        , mMemory(VK_NULL_HANDLE)
        , mType(type)
        , mStride(0U)
        , mBufferSize(0)
        , mVertCount(0)
    {
    }

    VulkanBuffer::~VulkanBuffer()
    {
        if (mBuffer)
        {
            vkDestroyBuffer(mVkObjects.mLogicalDevice, mBuffer, nullptr);
            mBuffer = VK_NULL_HANDLE;
        }

        if (mMemory)
        {
            vkFreeMemory(mVkObjects.mLogicalDevice, mMemory, nullptr);
            mMemory = VK_NULL_HANDLE;
        }
    }

    void VulkanBuffer::setBufferData(void* data, SwU64 size, SwU32 stride)
    {
        createOrResize(size);

        mVertCount = (SwU32)(size / (SwU64)stride);

        void* mappedPtr = nullptr;
        vkMapMemory(mVkObjects.mLogicalDevice, mMemory, 0U, mBufferSize, 0U, &mappedPtr);
        memcpy(mappedPtr, data, size);
        vkUnmapMemory(mVkObjects.mLogicalDevice, mMemory);
        mStride = stride;
    }

    void VulkanBuffer::createOrResize(SwU64 newSize)
    {

        if (mBuffer != VK_NULL_HANDLE)
            vkDestroyBuffer(mVkObjects.mLogicalDevice, mBuffer, nullptr);
        if (mMemory)
            vkFreeMemory(mVkObjects.mLogicalDevice, mMemory, nullptr);

        VkBufferUsageFlags usage = 0;
        switch (mType)
        {
        case BufferType::Vertex:
            usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            break;
        case BufferType::Index:
            usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            break;
        default:
            break;
        }

        VkBufferCreateInfo bufferInfo;

        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext = VK_NULL_HANDLE;
        bufferInfo.flags = 0;
        bufferInfo.size = newSize;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.queueFamilyIndexCount = 0;
        bufferInfo.pQueueFamilyIndices = &mVkObjects.mQueueFamilyIndex;

        vkCreateBuffer(mVkObjects.mLogicalDevice, &bufferInfo, nullptr, &mBuffer);

        VkMemoryRequirements req;
        vkGetBufferMemoryRequirements(mVkObjects.mLogicalDevice, mBuffer, &req);

        VkMemoryAllocateInfo allocInfo;
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = VK_NULL_HANDLE;
        allocInfo.allocationSize = req.size;
        allocInfo.memoryTypeIndex = FindMemoryType(mVkObjects.mMemoryProperties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);

        vkAllocateMemory(mVkObjects.mLogicalDevice, &allocInfo, nullptr, &mMemory);

        vkBindBufferMemory(mVkObjects.mLogicalDevice, mBuffer, mMemory, 0U);

        mBufferSize = newSize;
    }

}