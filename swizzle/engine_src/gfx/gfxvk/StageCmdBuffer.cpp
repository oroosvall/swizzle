#include "StageCmdBuffer.hpp"

namespace swizzle::gfx
{

    StageCmdBuffer::StageCmdBuffer(const VkDevice& device, const VkCommandPool& cmdPool, const VkQueue& queue, const VkPhysicalDeviceMemoryProperties& deviceMemorProperties)
        : mDevice(device)
        , mCmdPool(cmdPool)
        , mQueue(queue)
        , mCmdBuffer(VK_NULL_HANDLE)
        , mRecordingStarted(false)
        , mStageBuffers()
        , mDeviceMemorProperties(deviceMemorProperties)
    {

        VkCommandBufferAllocateInfo cmdAllocInfo;

        cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdAllocInfo.pNext = VK_NULL_HANDLE;
        cmdAllocInfo.commandPool = mCmdPool;
        cmdAllocInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdAllocInfo.commandBufferCount = 1;

        vkAllocateCommandBuffers(mDevice, &cmdAllocInfo, &mCmdBuffer);
    }

    StageCmdBuffer::~StageCmdBuffer()
    {
        if (mRecordingStarted)
        {
            vkEndCommandBuffer(mCmdBuffer);

            clearStageBuffers();
        }

        vkFreeCommandBuffers(mDevice, mCmdPool, 1, &mCmdBuffer);
    }

    SwBool StageCmdBuffer::readyToSubmit() const
    {
        return mRecordingStarted;
    }

    void StageCmdBuffer::beginStageRecording()
    {
        VkCommandBufferBeginInfo beginInfo;
        beginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = VK_NULL_HANDLE;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = {};

        vkBeginCommandBuffer(mCmdBuffer, &beginInfo);
        mRecordingStarted = true;

        clearStageBuffers();

    }

    void StageCmdBuffer::endAndSubmitRecording()
    {
        vkEndCommandBuffer(mCmdBuffer);

        VkSubmitInfo subinfo;
        subinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        subinfo.pNext = 0;
        subinfo.waitSemaphoreCount = 0;
        subinfo.pWaitSemaphores = VK_NULL_HANDLE;
        subinfo.pWaitDstStageMask = VK_NULL_HANDLE;
        subinfo.commandBufferCount = 1;
        subinfo.pCommandBuffers = &mCmdBuffer;
        subinfo.signalSemaphoreCount = 0;
        subinfo.pSignalSemaphores = VK_NULL_HANDLE;

        vkQueueSubmit(mQueue, 1, &subinfo, VK_NULL_HANDLE);
    }

    StageMemoryBuffer& StageCmdBuffer::allocateStagingMemory(SwU8* data, VkDeviceSize memSize)
    {
        StageMemoryBuffer memoryStuff;

        SwU32 queueIndex = VK_QUEUE_FAMILY_IGNORED;

        VkBufferCreateInfo bufferInfo;
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext = VK_NULL_HANDLE;
        bufferInfo.flags = 0;
        bufferInfo.size = memSize;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.queueFamilyIndexCount = 0;
        bufferInfo.pQueueFamilyIndices = &queueIndex;

        vkCreateBuffer(mDevice, &bufferInfo, nullptr, &memoryStuff.mBuffer);

        VkMemoryRequirements req;
        vkGetBufferMemoryRequirements(mDevice, memoryStuff.mBuffer, &req);

        VkMemoryAllocateInfo allocInfo;
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = VK_NULL_HANDLE;
        allocInfo.allocationSize = req.size;
        allocInfo.memoryTypeIndex = FindMemoryType(mDeviceMemorProperties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, req.memoryTypeBits);

        vkAllocateMemory(mDevice, &allocInfo, nullptr, &memoryStuff.mMemory);

        vkBindBufferMemory(mDevice, memoryStuff.mBuffer, memoryStuff.mMemory, 0U);

        void* dataPtr;
        vkMapMemory(mDevice, memoryStuff.mMemory, 0, memSize, 0, &dataPtr);
        memcpy(dataPtr, data, static_cast<size_t>(memSize));
        vkUnmapMemory(mDevice, memoryStuff.mMemory);

        mStageBuffers.emplace_back(memoryStuff);

        return mStageBuffers.back();
    }

    void StageCmdBuffer::transitionImage(VkImage image)
    {

    }

    void StageCmdBuffer::clearStageBuffers()
    {
        for (auto& stageBuffer : mStageBuffers)
        {
            vkDestroyBuffer(mDevice, stageBuffer.mBuffer, nullptr);
            vkFreeMemory(mDevice, stageBuffer.mMemory, nullptr);
        }

        mStageBuffers.clear();
    }

}