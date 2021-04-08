
#include <common/Types.hpp>

#include "VulkanPhysicalDevice.hpp"

#include "VulkanCmdBuffer.hpp"
#include "VulkanSwapchain.hpp"
#include "shader/VulkanShader.hpp"
#include "shader/VulkanMaterial.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanTexture.hpp"

#include <swizzle/core/Logging.hpp>

namespace swizzle::gfx
{

    VulkanTexture* gTexturePtr2;
    VkSampler gSampler2;

    VulkanCommandBuffer::VulkanCommandBuffer(const VkContainer vkObjects, U32 swapCount)
        : mVkObjects(vkObjects)
        , mSwapCount(swapCount)
        , mCmdBuffer()
        , mCompleteFence()
        , mActiveBuffer(VK_NULL_HANDLE)
        , mNextFence(VK_NULL_HANDLE)
        , mSelected(0u)
    {
        createVkResources();

        gTexturePtr2 = new VulkanTexture(mVkObjects, 1024U, 1024U);

        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.pNext = VK_NULL_HANDLE;
        samplerInfo.flags = 0;
        samplerInfo.magFilter = VkFilter::VK_FILTER_NEAREST;
        samplerInfo.minFilter = VkFilter::VK_FILTER_NEAREST;
        samplerInfo.mipmapMode = VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST;
        samplerInfo.addressModeU = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.addressModeV = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.addressModeW = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.mipLodBias = 0.0F;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 0.0F;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VkCompareOp::VK_COMPARE_OP_ALWAYS;
        samplerInfo.minLod = 0.0F;
        samplerInfo.maxLod = 0.0F;
        samplerInfo.borderColor = VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        vkCreateSampler(mVkObjects.mLogicalDevice->getLogical(), &samplerInfo, mVkObjects.mDebugAllocCallbacks, &gSampler2);

    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        for (U32 i = 0u; i < mSwapCount; ++i)
        {
            vkDestroyFence(mVkObjects.mLogicalDevice->getLogical(), mCompleteFence[i], mVkObjects.mDebugAllocCallbacks);
        }

        vkFreeCommandBuffers(mVkObjects.mLogicalDevice->getLogical(), mVkObjects.mLogicalDevice->getCommandPool(), mSwapCount, mCmdBuffer.data());
    }

