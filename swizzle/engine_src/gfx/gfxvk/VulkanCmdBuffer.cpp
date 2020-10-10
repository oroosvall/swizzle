
#include "VulkanCmdBuffer.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanShader.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanTexture.hpp"

#include <swizzle/core/logging/Logging.hpp>

namespace swizzle::gfx
{
    VulkanTexture* gTexturePtr;
    VkSampler gSampler;

    VulkanCommandBuffer::VulkanCommandBuffer(const VulkanObjectContainer& vkObjects)
        : mVkObjects(vkObjects)
        , mCmdBuffer(VK_NULL_HANDLE)
        , mCompleteFence(VK_NULL_HANDLE)
    {
        VkCommandBufferAllocateInfo cmdAllocInfo;

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

        VkResult result = vkCreateFence(mVkObjects.mLogicalDevice, &fenceInfo, nullptr, &mCompleteFence);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Error creating swapchain fence %s", VkResultToString(result));
        }

        gTexturePtr = new VulkanTexture(mVkObjects);

        VkSamplerCreateInfo samplerInfo;
        samplerInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.pNext = VK_NULL_HANDLE;
        samplerInfo.flags = 0;
        samplerInfo.magFilter = VkFilter::VK_FILTER_LINEAR;
        samplerInfo.minFilter = VkFilter::VK_FILTER_LINEAR;
        samplerInfo.mipmapMode = VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.addressModeU = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.mipLodBias = 0.0F;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 0.0F;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VkCompareOp::VK_COMPARE_OP_ALWAYS;
        samplerInfo.minLod = 0.0F;
        samplerInfo.maxLod = 1.0F;
        samplerInfo.borderColor = VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        vkCreateSampler(mVkObjects.mLogicalDevice, &samplerInfo, nullptr, &gSampler);

    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        vkDestroySampler(mVkObjects.mLogicalDevice, gSampler, nullptr);
        delete gTexturePtr;

        vkDestroyFence(mVkObjects.mLogicalDevice, mCompleteFence, nullptr);

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
        vkWaitForFences(mVkObjects.mLogicalDevice, 1, &mCompleteFence, VK_TRUE, UINT64_MAX);
        vkResetFences(mVkObjects.mLogicalDevice, 1, &mCompleteFence);

        VkCommandBufferBeginInfo beginInfo;

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
        U32 waitCount = 0;
        VkSemaphore sync = VK_NULL_HANDLE;

        if (swp)
        {
            VulkanSwapchain* swapchain = (VulkanSwapchain*)(swp.get());
            sync = swapchain->getSemaphore();
            waitCount = 1;
        }

        VkSubmitInfo subinfo;
        subinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        subinfo.pNext = 0;
        subinfo.waitSemaphoreCount = 0;
        subinfo.pWaitSemaphores = VK_NULL_HANDLE;
        subinfo.pWaitDstStageMask = VK_NULL_HANDLE;
        subinfo.commandBufferCount = 1;
        subinfo.pCommandBuffers = &mCmdBuffer;
        subinfo.signalSemaphoreCount = waitCount;
        subinfo.pSignalSemaphores = &sync;

