
#include "VulkanCmdBuffer.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanShader.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanTexture.hpp"

#include <swizzle/core/Logging.hpp>

namespace swizzle::gfx
{
    VulkanTexture* gTexturePtr;
    VkSampler gSampler;

    VulkanCommandBuffer::VulkanCommandBuffer(const VulkanObjectContainer& vkObjects)
        : mVkObjects(vkObjects)
        , mCmdBuffer(VK_NULL_HANDLE)
        , mCompleteFence(VK_NULL_HANDLE)
    {
        VkCommandBufferAllocateInfo cmdAllocInfo = {};

        cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdAllocInfo.pNext = VK_NULL_HANDLE;
        cmdAllocInfo.commandPool = mVkObjects.mCmdPool;
        cmdAllocInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdAllocInfo.commandBufferCount = 1;

        vkAllocateCommandBuffers(mVkObjects.mLogicalDevice, &cmdAllocInfo, &mCmdBuffer);

        VkFenceCreateInfo fenceInfo;

        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = VK_NULL_HANDLE;
        fenceInfo.flags = VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT;

        VkResult result = vkCreateFence(mVkObjects.mLogicalDevice, &fenceInfo, mVkObjects.mDebugAllocCallbacks, &mCompleteFence);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Error creating swapchain fence %s", vk::VkResultToString(result));
        }

        gTexturePtr = new VulkanTexture(mVkObjects, 1024U, 1024U);

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

        vkCreateSampler(mVkObjects.mLogicalDevice, &samplerInfo, mVkObjects.mDebugAllocCallbacks, &gSampler);
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        vkDestroySampler(mVkObjects.mLogicalDevice, gSampler, mVkObjects.mDebugAllocCallbacks);
        delete gTexturePtr;

        vkDestroyFence(mVkObjects.mLogicalDevice, mCompleteFence, mVkObjects.mDebugAllocCallbacks);

