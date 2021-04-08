
/* Include files */

#include <swizzle/core/Logging.hpp>

#include "VkContext.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanCmdBuffer.hpp"

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

    VkGfxContext::VkGfxContext(core::Resource<VulkanInstance> vkInstance, U32 deviceIndex)
        : mVkInstance(vkInstance)
        , mPhysicalDevice(VK_NULL_HANDLE)
        , mLogicalDevice(VK_NULL_HANDLE)
    {
        deviceIndex; // TODO: use this

        auto devices = mVkInstance->listDevices();

        mPhysicalDevice = core::CreateRef<PhysicalDevice>(devices[0]);

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

    }

    void VkGfxContext::waitIdle()
    {
        vkDeviceWaitIdle(mLogicalDevice->getLogical());
    }

    GfxStatistics VkGfxContext::getStatistics()
    {
        return {};
    }

    void VkGfxContext::resetFrameCount()
    {

    }

    U32 VkGfxContext::getFrameCount()
    {
        return 0ULL;
    }

    core::Resource<Buffer> VkGfxContext::createBuffer(BufferType type)
    {
        return core::CreateRef<VulkanBuffer>(mVkContainer, type);
    }

    core::Resource<CommandBuffer> VkGfxContext::createCommandBuffer(U32 swapCount)
    {
        return core::CreateRef<VulkanCommandBuffer>(mVkContainer, swapCount);
    }

    core::Resource<Swapchain> VkGfxContext::createSwapchain(core::Resource<core::Window> window, U32 swapCount)
    {
        swapCount;
        return core::CreateRef<VulkanSwapchain>(mVkContainer, window);
    }

    core::Resource<Texture> VkGfxContext::createTexture(U32 width, U32 height)
    {
        return core::CreateRef<VulkanTexture>(mVkContainer, width, height);
    }

    VkContainer VkGfxContext::getVkContainer() const
    {
        return mVkContainer;
    }

}
/* Class Protected Function Definition */

/* Class Private Function Definition */
