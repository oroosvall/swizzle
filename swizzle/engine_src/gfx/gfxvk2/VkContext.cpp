
/* Include files */

#include <swizzle/core/Logging.hpp>

#include "VkContext.hpp"
#include "res/DBuffer.hpp"
#include "res/VSwapchain.hpp"
#include "res/Texture2D.hpp"
#include "res/TextureCube.hpp"

#include <optick/optick.h>

//#include "VulkanBuffer.hpp"
//#include "VulkanSwapchain.hpp"
//#include "VulkanCmdBuffer.hpp"
//
//#include "VulkanCubeMap.hpp"

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
    VkGfxContext::VkGfxContext(const GfxContextCreateInfo& createInfo)
        : mVkInstance(nullptr)
        , mVkDevice(nullptr)
        , mCmdPool(nullptr)
        , mQueue(VK_NULL_HANDLE)
        , mFrameAllocator(sizeof(void*) * 128u)
    {
        mVkInstance = common::CreateRef<vk::Instance>(createInfo);
    }

    VkGfxContext::~VkGfxContext()
    {
        mFences.clear();
        mVkInstance.reset();
    }

    U32 VkGfxContext::getDeviceCount()
    {
        return mVkInstance->getNumDevices();
    }

    const SwChar* VkGfxContext::getDeviceName(U32 deviceIndex)
    {
        OPTICK_EVENT("VkGfxContext::getDeviceName");
        return mVkInstance->getDeviceName(deviceIndex);
    }

    const SwChar* VkGfxContext::getSelectedDeviceName()
    {
        const SwChar* name = "";
        if (mVkDevice)
        {
            name = mVkDevice->getDeviceName();
        }
        return name;
    }

    SwBool VkGfxContext::initializeDevice(const GfxContextInitializeInfo& createInfo)
    {
        std::vector<const char*> extensions;

        auto supportedExtensions = mVkInstance->listAvailableExtensions(createInfo.mDeviceIndex);

        addExtensionIfExisting(extensions, supportedExtensions, VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
        addExtensionIfExisting(extensions, supportedExtensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        LOG_INFO("Initializing Vulkan Device");
        mVkDevice = mVkInstance->initializeDevice(createInfo, extensions);

        SwBool ok = false;
        if (mVkDevice)
        {
            mCmdPool = common::CreateRef<vk::CmdPool>(mVkDevice, 0);
            //mQueue = mVkDevice->getQueue();

            if (mCmdPool)
            {
                ok = true;
            }
        }

        return ok;
    }

    void VkGfxContext::waitIdle()
    {
        if (mVkDevice)
        {
            mVkDevice->waitDeviceIdle();
        }
    }

    GfxStatistics VkGfxContext::getStatistics()
    {
        OPTICK_EVENT("VkGfxContext::getStatistics()");
        GfxStatistics stats{};

        stats.mGpuMemoryUsage = mVkDevice->getGPUMemUsage();
        stats.mCpuMemoryUsage = mVkDevice->getCPUMemUsage();

        stats.mNumBuffers = mVkDevice->getBufferCount();
        stats.mNumTextures = mVkDevice->getImageCount();

        return stats;
    }

    common::Resource<Buffer> VkGfxContext::createBuffer(BufferType type)
    {
        return common::CreateRef<vk::DBuffer>(mVkDevice, type);
    }

    common::Resource<CommandBuffer> VkGfxContext::createCommandBuffer(U32 swapCount)
    {
        return common::CreateRef<vk::CmdBuffer>(mVkDevice, mCmdPool, swapCount);
    }

    common::Resource<Swapchain> VkGfxContext::createSwapchain(common::Resource<core::SwWindow> window, U32 swapCount)
    {
        UNUSED_ARG(swapCount);
        return common::CreateRef<vk::VSwapchain>(mVkInstance, mVkDevice, window);
    }

    common::Resource<Texture> VkGfxContext::createTexture(U32 width, U32 height, U32 channels, const U8* data)
    {
        return common::CreateRef<vk::Texture2D>(mVkDevice, width, height, channels, data);
    }

    common::Resource<Texture> VkGfxContext::createCubeMapTexture(U32 width, U32 height, U32 channels, const U8* data)
    {
        return common::CreateRef<vk::TextureCube>(mVkDevice, width, height, channels, data);
    }

    void VkGfxContext::submit(common::Resource<CommandBuffer>* cmdBuffer, U32 cmdBufferCount, common::Resource<Swapchain> swapchain)
    {
        OPTICK_EVENT("VkGfxContext::submit");
        UNUSED_ARG(cmdBuffer);
        UNUSED_ARG(cmdBufferCount);
        UNUSED_ARG(swapchain);

        mFrameAllocator.reset();

        VkCommandBuffer* buffers = (VkCommandBuffer*)mFrameAllocator.allocate(sizeof(VkCommandBuffer) * cmdBufferCount);

        if (buffers)
        {
            common::Resource<vk::Fence> fence = getFence();

            for (U32 i = 0u; i < cmdBufferCount; ++i)
            {
                auto buf = (vk::CmdBuffer*)cmdBuffer[i].get();
                buffers[i] = buf->getActiveCommandBuffer();
                buf->setWaitFence(fence);
            }

            U32 signalCount = 0u;
            VkSemaphore signalSemaphores = VK_NULL_HANDLE;
            VkPipelineStageFlags waitStageMask{};

            if (swapchain)
            {
                vk::VSwapchain* swp = (vk::VSwapchain*)(swapchain.get());
                signalSemaphores = swp->getSemaphoreToSignal();
                waitStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                // wait = swapchain->getWaitForSemaphore();
                signalCount = 1U;

                VkFence waitFence = swp->getWaitFence();
                vkResetFences(mVkDevice->getDeviceHandle(), 1, &waitFence);
            }

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pNext = VK_NULL_HANDLE;
            submitInfo.waitSemaphoreCount = 0u;
            submitInfo.pWaitSemaphores = VK_NULL_HANDLE;
            submitInfo.pWaitDstStageMask = &waitStageMask;
            submitInfo.commandBufferCount = cmdBufferCount;
            submitInfo.pCommandBuffers = buffers;
            submitInfo.signalSemaphoreCount = signalCount;
            submitInfo.pSignalSemaphores = &signalSemaphores;

            vkQueueSubmit(mVkDevice->getQueue(), 1u, &submitInfo, fence->getHandle());
        }
        else
        {
            LOG_ERROR("Failed to allocate frame memory");
        }

        //mVkDevice->performCleanup();
        //mVkDevice->freeOldMemoryPools();

    }

    void VkGfxContext::resetCommandPool()
    {
        mCmdPool->resetPool();
    }

    void VkGfxContext::updateHeapBudget()
    {
        mVkDevice->updateHeapBudget();
    }

}
/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace swizzle::gfx
{
    void VkGfxContext::addExtensionIfExisting(std::vector<const char*>& extensions,
                                               const std::vector<std::string>& availableExtensions,
                                               const char* extensionName)
    {
        for (const auto& ext : availableExtensions)
        {
            if (ext == extensionName)
            {
                extensions.emplace_back(extensionName);
            }
        }

    }

    common::Resource<vk::Fence> VkGfxContext::getFence()
    {
        OPTICK_EVENT("VkGfxContext::getFence()");

        U64 index = ~0u;

        for (U64 i = 0u; i < mFences.size(); ++i)
        {
            if (!mFences[i]->isInUse())
            {
                mFences[i]->resetFence();
                index = i;
                break;
            }
        }


        if (index == ~0u)
        {
            index = mFences.size();
            mFences.emplace_back(common::CreateRef<vk::Fence>(mVkDevice));
        }

        return mFences[index];
    }

}
