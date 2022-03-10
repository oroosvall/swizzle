
/* Include files */

#include "CmdBuffer.hpp"
#include "Device.hpp"
#include "CmdPool.hpp"
#include "DBuffer.hpp"

#include "VSwapchain.hpp"
#include "ShaderPipeline.hpp"
#include "TextureBase.hpp"
#include "VMaterial.hpp"
#include "Fence.hpp"

#include <optick/optick.h>

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
        , mScissorInfo()
    {
        mCommandBuffers = mCmdPool->allocateCmdBuffer(mBufferCount, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        mFences = new common::Resource<vk::Fence>[mBufferCount];
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

        if ( (frameCount + mBufferCount + 1u) < mFrameCounter)
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
            VkResult res = vkResetCommandBuffer(getCmdBuffer(mActiveIndex),
                                 VkCommandBufferResetFlagBits::VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
            vk::LogVulkanError(res, "vkResetCommandBuffer");
        }
        else
        {
            VkResult res = vkResetCommandBuffer(getCmdBuffer(mActiveIndex), 0);
            vk::LogVulkanError(res, "vkResetCommandBuffer");
        }
    }

    void CmdBuffer::begin()
    {
        mDrawCount = 0u;
        OPTICK_EVENT("CmdBuffer::begin");
        mFrameCounter++;

        auto fence = getFence(mActiveIndex);
        if (fence)
        {
            VkFence f = fence->getHandle();
            vkWaitForFences(mDevice->getDeviceHandle(), 1U, &f, VK_TRUE, UINT64_MAX);
            fence->removeUser(shared_from_this());
            //VkResult res = vkResetFences(mDevice->getDeviceHandle(), 1U, &fence);
            //vk::LogVulkanError(res, "vkResetFences");
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

        mScissorInfo.enableCtr = 0u;
    }

    void CmdBuffer::end()
    {
        OPTICK_EVENT("CmdBuffer::end");
        VkResult res = vkEndCommandBuffer(getCmdBuffer(mActiveIndex));
        vk::LogVulkanError(res, "vkBeginCommandBuffer");
    }

    void CmdBuffer::uploadTexture(common::Resource<swizzle::gfx::Texture> texture)
    {
        OPTICK_EVENT("CmdBuffer::uploadTexture");
        TextureBase* tex = (TextureBase*)(texture.get());
        if (!tex->isUploaded())
        {
            tex->uploadImage(getCmdBuffer(mActiveIndex));
        }
    }

    void CmdBuffer::beginRenderPass(common::Resource<swizzle::gfx::Swapchain> swp)
    {
        VSwapchain* swapchain = (VSwapchain*)(swp.get());
        beginRenderPass(swapchain->getFrameBuffer());
    }

    void CmdBuffer::beginRenderPass(common::Resource<swizzle::gfx::FrameBuffer> fbo)
    {
        OPTICK_EVENT("CmdBuffer::beginRenderPass");
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

        vkCmdBeginRenderPass(getCmdBuffer(mActiveIndex), &beginInfo,
                             VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
    }

    void CmdBuffer::endRenderPass()
    {
        vkCmdEndRenderPass(getCmdBuffer(mActiveIndex));
    }

    void CmdBuffer::bindShader(common::Resource<swizzle::gfx::Shader> shader)
    {
        ShaderPipeline* shad = (ShaderPipeline*)(shader.get());

        vkCmdBindPipeline(getCmdBuffer(mActiveIndex), VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
                          shad->getPipeline());
    }

    void CmdBuffer::bindMaterial(common::Resource<swizzle::gfx::Shader> shader,
                               common::Resource<swizzle::gfx::Material> material)
    {
        OPTICK_EVENT("CmdBuffer::bindMaterial");
        ShaderPipeline* shad = (ShaderPipeline*)(shader.get());
        VMaterial* mat = (VMaterial*)(material.get());

        VkDescriptorSet descSet = mat->getDescriptorSet();

        vkCmdBindDescriptorSets(getCmdBuffer(mActiveIndex),
                                VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, shad->getPipelineLayout(), 0U, 1U,
                                &descSet, 0U, VK_NULL_HANDLE);
    }

    void CmdBuffer::setShaderConstant(common::Resource<swizzle::gfx::Shader> shader, U8* data, U32 size)
    {
        OPTICK_EVENT("CmdBuffer::setShaderConstant");
        ShaderPipeline* shad = (ShaderPipeline*)(shader.get());

        vkCmdPushConstants(getCmdBuffer(mActiveIndex), shad->getPipelineLayout(), VK_SHADER_STAGE_ALL, 0U,
                           size, data);
    }

    void CmdBuffer::setViewport(U32 x, U32 y)
    {
        OPTICK_EVENT("CmdBuffer::setViewport");
        if (x == 0U || y == 0U)
        {
            x = 1U;
            y = 1U;
        }

        VkCommandBuffer cmdBuf = getCmdBuffer(mActiveIndex);

        VkViewport vp = {};
        vp.x = 0.0F;
        vp.y = static_cast<F32>(y);
        vp.width = static_cast<F32>(x);
        vp.height = -static_cast<F32>(y);
        vp.minDepth = 0.0F;
        vp.maxDepth = 1.0F;
        vkCmdSetViewport(cmdBuf, 0U, 1U, &vp);

        VkRect2D r = {};
        r.extent.width = x;
        r.extent.height = y;
        r.offset.x = 0;
        r.offset.y = 0;

        mScissorInfo.x = 0;
        mScissorInfo.y = 0;
        mScissorInfo.w = x;
        mScissorInfo.h = y;

        vkCmdSetScissor(cmdBuf, 0U, 1U, &r);
    }

    void CmdBuffer::bindVertexBuffer(common::Resource<swizzle::gfx::Buffer> buffer)
    {
        OPTICK_EVENT("CmdBuffer::bindVertexBuffer");
        DBuffer* buff = (DBuffer*)buffer.get();

        auto& res = buff->getBuffer();
        common::Resource<CmdBuffer> self = shared_from_this();
        res->addUser(self, mFrameCounter);
        VkBuffer vkBuf = res->getVkHandle();
        VkDeviceSize offset = 0u;

        vkCmdBindVertexBuffers(getCmdBuffer(mActiveIndex), 0U, 1U, &vkBuf, &offset);
    }

    void CmdBuffer::bindIndexBuffer(common::Resource<swizzle::gfx::Buffer> buffer, SwBool bitSize16)
    {
        OPTICK_EVENT("CmdBuffer::bindIndexBuffer");
        DBuffer* buff = (DBuffer*)buffer.get();

        auto& res = buff->getBuffer();
        common::Resource<CmdBuffer> self = shared_from_this();
        res->addUser(self, mFrameCounter);
        VkBuffer idxBuf = res->getVkHandle();
        VkDeviceSize offset = 0u;

        VkIndexType size[] = { VkIndexType::VK_INDEX_TYPE_UINT32, VkIndexType::VK_INDEX_TYPE_UINT16 };

        vkCmdBindIndexBuffer(getCmdBuffer(mActiveIndex), idxBuf, offset, size[bitSize16]);
    }

    void CmdBuffer::draw(common::Resource<swizzle::gfx::Buffer> buffer)
    {
        OPTICK_EVENT("CmdBuffer::draw");
        mDrawCount++;
        DBuffer* buff = (DBuffer*)buffer.get();

        auto& res = buff->getBuffer();
        common::Resource<CmdBuffer> self = shared_from_this();
        res->addUser(self, mFrameCounter);
        VkBuffer vkBuf = res->getVkHandle();
        VkDeviceSize offset = 0u;

        vkCmdBindVertexBuffers(getCmdBuffer(mActiveIndex), 0U, 1U, &vkBuf, &offset);
        vkCmdDraw(getCmdBuffer(mActiveIndex), buff->getVertexCount(), 1U, 0U, 0U);
    }

    void CmdBuffer::drawIndexed(common::Resource<swizzle::gfx::Buffer> buffer,
                              common::Resource<swizzle::gfx::Buffer> index)
    {
        OPTICK_EVENT("CmdBuffer::drawIndexed");
        mDrawCount++;
        DBuffer* buff = (DBuffer*)buffer.get();
        DBuffer* idxBuff = (DBuffer*)index.get();

        auto& resVert = buff->getBuffer();
        auto& resIdx = idxBuff->getBuffer();
        resVert->addUser(shared_from_this(), mFrameCounter);
        resIdx->addUser(shared_from_this(), mFrameCounter);
        VkBuffer vertBuf = resVert->getVkHandle();
        VkBuffer idxBuf = resIdx->getVkHandle();

        VkDeviceSize offset = 0u;

        vkCmdBindVertexBuffers(getCmdBuffer(mActiveIndex), 0U, 1U, &vertBuf, &offset);
        vkCmdBindIndexBuffer(getCmdBuffer(mActiveIndex), idxBuf, offset, VkIndexType::VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(getCmdBuffer(mActiveIndex), idxBuff->getVertexCount() * 3u, 1u,  0u, 0u, 0u);
    }

    void CmdBuffer::drawNoBind(U32 vertexCount, U32 first)
    {
        vkCmdDraw(getCmdBuffer(mActiveIndex), vertexCount, 1u, first, 0u);
    }

    void CmdBuffer::drawIndexedNoBind(U32 vertexCount, U32 first, U32 vertOffset)
    {
        vkCmdDrawIndexed(getCmdBuffer(mActiveIndex), vertexCount, 1u, first, vertOffset, 0u);
    }

    const swizzle::gfx::ScissorInfo CmdBuffer::pushScissorRegion(S32 x, S32 y, S32 w, S32 h)
    {
        swizzle::gfx::ScissorInfo org = mScissorInfo;

        mScissorInfo.enableCtr++;
        mScissorInfo.x = x;
        mScissorInfo.y = y;
        mScissorInfo.w = w;
        mScissorInfo.h = h;

        VkRect2D scissor
        {
            x, y, (U32)w, (U32)h
        };
        vkCmdSetScissor(getCmdBuffer(mActiveIndex), 0, 1, &scissor);

        return org;
    }

    void CmdBuffer::popScisorInfo(const swizzle::gfx::ScissorInfo& sci)
    {
        if (mScissorInfo.enableCtr == sci.enableCtr + 1)
        {
            mScissorInfo = sci;
            VkRect2D scissor
            {
                sci.x, sci.y, (U32)sci.w, (U32)sci.h
            };
            vkCmdSetScissor(getCmdBuffer(mActiveIndex), 0, 1, &scissor);
        }
    }

    void CmdBuffer::setScissor(S32 x, S32 y, S32 w, S32 h)
    {
        VkRect2D scissor
        {
            x, y, (U32)w, (U32)h
        };
        vkCmdSetScissor(getCmdBuffer(mActiveIndex), 0, 1, &scissor);
    }

}

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{
    VkCommandBuffer CmdBuffer::getCmdBuffer(U32 index)
    {
        return mCommandBuffers[index];
    }

    common::Resource<vk::Fence> CmdBuffer::getFence(U32 index)
    //VkFence CmdBuffer::getFence(U32 index)
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

}
