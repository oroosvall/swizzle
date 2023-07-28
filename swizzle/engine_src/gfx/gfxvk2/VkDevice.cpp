
/* Include files */

#include <swizzle/core/Logging.hpp>
#include <swizzle/profiler/Profiler.hpp>

#include "Stats.hpp"
#include "VkDevice.hpp"

#include "res/DBuffer.hpp"
#include "res/QueryPool.hpp"
#include "res/ShaderPipeline.hpp"
#include "res/Texture2D.hpp"
#include "res/TextureCube.hpp"
#include "res/VFrameBuffer.hpp"
#include "res/VMaterial.hpp"
#include "res/VSwapchain.hpp"


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
    VkGfxDevice::VkGfxDevice(common::Resource<vk::Instance> instance, common::Resource<vk::Device> device)
        : mVkInstance(instance)
        , mVkDevice(device)
        , mCmdPool(nullptr)
        , mQueue(VK_NULL_HANDLE)
        , mFrameAllocator(sizeof(void*) * 128u)
        , mMeshShaderSupported(false)
    {
        mCmdPool = common::CreateRef<vk::CmdPool>(mVkDevice, 0);
    }

    VkGfxDevice::~VkGfxDevice()
    {
        mVkDevice->shutdown();

        mFences.clear();
        mCmdPool.reset();
        mVkDevice.reset();
        mVkInstance.reset();
    }

    SwBool VkGfxDevice::hasMeshShaderSupport()
    {
        return mMeshShaderSupported;
    }

    void VkGfxDevice::waitIdle()
    {
        if (mVkDevice)
        {
            mVkDevice->waitDeviceIdle();
        }
    }

    const SwChar* VkGfxDevice::getDeviceName()
    {
        return mVkDevice->getDeviceName();
    }

    GfxStatistics VkGfxDevice::getStatistics()
    {
        SWIZZLE_PROFILE_EVENT("VkGfxDevice::getStatistics()");
        GfxStatistics stats{};

        stats.mGpuMemoryUsage = mVkDevice->getGPUMemUsage();
        stats.mCpuMemoryUsage = mVkDevice->getCPUMemUsage();

        stats.mNumBuffers = mVkDevice->getBufferCount();
        stats.mNumTextures = mVkDevice->getImageCount();

        return stats;
    }

    common::Resource<swizzle::core::StatisticsIterator<GfxStatsType>> VkGfxDevice::getStatisticsIterator()
    {
        SWIZZLE_PROFILE_EVENT("VkGfxDevice::getStatisticsIterator()");
        auto stats = common::CreateRef<StatsIterator>(mVkDevice);
        stats->addMemoryStats();
        stats->addDeviceStats();
        stats->addInstanceStats();
        stats->addPipelineStats();
        stats->addTimingStats();
        return stats;
    }

    common::Resource<GfxBuffer> VkGfxDevice::createBuffer(GfxBufferType type, GfxMemoryArea memArea)
    {
        return common::CreateRef<vk::DBuffer>(mVkDevice, type, memArea);
    }

    common::Resource<CommandBuffer> VkGfxDevice::createCommandBuffer(U32 swapCount)
    {
        return common::CreateRef<vk::CmdBuffer>(mVkDevice, mCmdPool, swapCount);
    }

    common::Resource<Swapchain> VkGfxDevice::createSwapchain(common::Resource<core::SwWindow> window, U32 swapCount, SwBool srgb)
    {
        UNUSED_ARG(swapCount);
        return common::CreateRef<vk::VSwapchain>(mVkInstance, mVkDevice, window, srgb);
    }

    common::Resource<Texture> VkGfxDevice::createTexture(U32 width, U32 height, U32 channels, SwBool f32)
    {
        return common::CreateRef<vk::Texture2D>(mVkDevice, width, height, channels, f32);
    }

    common::Resource<Texture> VkGfxDevice::createCubeMapTexture(U32 width, U32 height, U32 channels)
    {
        return common::CreateRef<vk::TextureCube>(mVkDevice, width, height, channels);
    }

    common::Resource<FrameBuffer> VkGfxDevice::createFramebuffer(const FrameBufferCreateInfo& fboInfo)
    {
        return common::CreateRef<vk::VFrameBuffer>(mVkDevice, fboInfo);
    }

    common::Resource<Shader> VkGfxDevice::createShader(common::Resource<FrameBuffer> framebuffer,
                                                       const swizzle::gfx::ShaderType type,
                                                       const ShaderAttributeList& attribs)
    {
        common::Resource<vk::BaseFrameBuffer> fbo = vk::GetFrameBufferAsBaseFrameBuffer(framebuffer);
        return common::CreateRef<vk::ShaderPipeline>(mVkDevice, type, fbo, attribs);
    }
    common::Resource<Shader> VkGfxDevice::createShader(common::Resource<Swapchain> swapchain,
                                                       const swizzle::gfx::ShaderType type,
                                                       const ShaderAttributeList& attribs)
    {
        vk::VSwapchain* swp = (vk::VSwapchain*)swapchain.get();
        common::Resource<vk::BaseFrameBuffer> fbo = vk::GetFrameBufferAsBaseFrameBuffer(swp->getFrameBuffer());
        return common::CreateRef<vk::ShaderPipeline>(mVkDevice, type, fbo, attribs);
    }

    common::Resource<Material> VkGfxDevice::createMaterial(common::Resource<Shader> shader, SamplerMode samplerMode)
    {
        return common::CreateRef<vk::VMaterial>(mVkDevice, shader, samplerMode);
    }

    void VkGfxDevice::enablePipelineStatistics(SwBool enable)
    {
        mVkDevice->getStatisticsQuery()->enable(enable);
    }

    void VkGfxDevice::enableGpuTiming(SwBool enable)
    {
        mVkDevice->getTimingQuery()->enable(enable);
    }

    void VkGfxDevice::submit(common::Resource<CommandBuffer>* cmdBuffer, U32 cmdBufferCount,
                             common::Resource<Swapchain> swapchain)
    {
        SWIZZLE_PROFILE_EVENT("VkGfxDevice::submit");
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
            VkSemaphore waitSem = VK_NULL_HANDLE;
            U32 waitCount = 0u;

            if (swapchain)
            {
                vk::VSwapchain* swp = (vk::VSwapchain*)(swapchain.get());
                signalSemaphores = swp->getSemaphoreToSignal();
                waitStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                waitSem = swp->getWaitForSemaphore();
                waitCount = 1u;
                signalCount = 1u;

                VkFence waitFence = swp->getWaitFence();
                vkResetFences(mVkDevice->getDeviceHandle(), 1, &waitFence);
            }

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pNext = VK_NULL_HANDLE;
            submitInfo.waitSemaphoreCount = waitCount;
            submitInfo.pWaitSemaphores = &waitSem;
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

        mVkDevice->performCleanup();
        // mVkDevice->freeOldMemoryPools();
    }

    void VkGfxDevice::resetCommandPool()
    {
        mCmdPool->resetPool();
    }

    void VkGfxDevice::updateHeapBudget()
    {
        mVkDevice->updateHeapBudget();
    }

} // namespace swizzle::gfx
/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace swizzle::gfx
{
    SwBool VkGfxDevice::addExtensionIfExisting(std::vector<const char*>& extensions,
                                               const std::vector<std::string>& availableExtensions,
                                               const char* extensionName)
    {
        for (const auto& ext : availableExtensions)
        {
            if (ext == extensionName)
            {
                extensions.emplace_back(extensionName);
                return true;
            }
        }
        return false;
    }

    common::Resource<vk::Fence> VkGfxDevice::getFence()
    {
        SWIZZLE_PROFILE_EVENT("VkGfxDevice::getFence()");

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

} // namespace swizzle::gfx