        vkFreeCommandBuffers(mVkObjects.mLogicalDevice, mVkObjects.mCmdPool, 1, &mCmdBuffer);
    }

    void VulkanCommandBuffer::reset(bool hardReset)
    {
        if (hardReset)
        {
            vkResetCommandBuffer(mCmdBuffer,
                VkCommandBufferResetFlagBits::VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        }
        else
        {
            vkResetCommandBuffer(mCmdBuffer, 0);
        }
    }

    void VulkanCommandBuffer::begin()
    {
        vkWaitForFences(mVkObjects.mLogicalDevice, 1U, &mCompleteFence, VK_TRUE, UINT64_MAX);
        vkResetFences(mVkObjects.mLogicalDevice, 1U, &mCompleteFence);

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

        vkBeginCommandBuffer(mCmdBuffer, &beginInfo);
    }

    void VulkanCommandBuffer::end()
    {
        vkEndCommandBuffer(mCmdBuffer);
    }

    void VulkanCommandBuffer::submit(core::Resource<Swapchain> swp)
    {
        U32 waitCount = 0U;
        VkSemaphore sync = VK_NULL_HANDLE;

        if (swp)
        {
            VulkanSwapchain* swapchain = (VulkanSwapchain*)(swp.get());
            sync = swapchain->getSemaphore();
            waitCount = 1U;
        }

        // TODO: move into one sumbit call
        if (mVkObjects.stageCmdBuffer->readyToSubmit())
        {
            mVkObjects.stageCmdBuffer->endAndSubmitRecording();
        }

        VkSubmitInfo subinfo;
        subinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        subinfo.pNext = 0;
        subinfo.waitSemaphoreCount = 0U;
        subinfo.pWaitSemaphores = VK_NULL_HANDLE;
        subinfo.pWaitDstStageMask = VK_NULL_HANDLE;
        subinfo.commandBufferCount = 1U;
        subinfo.pCommandBuffers = &mCmdBuffer;
        subinfo.signalSemaphoreCount = waitCount;
        subinfo.pSignalSemaphores = &sync;

        vkQueueSubmit(mVkObjects.mQueue, 1U, &subinfo, mCompleteFence);
    }

    void VulkanCommandBuffer::beginRenderPass(core::Resource<Swapchain> swp)
    {
        VulkanSwapchain* swapchain = (VulkanSwapchain*)(swp.get());
        beginRenderPass(swapchain->getFrameBuffer());
    }

    void VulkanCommandBuffer::beginRenderPass(core::Resource<FrameBuffer> fbo)
    {
        if (!gTexturePtr->isUploaded())
        {
            gTexturePtr->uploadImage(mCmdBuffer);
        }

        PresentFrameBuffer* present = (PresentFrameBuffer*)(fbo.get());

        VkClearValue clears[] = { present->mClearValue , present->mDepthClearValue };

        VkRenderPassBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.pNext = VK_NULL_HANDLE;
        beginInfo.renderPass = present->mRenderPass;
        beginInfo.framebuffer = present->mFrameBuffer;
        beginInfo.renderArea = { 0U, 0U, present->mWidth, present->mHeight };
        beginInfo.clearValueCount = 2U;
        beginInfo.pClearValues = clears;

        vkCmdBeginRenderPass(mCmdBuffer, &beginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanCommandBuffer::endRenderPass()
    {
        vkCmdEndRenderPass(mCmdBuffer);
    }

    void VulkanCommandBuffer::bindShader(core::Resource<Shader> shader)
    {
        VulkanShader* shad = (VulkanShader*)(shader.get());

        vkCmdBindPipeline(mCmdBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, shad->getPipeline());
    }

    void VulkanCommandBuffer::bindMaterial(core::Resource<Shader> shader, core::Resource<Buffer> material)
    {
        VulkanShader* shad = (VulkanShader*)(shader.get());

        //VulkanTexture* tex = (VulkanTexture*)(texture.get());

        VulkanBuffer* buffer = (VulkanBuffer*)material.get();

        /*if (!tex->isUploaded())
        {
            tex->upload();
        }*/

        VkDescriptorSet descSet = shad->getDescriptorSet();

        VkDescriptorImageInfo descImg = {};
        descImg.sampler = gSampler;
        descImg.imageView = gTexturePtr->getView();
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

        vkUpdateDescriptorSets(mVkObjects.mLogicalDevice, 1U, &writeDesc, 0U, VK_NULL_HANDLE);

        writeDesc.dstBinding = 1;
        writeDesc.descriptorCount = 1;
        writeDesc.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDesc.pImageInfo = VK_NULL_HANDLE;
        writeDesc.pBufferInfo = &descBfr;

        vkUpdateDescriptorSets(mVkObjects.mLogicalDevice, 1U, &writeDesc, 0U, VK_NULL_HANDLE);

        vkCmdBindDescriptorSets(mCmdBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, shad->getPipelineLayout(), 0U, 1U, &descSet, 0U, VK_NULL_HANDLE);
    }

    void VulkanCommandBuffer::setShaderConstant(core::Resource<Shader> shader, U8* data, U32 size)
    {
        VulkanShader* shad = (VulkanShader*)(shader.get());

        vkCmdPushConstants(mCmdBuffer, shad->getPipelineLayout(), VK_SHADER_STAGE_ALL, 0U, size, data);
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
        vp.y = static_cast<float_t>(y);
        vp.width = static_cast<float_t>(x);
        vp.height = -static_cast<float_t>(y);
        vp.minDepth = 0.0F;
        vp.maxDepth = 1.0F;
        vkCmdSetViewport(mCmdBuffer, 0U, 1U, &vp);

        VkRect2D r = {};
        r.extent.width = x;
        r.extent.height = y;
        r.offset.x = 0;
        r.offset.y = 0;
        vkCmdSetScissor(mCmdBuffer, 0U, 1U, &r);
    }

    void VulkanCommandBuffer::draw(core::Resource<Buffer> buffer)
    {
        VulkanBuffer* buf = (VulkanBuffer*)buffer.get();
        VkBuffer bb = buf->getBuffer();

        VkDeviceSize offset = 0U;

        vkCmdBindVertexBuffers(mCmdBuffer, 0U, 1U, &bb, &offset);
        vkCmdDraw(mCmdBuffer, buf->getVertexCount(), 1U, 0U, 0U);
    }

}