        vkQueueSubmit(mVkObjects.mQueue, 1, &subinfo, mCompleteFence);
    }

    void VulkanCommandBuffer::beginRenderPass(core::Resource<Swapchain> swp)
    {
        VulkanSwapchain* swapchain = (VulkanSwapchain*)(swp.get());
        beginRenderPass(swapchain->getFrameBuffer());
    }

    void VulkanCommandBuffer::beginRenderPass(core::Resource<FrameBuffer> fbo)
    {
        if (!gTexturePtr->uploaded)
        {
            VkImageMemoryBarrier imgBarrier;
            imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imgBarrier.pNext = VK_NULL_HANDLE;
            imgBarrier.srcAccessMask = 0;
            imgBarrier.dstAccessMask = 0;
            imgBarrier.oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_PREINITIALIZED;
            imgBarrier.newLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            imgBarrier.image = gTexturePtr->mImage;
            imgBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imgBarrier.subresourceRange.baseMipLevel = 0;
            imgBarrier.subresourceRange.levelCount = 1;
            imgBarrier.subresourceRange.baseArrayLayer = 0;
            imgBarrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(mCmdBuffer,
                VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
                0,
                0, VK_NULL_HANDLE,
                0, VK_NULL_HANDLE,
                1, &imgBarrier);

            VkBufferImageCopy region{};
            region.bufferOffset = 0;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;

            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;

            region.imageOffset = { 0, 0, 0 };
            region.imageExtent = {
                gTexturePtr->mWidth,
                gTexturePtr->mHeight,
                1
            };

            vkCmdCopyBufferToImage(mCmdBuffer, gTexturePtr->mStageBuffer, gTexturePtr->mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region);

            imgBarrier.oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            imgBarrier.newLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            vkCmdPipelineBarrier(mCmdBuffer,
                VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
                0,
                0, VK_NULL_HANDLE,
                0, VK_NULL_HANDLE,
                1, &imgBarrier);

            gTexturePtr->uploaded = true;
        }

        PresentFrameBuffer* present = (PresentFrameBuffer*)(fbo.get());

        VkRenderPassBeginInfo beginInfo;
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.pNext = VK_NULL_HANDLE;
        beginInfo.renderPass = present->mRenderPass;
        beginInfo.framebuffer = present->mFrameBuffer;
        beginInfo.renderArea = { 0,0, present->mWidth, present->mHeight };
        beginInfo.clearValueCount = 1;
        beginInfo.pClearValues = &present->mClearValue;

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

        VkDescriptorSet descSet = shad->getDescriptorSet();

        VkWriteDescriptorSet writeDesc;

        VkDescriptorImageInfo descImg;
        descImg.sampler = gSampler;
        descImg.imageView = gTexturePtr->getView();
        descImg.imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

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

        vkUpdateDescriptorSets(mVkObjects.mLogicalDevice, 1, &writeDesc, 0, VK_NULL_HANDLE);

        vkCmdBindDescriptorSets(mCmdBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, shad->getPipelineLayout(), 0, 1, &descSet, 0, VK_NULL_HANDLE);
    }

    void VulkanCommandBuffer::setShaderConstant(core::Resource<Shader> shader, U8* data, U32 size)
    {
        VulkanShader* shad = (VulkanShader*)(shader.get());

        vkCmdPushConstants(mCmdBuffer, shad->getPipelineLayout(), VK_SHADER_STAGE_ALL, 0, size, data);
    }

    void VulkanCommandBuffer::setViewport(U32 x, U32 y)
    {
        if (x == 0U || y == 0U)
        {
            x = 1U;
            y = 1U;
        }
        VkViewport vp;
        vp.x = 0;
        vp.y = static_cast<float_t>(y);
        vp.width = static_cast<float_t>(x);
        vp.height = -static_cast<float_t>(y);
        vp.minDepth = 0.0F;
        vp.maxDepth = 1.0F;
        vkCmdSetViewport(mCmdBuffer, 0, 1, &vp);
        VkRect2D r;
        r.extent.width = x;
        r.extent.height = y;
        r.offset.x = 0;
        r.offset.y = 0;
        vkCmdSetScissor(mCmdBuffer, 0, 1, &r);
    }

    void VulkanCommandBuffer::draw(core::Resource<Buffer> buffer)
    {
        VulkanBuffer* buf = (VulkanBuffer*)buffer.get();
        VkBuffer bb = buf->getBuffer();

        VkDeviceSize offset = 0U;

        vkCmdBindVertexBuffers(mCmdBuffer, 0, 1, &bb, &offset);
        vkCmdDraw(mCmdBuffer, buf->getVertexCount(), 1U, 0U, 0U);
    }

}