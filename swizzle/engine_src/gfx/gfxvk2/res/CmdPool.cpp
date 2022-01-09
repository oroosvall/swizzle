
/* Include files */

#include "CmdPool.hpp"
#include "Device.hpp"

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
    CmdPool::CmdPool(common::Resource<vk::Device> device, U32 queueFamily)
        : mDevice(device)
        , mCommandPool(VK_NULL_HANDLE)
    {
        VkCommandPoolCreateInfo createInfo{};
        createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        createInfo.queueFamilyIndex = queueFamily;

        VkResult res = vkCreateCommandPool(mDevice->getDeviceHandle(), &createInfo, mDevice->getAllocCallbacks(), &mCommandPool);
        vk::LogVulkanError(res, "vkCreateCommandPool");
    }

    CmdPool::~CmdPool()
    {
        vkDestroyCommandPool(mDevice->getDeviceHandle(), mCommandPool, mDevice->getAllocCallbacks());
    }

    VkCommandBuffer* CmdPool::allocateCmdBuffer(U32 cmdBufferCount, VkCommandBufferLevel level)
    {
        VkCommandBuffer* cmdBuffers = new VkCommandBuffer[cmdBufferCount];

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.pNext = VK_NULL_HANDLE;
        allocInfo.commandPool = mCommandPool;
        allocInfo.level = level;
        allocInfo.commandBufferCount = cmdBufferCount;

        VkResult res = vkAllocateCommandBuffers(mDevice->getDeviceHandle(), &allocInfo, cmdBuffers);
        vk::LogVulkanError(res, "vkAllocateCommandBuffers");

        return cmdBuffers;
    }

    void CmdPool::freeCmdBuffer(VkCommandBuffer* cmdBuffers, U32 cmdBufferCount)
    {
        vkFreeCommandBuffers(mDevice->getDeviceHandle(), mCommandPool, cmdBufferCount, cmdBuffers);
        delete [] cmdBuffers;
    }

    void CmdPool::resetPool()
    {
        vkResetCommandPool(mDevice->getDeviceHandle(), mCommandPool,
                           VkCommandPoolResetFlagBits::VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
    }

}

/* Class Protected Function Definition */

/* Class Private Function Definition */
