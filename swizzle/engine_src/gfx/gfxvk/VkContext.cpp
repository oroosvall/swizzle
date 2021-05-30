
/* Include files */

#include <swizzle/core/Logging.hpp>

#include "VkContext.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanCmdBuffer.hpp"

#include "VulkanCubeMap.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace swizzle::gfx
{

    VkGfxContext::VkGfxContext(common::Resource<VulkanInstance> vkInstance, U32 deviceIndex)
        : mVkInstance(vkInstance)
        , mPhysicalDevice(VK_NULL_HANDLE)
        , mLogicalDevice(VK_NULL_HANDLE)
    {

        auto devices = mVkInstance->listDevices();
        if (deviceIndex >= devices.size())
        {
            deviceIndex = 0;
        }

        mPhysicalDevice = common::CreateRef<PhysicalDevice>(devices[deviceIndex]);

        auto extensions = mPhysicalDevice->listAvailableExtensions(); // use this to check if an extension is supported
        for (auto& it : extensions)
        {
            LOG_INFO("Found extension %s", it.c_str());
        }
        mPhysicalDevice->activateExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        mPhysicalDevice->getNumQueues();
        std::vector<F32> prios;
        prios.push_back(1.0F);
        mLogicalDevice = mPhysicalDevice->createLogicalDevice(prios);


        mVkContainer.mVkInstance = mVkInstance;
        mVkContainer.mPhysicalDevice = mPhysicalDevice;
        mVkContainer.mLogicalDevice = mLogicalDevice;
        mVkContainer.mDebugAllocCallbacks = nullptr;

        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = 10U;

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
        descriptorPoolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.pNext = VK_NULL_HANDLE;
        descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        descriptorPoolCreateInfo.maxSets = 10U;
        descriptorPoolCreateInfo.poolSizeCount = 1U;
        descriptorPoolCreateInfo.pPoolSizes = &poolSize;

        vkCreateDescriptorPool(mLogicalDevice->getLogical(), &descriptorPoolCreateInfo, mVkContainer.mDebugAllocCallbacks, &mVkContainer.mDescriptorPool);

    }

    VkGfxContext::~VkGfxContext()
    {
        vkDestroyDescriptorPool(mLogicalDevice->getLogical(), mVkContainer.mDescriptorPool, mVkContainer.mDebugAllocCallbacks);
    }

    void VkGfxContext::waitIdle()
    {
        vkDeviceWaitIdle(mLogicalDevice->getLogical());
    }

    GfxStatistics VkGfxContext::getStatistics()
    {
        GfxStatistics stats = {};

        stats.mNumBuffers = mLogicalDevice->getNumBuffers();
        stats.mNumTextures = mLogicalDevice->getNumTextures();

        return stats;
    }

    common::Resource<Buffer> VkGfxContext::createBuffer(BufferType type)
    {
        return common::CreateRef<VulkanBuffer>(mVkContainer, type);
    }

    common::Resource<CommandBuffer> VkGfxContext::createCommandBuffer(U32 swapCount)
    {
        return common::CreateRef<VulkanCommandBuffer>(mVkContainer, swapCount);
    }

    common::Resource<Swapchain> VkGfxContext::createSwapchain(common::Resource<core::SwWindow> window, U32 swapCount)
    {
        UNUSED_ARG(swapCount)
        return common::CreateRef<VulkanSwapchain>(mVkContainer, window);
    }

    common::Resource<Texture> VkGfxContext::createTexture(U32 width, U32 height, U32 channels, const U8* data)
    {
        return common::CreateRef<VulkanTexture>(mVkContainer, width, height, channels, data);
    }

    common::Resource<Texture> VkGfxContext::createCubeMapTexture(U32 width, U32 height, U32 channels, const U8* data)
    {
        return common::CreateRef<VulkanCubeMap>(mVkContainer, width, height, channels, data);
    }

    const SwChar* VkGfxContext::getDeviceName() const
    {
        return mLogicalDevice->getDeviceProperties().deviceName;
    }

    VkContainer VkGfxContext::getVkContainer() const
    {
        return mVkContainer;
    }

}
/* Class Protected Function Definition */

/* Class Private Function Definition */