    void VulkanCommandBuffer::reset(bool hardReset)
    {
        if (hardReset)
        {
            vkResetCommandBuffer(mActiveBuffer,
                VkCommandBufferResetFlagBits::VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        }
        else
        {
            vkResetCommandBuffer(mActiveBuffer, 0);
        }
    }

    void VulkanCommandBuffer::begin(core::Resource<Swapchain> swp)
    {
        if (swp)
        {
            //VulkanSwapchain* swapchain = (VulkanSwapchain*)(swp.get());
            //VkFence wait = swapchain->getWaitFence();

            //vkWaitForFences(mVkObjects.mLogicalDevice->getLogical(), 1U, &wait, VK_TRUE, UINT64_MAX);
            //mActiveBuffer = mCmdBuffer[swapchain->getCurrentFrame()];
        }
        else
        {
        }
        vkWaitForFences(mVkObjects.mLogicalDevice->getLogical(), 1U, &mNextFence, VK_TRUE, UINT64_MAX);
        vkResetFences(mVkObjects.mLogicalDevice->getLogical(), 1U, &mNextFence);

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

        vkBeginCommandBuffer(mActiveBuffer, &beginInfo);

        if (!gTexturePtr2->isUploaded())
        {
            gTexturePtr2->uploadImage(mActiveBuffer);
        }

    }

    void VulkanCommandBuffer::end()
    {
        vkEndCommandBuffer(mActiveBuffer);
    }

    void VulkanCommandBuffer::uploadTexture(core::Resource<Texture> texture)
    {
        VulkanTexture* tex = (VulkanTexture*)(texture.get());
        if (!tex->isUploaded())
        {
            tex->uploadImage(mActiveBuffer);
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

        VkSubmitInfo subinfo = {};
        subinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        subinfo.pNext = VK_NULL_HANDLE;
        subinfo.waitSemaphoreCount = 0u;
        subinfo.pWaitSemaphores = &wait;
        subinfo.pWaitDstStageMask = &waitStageMask;
        subinfo.commandBufferCount = 1U;
        subinfo.pCommandBuffers = &mActiveBuffer;
        subinfo.signalSemaphoreCount = waitCount;
        subinfo.pSignalSemaphores = &renderComplete;

        vkQueueSubmit(mVkObjects.mLogicalDevice->getQueue(0, 0), 1U, &subinfo, mNextFence);
        mSelected = (mSelected + 1u) % (U32)mCompleteFence.size();
        mActiveBuffer = mCmdBuffer[mSelected];
        mNextFence = mCompleteFence[mSelected];
        //if (waitFence != VK_NULL_HANDLE)
        //{
        //    mNextFence = waitFence;
        //}
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

        vkCmdBeginRenderPass(mActiveBuffer, &beginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanCommandBuffer::endRenderPass()
    {
        vkCmdEndRenderPass(mActiveBuffer);
    }

    void VulkanCommandBuffer::bindShader(core::Resource<Shader> shader)
    {
        VulkanShader2* shad = (VulkanShader2*)(shader.get());

        vkCmdBindPipeline(mActiveBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, shad->getPipeline());
    }

    void VulkanCommandBuffer::bindMaterial(core::Resource<Shader> shader, core::Resource<Material> material)
    {
        //static bool updated = false;
        VulkanShader2* shad = (VulkanShader2*)(shader.get());
        VulkanMaterial* mat = (VulkanMaterial*)(material.get());

        VkDescriptorSet descSet = mat->getDescriptorSet();

        //VulkanTexture* tex = (VulkanTexture*)(texture.get());

        /*VulkanBuffer* buffer = (VulkanBuffer*)material.get();

        VkDescriptorImageInfo descImg = {};
        descImg.sampler = gSampler2;
        descImg.imageView = gTexturePtr2->getView();
        descImg.imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkDescriptorBufferInfo descBfr = {};
        descBfr.buffer = buffer->getBuffer();
        descBfr.offset = 0;
        descBfr.range = sizeof(float) * 4;

        VkWriteDescriptorSet writeDesc = {};

        writeDesc.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDesc.pNext = VK_NULL_HANDLE;
        writeDesc.dstSet = descSet;
        writeDesc.dstBinding = 0;
        writeDesc.dstArrayElement = 0;
        writeDesc.descriptorCount = 1;
        writeDesc.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDesc.pImageInfo = &descImg;
        writeDesc.pBufferInfo = VK_NULL_HANDLE;
        writeDesc.pTexelBufferView = VK_NULL_HANDLE;

        if (!updated)
        {
            vkUpdateDescriptorSets(mVkObjects.mLogicalDevice->getLogical(), 1U, &writeDesc, 0U, VK_NULL_HANDLE);

            writeDesc.dstBinding = 1;
            writeDesc.descriptorCount = 1;
            writeDesc.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writeDesc.pImageInfo = VK_NULL_HANDLE;
            writeDesc.pBufferInfo = &descBfr;

            vkUpdateDescriptorSets(mVkObjects.mLogicalDevice->getLogical(), 1U, &writeDesc, 0U, VK_NULL_HANDLE);
            updated = true;
        }*/

        vkCmdBindDescriptorSets(mActiveBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, shad->getPipelineLayout(), 0U, 1U, &descSet, 0U, VK_NULL_HANDLE);
    }

    void VulkanCommandBuffer::setShaderConstant(core::Resource<Shader> shader, U8* data, U32 size)
    {
        VulkanShader2* shad = (VulkanShader2*)(shader.get());

        vkCmdPushConstants(mActiveBuffer, shad->getPipelineLayout(), VK_SHADER_STAGE_ALL, 0U, size, data);
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
        vkCmdSetViewport(mActiveBuffer, 0U, 1U, &vp);

        VkRect2D r = {};
        r.extent.width = x;
        r.extent.height = y;
        r.offset.x = 0;
        r.offset.y = 0;
        vkCmdSetScissor(mActiveBuffer, 0U, 1U, &r);
    }

    void VulkanCommandBuffer::draw(core::Resource<Buffer> buffer)
    {
        VulkanBuffer* buf = (VulkanBuffer*)buffer.get();
        VkBuffer bb = buf->getBuffer();

        VkDeviceSize offset = 0U;

        vkCmdBindVertexBuffers(mActiveBuffer, 0U, 1U, &bb, &offset);
        vkCmdDraw(mActiveBuffer, buf->getVertexCount(), 1U, 0U, 0U);
    }

    void VulkanCommandBuffer::createVkResources()
    {
        mCmdBuffer.resize(mSwapCount);
        mCompleteFence.resize(mSwapCount);

        VkCommandBufferAllocateInfo cmdAllocInfo = {};

        cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdAllocInfo.pNext = VK_NULL_HANDLE;
        cmdAllocInfo.commandPool = mVkObjects.mLogicalDevice->getCommandPool();
        cmdAllocInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdAllocInfo.commandBufferCount = mSwapCount;

        vkAllocateCommandBuffers(mVkObjects.mLogicalDevice->getLogical(), &cmdAllocInfo, mCmdBuffer.data());
        mActiveBuffer = mCmdBuffer[0];

        VkFenceCreateInfo fenceInfo = {};

        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = VK_NULL_HANDLE;
        fenceInfo.flags = VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT;

        for (U32 i = 0u; i < mSwapCount; ++i)
        {
            VkResult result = vkCreateFence(mVkObjects.mLogicalDevice->getLogical(), &fenceInfo, mVkObjects.mDebugAllocCallbacks, &mCompleteFence[i]);
            if (result != VK_SUCCESS)
            {
                LOG_ERROR("Error creating Queue fence %s", vk::VkResultToString(result));
            }
        }

        mNextFence = mCompleteFence[0];

    }

}