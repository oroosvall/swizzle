
/* Include files */

#include <swizzle/profiler/Profiler.hpp>

#include "CmdBuffer.hpp"
#include "CmdPool.hpp"
#include "DBuffer.hpp"
#include "Device.hpp"

#include "Fence.hpp"
#include "QueryPool.hpp"
#include "ShaderPipeline.hpp"
#include "TextureBase.hpp"
#include "VMaterial.hpp"
#include "VSwapchain.hpp"
#include "VkResource.hpp"

#include "cmd/CommandTransaction.hpp"
#include "cmd/DrawCommandTransaction.hpp"

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
    CmdBuffer::CmdBuffer(common::Resource<vk::Device> device, common::Resource<vk::CmdPool> cmdPool, U32 bufferCount)
        : mDevice(device)
        , mCmdPool(cmdPool)
        , mBufferCount(bufferCount)
        , mActiveIndex(0u)
        , mFrameCounter(0u)
        , mCommandBuffers(nullptr)
        , mFences(nullptr)
        , mDrawCount(0u)
        , mCachedGatherPipelineStatistics(false)
    {
        mCommandBuffers =
            mCmdPool->allocateCmdBuffer(mBufferCount, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        mFences = new common::Resource<vk::Fence>[mBufferCount];
        mLifetimeToken = common::CreateRef<LifetimeToken>(0ull, mBufferCount);
    }

    CmdBuffer::~CmdBuffer()
    {
        mCmdPool->freeCmdBuffer(mCommandBuffers, mBufferCount);
        delete[] mFences;
    }

    U64 CmdBuffer::getFrameCount()
    {
        return mFrameCounter;
    }

    SwBool CmdBuffer::isFrameInProgress(U64 frameCount)
    {
        SwBool inFlight = true;

        if ((frameCount + mBufferCount + 1u) < mFrameCounter)
        {
            inFlight = false;
        }

        return inFlight;
    }

    VkCommandBuffer CmdBuffer::getActiveCommandBuffer()
    {
        return getCmdBuffer(mActiveIndex);
    }

    void CmdBuffer::setWaitFence(common::Resource<vk::Fence> fence)
    {
        fence->addUser(shared_from_this());
        mFences[mActiveIndex] = fence;
        incrementActiveIndex();
    }

    U32 CmdBuffer::getDrawCount()
    {
        return mDrawCount;
    }

    U64 CmdBuffer::getVertCount()
    {
        return 0ull;
    }

    U64 CmdBuffer::getTriCount()
    {
        return 0ull;
    }

    void CmdBuffer::reset(bool hardReset)
    {
        if (hardReset)
        {
            VkResult res =
                vkResetCommandBuffer(getCmdBuffer(mActiveIndex),
                                     VkCommandBufferResetFlagBits::VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
            vk::LogVulkanError(res, "vkResetCommandBuffer");
        }
        else
        {
            VkResult res = vkResetCommandBuffer(getCmdBuffer(mActiveIndex), 0u);
            vk::LogVulkanError(res, "vkResetCommandBuffer");
        }
    }

    common::Unique<swizzle::gfx::CommandTransaction> CmdBuffer::begin()
    {
        mDrawCount = 0u;
        SWIZZLE_PROFILE_EVENT("CmdBuffer::begin");
        mFrameCounter++;

        auto fence = getFence(mActiveIndex);
        if (fence)
        {
            VkFence f = fence->getHandle();
            vkWaitForFences(mDevice->getDeviceHandle(), 1u, &f, VK_TRUE, UINT64_MAX);
            fence->removeUser(shared_from_this());
            // VkResult res = vkResetFences(mDevice->getDeviceHandle(), 1U, &fence);
            // vk::LogVulkanError(res, "vkResetFences");
        }

        VkCommandBufferBeginInfo beginInfo = {};

        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = VK_NULL_HANDLE;
        beginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        beginInfo.pInheritanceInfo = VK_NULL_HANDLE;

        /*if (mCommandBufferType == eCmdBufferType::eSecondary)
        {
            VkCommandBufferInheritanceInfo inhertianceInfo;

            inhertianceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
            inhertianceInfo.pNext = VK_NULL_HANDLE;
            inhertianceInfo.renderPass = VK_NULL_HANDLE;
            inhertianceInfo.subpass = 0;
            inhertianceInfo.framebuffer = VK_NULL_HANDLE;
            inhertianceInfo.occlusionQueryEnable = VK_FALSE;
            inhertianceInfo.queryFlags = VkQueryControlFlagBits::VK_QUERY_CONTROL_PRECISE_BIT;
            inhertianceInfo.pipelineStatistics =
                VkQueryPipelineStatisticFlagBits::VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT;

            beginInfo.pInheritanceInfo = &inhertianceInfo;
        }*/

        VkResult res = vkBeginCommandBuffer(getCmdBuffer(mActiveIndex), &beginInfo);
        vk::LogVulkanError(res, "vkBeginCommandBuffer");

        mCachedGatherPipelineStatistics = mDevice->getStatisticsQuery()->isEnabled();

        if (mCachedGatherPipelineStatistics)
        {
            QueryData qd = mDevice->getStatisticsQuery()->getQuery();
            vkCmdResetQueryPool(getCmdBuffer(mActiveIndex), qd.mQuery, 0u, 1u);
            vkCmdBeginQuery(getCmdBuffer(mActiveIndex), qd.mQuery, 0u, 0u);
        }

        if (mDevice->getTimingQuery()->isEnabled())
        {
            QueryData qd = mDevice->getTimingQuery()->getQuery();
            vkCmdResetQueryPool(getCmdBuffer(mActiveIndex), qd.mQuery, 0u, qd.mQueryCount);
            mDevice->getTimingQuery()->resetQuery();
        }

        mLifetimeToken->incrementCheckpoint();
        return common::CreateUnique<VCommandTransaction>(getCmdBuffer(mActiveIndex), mLifetimeToken);
    }

    void CmdBuffer::end(common::Unique<swizzle::gfx::CommandTransaction>&& transaction)
    {
        SWIZZLE_PROFILE_EVENT("CmdBuffer::end");

        transaction.reset();

        if (mCachedGatherPipelineStatistics)
        {
            QueryData qd = mDevice->getStatisticsQuery()->getQuery();
            vkCmdEndQuery(getCmdBuffer(mActiveIndex), qd.mQuery, 0u);
        }

        if (mDevice->getTimingQuery()->isEnabled())
        {
            QueryData qd = mDevice->getTimingQuery()->getQuery();
            vkCmdWriteTimestamp(getCmdBuffer(mActiveIndex),
                                VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                                qd.mQuery, qd.mQueryIndex);
        }

        VkResult res = vkEndCommandBuffer(getCmdBuffer(mActiveIndex));
        vk::LogVulkanError(res, "vkEndCommandBuffer");
    }

    common::Unique<swizzle::gfx::DrawCommandTransaction>
        CmdBuffer::beginRenderPass(common::Resource<swizzle::gfx::Swapchain> swp,
                                   common::Unique<swizzle::gfx::CommandTransaction>&& transaction)
    {
        VSwapchain* swapchain = (VSwapchain*)(swp.get());
        auto fbo = swapchain->getFrameBuffer();
        return beginRenderPass(fbo, std::move(transaction));
    }

    common::Unique<swizzle::gfx::DrawCommandTransaction>
        CmdBuffer::beginRenderPass(common::Resource<swizzle::gfx::FrameBuffer> fbo,
                                   common::Unique<swizzle::gfx::CommandTransaction>&& transaction)
    {
        auto trans = (VCommandTransaction*)transaction.release();
        VkCommandBuffer cmd = trans->getBuffer();
        auto token = trans->getToken();

        delete trans;

        BaseFrameBuffer* present = (BaseFrameBuffer*)(fbo.get());

        std::vector<VkClearValue> clears;
        U32 numAttach = present->getNumColorAttachments();

        for (U32 i = 0u; i < numAttach; ++i)
        {
            clears.push_back(present->getColorClearValue(i));
        }

        if (present->hasDepthAttachment())
        {
            clears.push_back(present->getDepthClearValue());
        }

        VkRenderPassBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.pNext = VK_NULL_HANDLE;
        beginInfo.renderPass = present->getRenderPass();
        beginInfo.framebuffer = present->getFrameBuffer();
        U32 width = 0u;
        U32 height = 0u;
        present->getSize(width, height);
        beginInfo.renderArea = {0u, 0u, width, height};
        beginInfo.clearValueCount = static_cast<U32>(clears.size());
        beginInfo.pClearValues = clears.data();

        vkCmdBeginRenderPass(cmd, &beginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
        if (mDevice->getTimingQuery()->isEnabled())
        {
            QueryData qd = mDevice->getTimingQuery()->getQuery();
            vkCmdWriteTimestamp(cmd, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, qd.mQuery,
                                qd.mQueryIndex);
        }

        return common::CreateUnique<VDrawCommandTransaction>(cmd, token);
    }

    common::Unique<swizzle::gfx::CommandTransaction>
        CmdBuffer::endRenderPass(common::Unique<swizzle::gfx::DrawCommandTransaction>&& transaction)
    {
        auto trans = (VDrawCommandTransaction*)transaction.release();
        VkCommandBuffer cmd = trans->getBuffer();
        auto token = trans->getToken();

        delete trans;

        vkCmdEndRenderPass(cmd);

        return common::CreateUnique<VCommandTransaction>(cmd, token);
    }

} // namespace vk

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{

    VkCommandBuffer CmdBuffer::getCmdBuffer(U32 index)
    {
        return mCommandBuffers[index];
    }

    common::Resource<vk::Fence> CmdBuffer::getFence(U32 index)
    // VkFence CmdBuffer::getFence(U32 index)
    {
        return mFences[index];
    }

    void CmdBuffer::incrementActiveIndex()
    {
        mActiveIndex++;
        if (mActiveIndex == mBufferCount)
        {
            mActiveIndex = 0u;
        }
    }

} // namespace vk
