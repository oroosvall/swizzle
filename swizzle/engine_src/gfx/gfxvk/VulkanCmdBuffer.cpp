
#include <common/Types.hpp>

#include "VulkanDevice.hpp"

#include "VulkanCmdBuffer.hpp"
#include "VulkanSwapchain.hpp"
#include "shader/VulkanShader.hpp"
#include "shader/VulkanMaterial.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanTexture.hpp"

#include <swizzle/core/Logging.hpp>

namespace swizzle::gfx
{

    VulkanCommandBuffer::VulkanCommandBuffer(const VkContainer vkObjects, U32 swapCount)
        : mVkObjects(vkObjects)
        , mSwapCount(swapCount)
        , mCmdBuffers()
        , mActiveIndex(0u)
        , mDrawCount(0u)
        , mVertCount(0u)
        , mTriCount(0u)
    {
        createVkResources();
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        for (U32 i = 0u; i < mSwapCount; ++i)
        {
            vkDestroyFence(mVkObjects.mLogicalDevice->getLogical(), mCmdBuffers[i].mFence, mVkObjects.mDebugAllocCallbacks);
            vkFreeCommandBuffers(mVkObjects.mLogicalDevice->getLogical(), mVkObjects.mLogicalDevice->getCommandPool(), 1, &mCmdBuffers[i].mCmdBuffer);
            mCmdBuffers[i].mBufferReferences.clear();
        }

    }

    U32 VulkanCommandBuffer::getDrawCount()
    {
        return mDrawCount;
    }

    U64 VulkanCommandBuffer::getVertCount()
    {
        return mVertCount;
    }

    U64 VulkanCommandBuffer::getTriCount()
    {
        return mTriCount;
    }

    void VulkanCommandBuffer::reset(bool hardReset)
    {
        if (hardReset)
        {
            vkResetCommandBuffer(getCmdBuffer(mActiveIndex).mCmdBuffer,
                VkCommandBufferResetFlagBits::VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        }
        else
        {
            vkResetCommandBuffer(getCmdBuffer(mActiveIndex).mCmdBuffer, 0);
        }
        getCmdBuffer(mActiveIndex).mBufferReferences.clear();
    }

    void VulkanCommandBuffer::begin()
    {
        mDrawCount = 0u;
        mVertCount = 0u;
        mTriCount = 0u;

        auto fence = getCmdBuffer(mActiveIndex).mFence;

        vkWaitForFences(mVkObjects.mLogicalDevice->getLogical(), 1U, &fence, VK_TRUE, UINT64_MAX);
        vkResetFences(mVkObjects.mLogicalDevice->getLogical(), 1U, &fence);

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

        vkBeginCommandBuffer(getCmdBuffer(mActiveIndex).mCmdBuffer, &beginInfo);
    }

    void VulkanCommandBuffer::end()
    {
        vkEndCommandBuffer(getCmdBuffer(mActiveIndex).mCmdBuffer);
    }

    void VulkanCommandBuffer::uploadTexture(core::Resource<Texture> texture)
    {
        VulkanTextureIfc* tex = (VulkanTextureIfc*)(texture.get());
        if (!tex->isUploaded())
        {
            tex->uploadImage(getCmdBuffer(mActiveIndex).mCmdBuffer);
        }
    }

    void VulkanCommandBuffer::submit(core::Resource<Swapchain> swp)
    {
        U32 waitCount = 0U;
        VkSemaphore renderComplete = VK_NULL_HANDLE;
        VkSemaphore wait = VK_NULL_HANDLE;
        VkPipelineStageFlags waitStageMask = VK_NULL_HANDLE;
        VkFence waitFence = VK_NULL_HANDLE;

        if (swp)
        {
            VulkanSwapchain* swapchain = (VulkanSwapchain*)(swp.get());
            renderComplete = swapchain->getSemaphoreToSignal();
            wait = swapchain->getWaitForSemaphore();
            waitStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            waitFence = swapchain->getWaitFence();
            waitCount = 1U;

            vkResetFences(mVkObjects.mLogicalDevice->getLogical(), 1, &waitFence);
        }

        // TODO: move into one sumbit call
        /*if (mVkObjects.stageCmdBuffer->readyToSubmit())
        {
            mVkObjects.stageCmdBuffer->endAndSubmitRecording();
        }*/

        auto cmdBuffer = getCmdBuffer(mActiveIndex).mCmdBuffer;
        auto fence = getCmdBuffer(mActiveIndex).mFence;

        VkSubmitInfo subinfo = {};
        subinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        subinfo.pNext = VK_NULL_HANDLE;
        subinfo.waitSemaphoreCount = 0u;
        subinfo.pWaitSemaphores = &wait;
        subinfo.pWaitDstStageMask = &waitStageMask;
        subinfo.commandBufferCount = 1U;
        subinfo.pCommandBuffers = &cmdBuffer;
        subinfo.signalSemaphoreCount = waitCount;
        subinfo.pSignalSemaphores = &renderComplete;

        vkQueueSubmit(mVkObjects.mLogicalDevice->getQueue(0, 0), 1U, &subinfo, fence);

        mActiveIndex++;
        if (mActiveIndex == mCmdBuffers.size())
        {
            mActiveIndex = 0u;
        }

    }

    void VulkanCommandBuffer::beginRenderPass(core::Resource<Swapchain> swp)
    {
        VulkanSwapchain* swapchain = (VulkanSwapchain*)(swp.get());
        beginRenderPass(swapchain->getFrameBuffer());
    }

    void VulkanCommandBuffer::beginRenderPass(core::Resource<FrameBuffer> fbo)
    {
        PresentFBO* present = (PresentFBO*)(fbo.get());

        VkClearValue clears[] = { present->mClearValue , present->mDepthClearValue };

        VkRenderPassBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.pNext = VK_NULL_HANDLE;
        beginInfo.renderPass = present->mRenderPass;
        beginInfo.framebuffer = present->mFrameBuffer;
        beginInfo.renderArea = { 0U, 0U, present->mWidth, present->mHeight };
        beginInfo.clearValueCount = 2U;
        beginInfo.pClearValues = clears;

        vkCmdBeginRenderPass(getCmdBuffer(mActiveIndex).mCmdBuffer, &beginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanCommandBuffer::endRenderPass()
    {
        vkCmdEndRenderPass(getCmdBuffer(mActiveIndex).mCmdBuffer);
    }

    void VulkanCommandBuffer::bindShader(core::Resource<Shader> shader)
    {
        VulkanShader* shad = (VulkanShader*)(shader.get());

        vkCmdBindPipeline(getCmdBuffer(mActiveIndex).mCmdBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, shad->getPipeline());
    }

    void VulkanCommandBuffer::bindMaterial(core::Resource<Shader> shader, core::Resource<Material> material)
    {
        VulkanShader* shad = (VulkanShader*)(shader.get());
        VulkanMaterial* mat = (VulkanMaterial*)(material.get());

        VkDescriptorSet descSet = mat->getDescriptorSet();

        vkCmdBindDescriptorSets(getCmdBuffer(mActiveIndex).mCmdBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, shad->getPipelineLayout(), 0U, 1U, &descSet, 0U, VK_NULL_HANDLE);
    }

    void VulkanCommandBuffer::setShaderConstant(core::Resource<Shader> shader, U8* data, U32 size)
    {
        VulkanShader* shad = (VulkanShader*)(shader.get());

        vkCmdPushConstants(getCmdBuffer(mActiveIndex).mCmdBuffer, shad->getPipelineLayout(), VK_SHADER_STAGE_ALL, 0U, size, data);
    }

    void VulkanCommandBuffer::setViewport(U32 x, U32 y)
    {
        if (x == 0U || y == 0U)
        {
            x = 1U;
            y = 1U;
        }

        VkViewport vp = {};
        vp.x = 0.0F;
        vp.y = static_cast<F32>(y);
        vp.width = static_cast<F32>(x);
        vp.height = -static_cast<F32>(y);
        vp.minDepth = 0.0F;
        vp.maxDepth = 1.0F;
        vkCmdSetViewport(getCmdBuffer(mActiveIndex).mCmdBuffer, 0U, 1U, &vp);

        VkRect2D r = {};
        r.extent.width = x;
        r.extent.height = y;
        r.offset.x = 0;
        r.offset.y = 0;
        vkCmdSetScissor(getCmdBuffer(mActiveIndex).mCmdBuffer, 0U, 1U, &r);
    }

    void VulkanCommandBuffer::draw(core::Resource<Buffer> buffer)
    {
        auto& cmdBuf = getCmdBuffer(mActiveIndex);
        mDrawCount++;
        VulkanBuffer* buf = (VulkanBuffer*)buffer.get();
        VkBuffer bb = buf->getBuffer()->mBuffer;

        cmdBuf.mBufferReferences.push_back(buf->getBuffer());

        VkDeviceSize offset = 0U;
        mVertCount += buf->getVertexCount();

        vkCmdBindVertexBuffers(cmdBuf.mCmdBuffer, 0U, 1U, &bb, &offset);
        vkCmdDraw(cmdBuf.mCmdBuffer, buf->getVertexCount(), 1U, 0U, 0U);
    }

    void VulkanCommandBuffer::drawIndexed(core::Resource<Buffer> buffer, core::Resource<Buffer> index)
    {
        auto& cmdBuf = getCmdBuffer(mActiveIndex);

        mDrawCount++;
        VulkanBuffer* buf = (VulkanBuffer*)buffer.get();
        VkBuffer bb = buf->getBuffer()->mBuffer;

        cmdBuf.mBufferReferences.push_back(buf->getBuffer());

        VulkanBuffer* idx = (VulkanBuffer*)index.get();
        VkBuffer ib = idx->getBuffer()->mBuffer;

        cmdBuf.mBufferReferences.push_back(idx->getBuffer());

        VkDeviceSize offset = 0U;

        vkCmdBindVertexBuffers(cmdBuf.mCmdBuffer, 0U, 1U, &bb, &offset);
        vkCmdBindIndexBuffer(cmdBuf.mCmdBuffer, ib, 0u, VkIndexType::VK_INDEX_TYPE_UINT32);

        mVertCount += buf->getVertexCount();
        mTriCount += idx->getVertexCount();

        vkCmdDrawIndexed(cmdBuf.mCmdBuffer, idx->getVertexCount() * 3, 1u, 0u, 0u, 0u);
    }

    CmdBuffer& VulkanCommandBuffer::getCmdBuffer(U32 index)
    {
        return mCmdBuffers[index];
    }


    void VulkanCommandBuffer::createVkResources()
    {
        mCmdBuffers.resize(mSwapCount);

        VkCommandBufferAllocateInfo cmdAllocInfo = {};

        cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdAllocInfo.pNext = VK_NULL_HANDLE;
        cmdAllocInfo.commandPool = mVkObjects.mLogicalDevice->getCommandPool();
        cmdAllocInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdAllocInfo.commandBufferCount = 1u;

        VkFenceCreateInfo fenceInfo = {};

        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = VK_NULL_HANDLE;
        fenceInfo.flags = VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT;

        for (U32 i = 0u; i < mSwapCount; ++i)
        {
            VkResult result = vkAllocateCommandBuffers(mVkObjects.mLogicalDevice->getLogical(), &cmdAllocInfo, &mCmdBuffers[i].mCmdBuffer);

            if (result != VK_SUCCESS)
            {
                LOG_ERROR("Error creating CommandBuffer %s", vk::VkResultToString(result));
            }

            result = vkCreateFence(mVkObjects.mLogicalDevice->getLogical(), &fenceInfo, mVkObjects.mDebugAllocCallbacks, &mCmdBuffers[i].mFence);
            if (result != VK_SUCCESS)
            {
                LOG_ERROR("Error creating cmd buffer Fence %s", vk::VkResultToString(result));
            }
        }

        mActiveIndex = 0u;

    }

}